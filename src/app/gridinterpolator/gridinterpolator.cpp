/*
** Copyright (C) 2010 Peter J. Ersts (ersts at amnh.org)
** Creation Date: 2010-02-22
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
** This work was partially funded by the National Aeronautics and Space Administration
** under award(s) NNX09AK19G, NNX09AK20G
*/
#include "gridinterpolator.h"

#include <QFile>
#include <QDateTime>
#include <QTextStream>

GridInterpolator::GridInterpolator()
{
  cvIsValid = false;
  cvLastProgressValue = 0;
}

GridInterpolator::~GridInterpolator()
{
  clearDataSets();
}

void GridInterpolator::run()
{
  cvAbort = false;
  if( cvIsValid )
  {
    QMap< QString, QMap< int, Raster* > >::const_iterator lvSpecies = cvDataSets.begin();
    while( lvSpecies != cvDataSets.end() )
    {
      emit consoleMessage( tr( "Starting species %1" ) .arg( lvSpecies.key() ) );
      QList< int > lvYears = lvSpecies.value().keys();
      int lvCounter = 0;
      while( lvCounter < lvYears.size() - 2 )
      {
        emit progress( 0 );
        emit consoleMessage( tr( "Interpolating %1 to %2, by %3" ) .arg( QString::number( lvYears[ lvCounter ] ) ) .arg( QString::number( lvYears[ lvCounter + 1 ] ) ) .arg( QString::number( cvStepSize ) ) );
        if( !interpolate( lvSpecies.key(), lvYears[ lvCounter ], cvDataSets[ lvSpecies.key()][ lvYears[ lvCounter ] ], lvYears[ lvCounter + 1 ], cvDataSets[ lvSpecies.key()][ lvYears[ lvCounter + 1 ] ], false ) )
        {
          consoleMessage( cvLastError );
          break;
        }
        lvCounter++;
      }

      emit progress( 0 );
      emit consoleMessage( tr( "Interpolating %1 to %2, by %3 extending %4 steps beyond last date" ) .arg( QString::number( lvYears[ lvCounter ] ) ) .arg( QString::number( lvYears[ lvCounter + 1 ] ) ) .arg( QString::number( cvStepSize ) ) .arg( QString::number( cvStepsBeyond ) ) );
      if( !interpolate( lvSpecies.key(), lvYears[ lvCounter ], cvDataSets[ lvSpecies.key()][ lvYears[ lvCounter ] ], lvYears[ lvCounter + 1 ], cvDataSets[ lvSpecies.key()][ lvYears[ lvCounter + 1 ] ], true ) )
      {
        consoleMessage( cvLastError );
        break;
      }
      lvSpecies++;
    }
  }
  emit finished();
}

bool GridInterpolator::setParameters( const QMap< QString, QMap< int, QString > > &theInputMap,  int theStepSize, int theStepsBeyond, double theThreshold, QString theOutputDirectory )
{
  cvIsValid = false;
  clearDataSets();

  cvStepSize = theStepSize;
  cvStepsBeyond = theStepsBeyond;
  cvThreshold = theThreshold;

  if( theOutputDirectory.isEmpty() )
  {
    cvLastError = tr( "The output directory is blank" );
    return false;
  }
  cvOutputDirectory = theOutputDirectory;

  if( 0 >= cvStepSize || 0 > cvStepsBeyond || 0.0 > cvThreshold )
  {
    cvLastError = tr( "Step size >= 1 and all other parameters must be >= 0" );
    return false;
  }

  QMap< QString, QMap< int, QString > >::const_iterator lvSpecies = theInputMap.begin();
  while( lvSpecies != theInputMap.end() )
  {
    cvDataSets[ lvSpecies.key()] = QMap< int, Raster* >();
    QMap< int, QString >::const_iterator lvFiles = lvSpecies.value().begin();
    while( lvFiles != lvSpecies.value().end() )
    {
      cvDataSets[ lvSpecies.key()][ lvFiles.key()] = new Raster( lvFiles.value(), true );
      if( !cvDataSets[ lvSpecies.key()][ lvFiles.key()]->isValid() )
      {
        cvLastError =  cvDataSets[ lvSpecies.key()][ lvFiles.key()]->lastError();
        clearDataSets();
        return false;
      }
      lvFiles++;
    }

    if( 1 >= cvDataSets[ lvSpecies.key()].size() )
    {
      cvLastError =  tr( "Each species must have at least two valid raster layers" );
      cvDataSets.clear();
      return false;
    }

    QMap< int, Raster* >::const_iterator lvRasters = cvDataSets[ lvSpecies.key()].begin();
    Raster* lvRaster = lvRasters.value();
    lvRasters++;
    while( lvRasters != cvDataSets[ lvSpecies.key()].end() )
    {
      if( *lvRaster != *( lvRasters.value() ) )
      {
        cvLastError =  tr( "Raster layers for species [%1] do not have the same extent or core structure" ) .arg( lvSpecies.key() );
        clearDataSets();
        return false;
      }
      lvRasters++;
    }

    lvSpecies++;
  }

  cvOutputDirectory.setPath( theOutputDirectory );
  QDateTime lvDateTime = QDateTime::currentDateTime();
  QString lvNewDirectory;
  if( fabs( 0.000 - cvThreshold ) <= 0.000001 )
  {
    lvNewDirectory = lvDateTime.toString( "yyyyMMdd_hhmmss" );
  }
  else
  {
    lvNewDirectory = lvDateTime.toString( "yyyyMMdd_hhmmss_t-" + QString::number( cvThreshold, 'f', 3 ) );
  }

  if( !cvOutputDirectory.mkdir( lvNewDirectory ) )
  {
    cvLastError = tr( "Ouput directory is not writeable" );
    return false;
  }
  cvOutputDirectory.cd( lvNewDirectory );

  cvIsValid = true;
  return true;
}


