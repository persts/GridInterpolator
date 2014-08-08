TARGET = GridInterpolator
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
CONFIG += qt
CONFIG += release
CONFIG += embed_manifest_exe
RESOURCES = ./src/gui/gui.qrc

SOURCES += aboutdialog.cpp
SOURCES += main.cpp
SOURCES += ./src/app/core/raster.cpp
SOURCES += ./src/app/gridinterpolator/gridinterpolator.cpp
SOURCES += ./src/gui/gridinterpolatorwidget.cpp

HEADERS += aboutdialog.h
HEADERS += ./src/app/core/raster.h
HEADERS += ./src/app/gridinterpolator/gridinterpolator.h
HEADERS += ./src/gui/gridinterpolatorwidget.h

FORMS += ./src/ui/gridinterpolatorwidget.ui
FORMS += aboutdialog.ui

INCLUDEPATH = ./ ./src/gui/ ./src/app/core ./src/app/gridinterpolator
unix:INCLUDEPATH += $$quote(/usr/include/gdal)
win32:INCLUDEPATH += C:/OSGeo4W/include

unix:LIBS += -L/usr/lib/ -lgdal
win32:LIBS = -LC:/OSGeo4W/lib -lgdal_i
