/*
** Copyright (C) 2010 Peter J. Ersts (ersts at amnh.org)
** Creation Date: 2010-02-20
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
**
** This work was partially funded by the National Science Foundation under Grant No.DEB-0641023
**
** Any opinions, findings and conclusions or recommendations expressed in this material are those
** of the author(s) and do not necessarily reflect the views of the National Science Foundation (NSF)
*/
#ifndef RASTER_H
#define RASTER_H

#include <gdal.h>

#include <QString>

class Raster
{
  public:
    Raster();
    Raster( QString theFileName, bool readOnly = true );
    ~Raster();

    const double* const geotransform() { return cvGeotransform; }

    int height() { return cvHeight; }

    bool isNoDataValueValid() { return ( bool )cvValidNoData; }

    bool isValid() { return cvIsValid; }

    QString lastError() { return cvLastError; }

    double noDataValue() { return cvNoDataValue; }

    int numberOfBands() { return cvNumberOfBands; }

    float* rawData( int, int );

    int width() { return cvWidth; }


    bool operator==( const Raster &that );
    bool operator!=( const Raster &that );

  private:
    QString cvLastError;

    bool cvIsValid;
    bool cvIsReadOnly;

    int cvWidth;
    int cvHeight;
    int cvNumberOfBands;
    int cvValidNoData;
    int cvDataTypeSize;

    int* cvCurrentRowBuffered;
    float** cvRowBuffer;

    double cvNoDataValue;
    double cvGeotransform[6];

    GDALDatasetH cvGDALDataSet;
    GDALRasterBandH* cvBands;
    GDALDataType cvDataType;
};

#endif // RASTER_H