/*
 * PRIVATE METHODS
 */
void GridInterpolator::clearDataSets()
{
  QMap< QString, QMap< int, Raster* > >::const_iterator lvSpecies = cvDataSets.begin();
  while( lvSpecies != cvDataSets.end() )
  {
    QMap< int, Raster* >::const_iterator lvRasters = lvSpecies.value().begin();
    while( lvRasters != lvSpecies.value().end() )
    {
      delete cvDataSets[ lvSpecies.key()][ lvRasters.key()];
      lvRasters++;
    }
    lvSpecies++;
  }
}

bool GridInterpolator::interpolate( QString theSpecies, int theStartYear, Raster* theStartRaster, int theEndYear, Raster* theEndRaster, bool extend )
{
  QMap< int, QFile* > lvRealFileMap;
  QMap< int, QFile* > lvBinaryFileMap;
  QTextStream lvOutputStream;
  bool lvHasError = false;
  int lvSteps = ( theEndYear - theStartYear ) / cvStepSize;
  extend == true ? lvSteps += cvStepsBeyond : false;

  const double* const lvGeotransform = theStartRaster->geotransform();

  for( int lvIterator = 0; lvIterator <= lvSteps; lvIterator++ )
  {
    if( cvAbort )
    {
      cvLastError = tr( "Canceled by user" );
      lvHasError = true;
      break;
    }
    lvRealFileMap[ lvIterator ] = new QFile( cvOutputDirectory.path() + QDir::toNativeSeparators( "/" ) + theSpecies + "_" + QString::number( theStartYear + ( lvIterator * cvStepSize ) ) + ".asc" );
    if( !lvRealFileMap[ lvIterator ]->open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
      cvLastError = tr( "Unable to create new output file" );
      lvHasError = true;
      break;
    }
    lvOutputStream.setDevice( lvRealFileMap[ lvIterator ] );
    lvOutputStream << "ncols " << theStartRaster->width() << "\n";
    lvOutputStream << "nrows " << theStartRaster->height() << "\n";
    lvOutputStream << "xllcorner " << QString::number( lvGeotransform[0], 'f', 10 ) << "\n";
    lvOutputStream << "yllcorner " << QString::number( lvGeotransform[3] + (( double )theStartRaster->height() * lvGeotransform[5] ), 'f', 10 ) << "\n";
    lvOutputStream << "cellsize " << QString::number( lvGeotransform[1], 'f', 10 ) << "\n";
    if( theStartRaster->isNoDataValueValid() )
    {
      lvOutputStream << "NODATA_value " << theStartRaster->noDataValue() << "\n";
    }

    //Binary Files
    if( cvThreshold > 0.000 )
    {
      lvBinaryFileMap[ lvIterator ] = new QFile( cvOutputDirectory.path() + QDir::toNativeSeparators( "/" ) + theSpecies + "_" + QString::number( theStartYear + ( lvIterator * cvStepSize ) ) + "-T" + QString::number( cvThreshold, 'f', 3 ) + ".asc" );
      if( !lvBinaryFileMap[ lvIterator ]->open( QIODevice::WriteOnly | QIODevice::Text ) )
      {
        cvLastError = tr( "Unable to create new output file" );
        lvHasError = true;
        break;
      }
      lvOutputStream.setDevice( lvBinaryFileMap[ lvIterator ] );
      lvOutputStream << "ncols " << theStartRaster->width() << "\n";
      lvOutputStream << "nrows " << theStartRaster->height() << "\n";
      lvOutputStream << "xllcorner " << QString::number( lvGeotransform[0], 'f', 10 ) << "\n";
      lvOutputStream << "yllcorner " << QString::number( lvGeotransform[3] + (( double )theStartRaster->height() * lvGeotransform[5] ), 'f', 10 ) << "\n";
      lvOutputStream << "cellsize " << QString::number( lvGeotransform[1], 'f', 10 ) << "\n";
      if( theStartRaster->isNoDataValueValid() )
      {
        lvOutputStream << "NODATA_value " << theStartRaster->noDataValue() << "\n";
      }
    }

  }

  QMap< int, QFile* >::const_iterator lvMapIterator = lvRealFileMap.begin();
  if( !lvHasError )
  {
    float lvResult;
    float* lvStartData;
    float* lvEndData;
    float lvRange = ( float )( theEndYear - theStartYear );
    int lvHeight = theStartRaster->height();
    int lvWidth = theStartRaster->width();
    for( int lvRowIterator = 0; lvRowIterator < lvHeight; lvRowIterator++ )
    {
      if( cvAbort )
      {
        cvLastError = tr( "Canceled by user" );
        lvHasError = true;
        break;
      }
      lvStartData = theStartRaster->rawData( 1, lvRowIterator );
      lvEndData = theEndRaster->rawData( 1, lvRowIterator );

      if( 0 == lvStartData || 0 == lvEndData )
      {
        cvLastError = QObject::tr( "Read error, the row buffer was null" );
        break;
      }

      lvMapIterator = lvRealFileMap.begin();
      while( lvMapIterator != lvRealFileMap.end() )
      {
        if( cvAbort )
        {
          cvLastError = tr( "Canceled by user" );
          lvHasError = true;
          break;
        }
        lvOutputStream.setDevice( lvMapIterator.value() );
        for( int lvColumnIterator = 0; lvColumnIterator < lvWidth; lvColumnIterator++ )
        {
          if( cvAbort )
          {
            cvLastError = tr( "Canceled by user" );
            lvHasError = true;
            break;
          }
          lvResult = lvStartData[ lvColumnIterator ] + ((( lvEndData[ lvColumnIterator ] - lvStartData[ lvColumnIterator ] ) / lvRange ) * ( float )lvMapIterator.key() * cvStepSize );

          //Both raster layers have to be the same, and have been check by this point, so we only need to check one for a valid no data value
          if( theStartRaster->isNoDataValueValid() && ( lvStartData[ lvColumnIterator ] == theStartRaster->noDataValue() || lvEndData[ lvColumnIterator ] == theEndRaster->noDataValue() ) )
          {
            lvOutputStream << theStartRaster->noDataValue();
          }
          else
          {
            lvOutputStream << lvResult;
          }
            
          lvOutputStream << " ";
        }

        //Binary Files
        if( cvThreshold > 0.000 )
        {
          if( cvAbort )
          {
            cvLastError = tr( "Canceled by user" );
            lvHasError = true;
            break;
          }
          lvOutputStream.setDevice( lvBinaryFileMap[ lvMapIterator.key()] );
          for( int lvColumnIterator = 0; lvColumnIterator < lvWidth; lvColumnIterator++ )
          {
            lvResult = lvStartData[ lvColumnIterator ] + ((( lvEndData[ lvColumnIterator ] - lvStartData[ lvColumnIterator ] ) / lvRange ) * ( float )lvMapIterator.key() * cvStepSize );

            //Both raster layers have to be the same, and have been check by this point, so we only need to check one for a valid no data value
            if( theStartRaster->isNoDataValueValid() && ( lvStartData[ lvColumnIterator ] == theStartRaster->noDataValue() || lvEndData[ lvColumnIterator ] == theEndRaster->noDataValue() ) )
            {
              lvOutputStream << theStartRaster->noDataValue();
            }
            else
            {
              lvResult < cvThreshold ? lvOutputStream << "0" : lvOutputStream << "1";
            }

	    lvOutputStream << " ";
          }
        }

        lvMapIterator++;
      }
      updateProgress( lvHeight - 1, lvRowIterator );
    }

  }

  lvMapIterator = lvRealFileMap.begin();
  while( lvMapIterator != lvRealFileMap.end() )
  {
    lvMapIterator.value()->close();
    delete lvMapIterator.value();
    lvMapIterator++;
  }

  lvMapIterator = lvBinaryFileMap.begin();
  while( lvMapIterator != lvBinaryFileMap.end() )
  {
    lvMapIterator.value()->close();
    delete lvMapIterator.value();
    lvMapIterator++;
  }

  return !lvHasError;
}


void GridInterpolator::updateProgress( int theDestination, int theProgress )
{
  if( 0 == theProgress )
  {
    cvLastProgressValue = 0;
    emit progress( 0 );
    return;
  }

  int lvCurrentProgress = ( int )((( double )theProgress / ( double )theDestination ) * 100.0 ) ;
  if( lvCurrentProgress != cvLastProgressValue )
  {
    cvLastProgressValue = lvCurrentProgress;
    emit progress( lvCurrentProgress );
  }
}
