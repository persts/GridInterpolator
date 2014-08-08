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
#ifndef GRIDINTERPOLATOR_H
#define GRIDINTERPOLATOR_H

#include "raster.h"

#include <QDir>
#include <QMap>
#include <QString>
#include <QThread>

class GridInterpolator: public QThread
{
    Q_OBJECT

  public:
    GridInterpolator();
    ~GridInterpolator();

    void abort() { cvAbort = true; }
    QString lastError() { return cvLastError; }
    void run();
    bool setParameters( const QMap< QString, QMap< int, QString > > &theInputMap, int theStepSize, int theStepsBeyond, double theThreshold, QString theOutputDirectory );

  signals:
    void consoleMessage( QString );
    void finished();
    void progress( int );

  private:
    QMap< QString, QMap< int, Raster* > > cvDataSets;
    QString cvLastError;

    QDir cvOutputDirectory;

    bool cvAbort;
    bool cvIsValid;

    int cvStepSize;
    int cvStepsBeyond;
    int cvLastProgressValue;

    double cvThreshold;

    void clearDataSets();
    void updateProgress( int, int );
    bool interpolate( QString, int, Raster*, int, Raster*, bool );
};

#endif // GRIDINTERPOLATOR_H
