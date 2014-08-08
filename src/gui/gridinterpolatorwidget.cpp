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
#include "gridinterpolatorwidget.h"
#include "aboutdialog.h"

#include "ui_gridinterpolatorwidget.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDirModel>
#include <QStringList>
#include <QFileDialog>
#include <QTableWidgetItem>

GridInterpolatorWidget::GridInterpolatorWidget( QWidget *parent ) :
  QWidget( parent )
{
  this->setAttribute( Qt::WA_DeleteOnClose );
  cvUi = new Ui::GridInterpolatorWidget();
  cvUi->setupUi( this );

  QDirModel* lvModel = new QDirModel( QStringList(), QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst );

  cvUi->tvFileBrowser->setModel( lvModel );
  cvUi->tvFileBrowser->setColumnWidth( 0, 400 );
  cvUi->tvFileBrowser->setCurrentIndex( lvModel->index( QDir::currentPath() ) );
  cvUi->tvFileBrowser->scrollTo( lvModel->index( QDir::currentPath() ) );

  connect( cvUi->tvFileBrowser, SIGNAL( doubleClicked( const QModelIndex& ) ), this, SLOT( doubleClicked( const QModelIndex& ) ) );
  cvUi->pBar->setMinimum( 0 );
  cvUi->pBar->setMaximum( 100 );
  cvUi->pBar->setValue( 0 );
  cvUi->pBar->setTextVisible( true );

  connect( &cvInterpolator, SIGNAL( consoleMessage( QString ) ), this, SLOT( consoleMessage( QString ) ) );
  connect( &cvInterpolator, SIGNAL( finished() ), this, SLOT( finished() ) );
  connect( &cvInterpolator, SIGNAL( progress( int ) ), this, SLOT( progress( int ) ) );
}


/*
 * PUBLIC METHODS
 */
void GridInterpolatorWidget::consoleMessage( QString theMessage )
{
  QDateTime lvDateTime = QDateTime::currentDateTime();
  cvUi->teConsole->append( "[ " + lvDateTime.toString( "yyyy-MM-dd hh:mm:ss " ) + " ]: " + theMessage );
}

void GridInterpolatorWidget::progress( int theProgress )
{
  cvUi->pBar->setValue( theProgress );
}



/*
 * PRIVATE METHODS
 */
QString GridInterpolatorWidget::buildPath( const QModelIndex& theIndex )
{
  if( !theIndex.parent().isValid() )
  {
#ifdef WIN32
    return theIndex.data().toString();
#else
    return "";
#endif
  }

  return buildPath( theIndex.parent() ) + QDir::toNativeSeparators( "/" ) + theIndex.data().toString();
}

void GridInterpolatorWidget::doubleClicked( const QModelIndex& theIndex )
{
  QString lvFile = buildPath( theIndex );
  QFileInfo lvFileInfo( lvFile );
  if( !lvFileInfo.isDir() )
  {
    int lvYearStart = lvFileInfo.fileName().lastIndexOf( "_" ) + 1;
    int lvEnd = lvFileInfo.fileName().lastIndexOf( "." );
    int lvYear = lvFileInfo.fileName().mid( lvYearStart, lvEnd - lvYearStart ).toInt();
    QString lvSpecies = lvFileInfo.fileName().mid( 0, lvYearStart - 1 );
    if( 0 != lvYear && !lvSpecies.isEmpty() )
    {
      if( !cvRasterLayerFileNames.contains( lvSpecies ) )
      {
        cvRasterLayerFileNames[ lvSpecies ] = QMap< int, QString >();
      }
      cvRasterLayerFileNames[ lvSpecies ][ lvYear ] = lvFile;

      cvUi->twFiles->clear();
      cvUi->twFiles->setRowCount( 0 );
      QMap< QString, QMap< int, QString > >::const_iterator lvSpeciesIterator = cvRasterLayerFileNames.begin();
      QMap< int, QString >::const_iterator lvYearIterator;
      while( lvSpeciesIterator != cvRasterLayerFileNames.end() )
      {
        QMap< int, QString > lvTempMap = lvSpeciesIterator.value();
        lvYearIterator = lvTempMap.begin();
        while( lvYearIterator != lvTempMap.end() )
        {
          int lvRowCount = cvUi->twFiles->rowCount();
          cvUi->twFiles->insertRow( lvRowCount );
          cvUi->twFiles->setItem( lvRowCount, 0, new QTableWidgetItem( lvSpeciesIterator.key() ) );
          cvUi->twFiles->setItem( lvRowCount, 1, new QTableWidgetItem( QString::number( lvYearIterator.key() ) ) );
          cvUi->twFiles->setItem( lvRowCount, 2, new QTableWidgetItem( lvYearIterator.value() ) );
          lvYearIterator++;
        }

        lvSpeciesIterator++;
      }
      cvUi->twFiles->setHorizontalHeaderLabels( QStringList() << "Species" << "Year" << "Raster Layer" );

    }
    else
    {
      consoleMessage( tr( "Expected S_YYYY.asc, where S is a species name and YYYY is a year other than 0" ) );
    }
  }
}

void GridInterpolatorWidget::finished()
{
  cvUi->pbtnRun->setEnabled( true );
}

void GridInterpolatorWidget::on_pbtnAbout_clicked()
{
  new AboutDialog();
}

void GridInterpolatorWidget::on_pbtnCancel_clicked()
{
  cvInterpolator.abort();
}

void GridInterpolatorWidget::on_pbtnRun_clicked()
{
  if( !cvInterpolator.setParameters( cvRasterLayerFileNames, cvUi->sbStepsSize->value(), cvUi->sbStepsBeyond->value(), cvUi->dsbThreshold->value(), cvUi->leOutputDirectory->text() ) )
  {
    consoleMessage( cvInterpolator.lastError() );
    return;
  }
  cvUi->pbtnRun->setEnabled( false );
  cvInterpolator.start();
}

void GridInterpolatorWidget::on_pbtnSelectOutputDirectory_clicked()
{
  QString lvFileName = QFileDialog::getExistingDirectory( this, tr( "Open Directory" ), buildPath( cvUi->tvFileBrowser->currentIndex().parent() ), QFileDialog::ShowDirsOnly );
  if( !lvFileName.isEmpty() )
  {
    cvUi->leOutputDirectory->setText( lvFileName );
  }
}

