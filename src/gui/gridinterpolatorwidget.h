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
#ifndef GRIDINTERPOLATORWIDGET_H
#define GRIDINTERPOLATORWIDGET_H

#include "gridinterpolator.h"

#include <QWidget>
#include <QModelIndex>

namespace Ui
{
  class GridInterpolatorWidget;
}

class GridInterpolatorWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit GridInterpolatorWidget( QWidget *parent = 0 );

  signals:

  public slots:
    void progress( int );
    void consoleMessage( QString );

  private slots:
    void doubleClicked( const QModelIndex& );
    void finished();
    void on_pbtnAbout_clicked();
    void on_pbtnCancel_clicked();
    void on_pbtnRun_clicked();
    void on_pbtnSelectOutputDirectory_clicked();

  private:
    Ui::GridInterpolatorWidget* cvUi;

    GridInterpolator cvInterpolator;

    QString buildPath( const QModelIndex& );


    QMap< QString, QMap< int, QString > > cvRasterLayerFileNames;

};

#endif // GRIDINTERPOLATORWIDGET_H
