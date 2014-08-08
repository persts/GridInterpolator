/*
** Copyright (C) 2012 Peter J. Ersts (ersts at amnh.org)
** Creation Date: 2012-08-20
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
#include <QApplication>
#include <QSettings>
#include "gridinterpolatorwidget.h"


int main( int argc, char* argv[] )
{
  QApplication lvApp( argc, argv );
  GridInterpolatorWidget* lvMainApp = new GridInterpolatorWidget();
  lvMainApp->show();

  QCoreApplication::setOrganizationName( "biodiversityinformatics" );
  QCoreApplication::setOrganizationDomain( "biodiversityinformatics.amnh.org" );
  QCoreApplication::setApplicationName( "sdmtoolkit" );


  return lvApp.exec();
}

