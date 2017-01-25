#-------------------------------------------------------------------------------
#    Data Scalable Approach for Parallel Coordinates
#    Copyright (C) 2016 - Hoa Nguyen, Paul Rosen
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------------------------------------

QT       += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

TARGET = DarkView

LIBS += -L../../bin/libs
LIBS += -lcommon
LIBS += -lshogun
mac: LIBS += -framework GLUT
else:unix|win32: LIBS += -lGLUT

# glew for windows
win32:LIBS += -lglew32

DESTDIR     = ../../bin
OBJECTS_DIR = ../../bin/build/darkview_app/.obj
MOC_DIR     = ../../bin/build/darkview_app/.moc
RCC_DIR     = ../../bin/build/darkview_app/.rcc
UI_DIR      = ../../bin/build/darkview_app/.ui

win32:DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += .
INCLUDEPATH += ../
INCLUDEPATH += ../../include
INCLUDEPATH += ../../../Common/include
INCLUDEPATH += ../../include/drl/drl

SOURCES += \ 
          ../../src/DarkView/MainWindow.cpp \ 
          ../../src/DarkView/Main.cpp \ 
          ../../src/DarkView/MainWidget.cpp \ 
          ../../src/DarkView/ParallelCoordinates.cpp \ 
          ../../src/DarkView/ScatterPlot.cpp \ 
          ../../src/DarkView/SmallMultiples.cpp \ 
          ../../src/DarkView/DataIndirector.cpp \ 
          ../../src/DarkView/QDimensionWidget.cpp \
          ../../src/DarkView/Kmean.cpp \
          ../../src/DarkView/Scatter.cpp \
    ../../src/DarkView/Shogun_Wrapper.cpp \
    ../../src/DarkView/DRL_Wrapper.cpp \
    ../../src/DarkView/DimensionalityReduction.cpp


HEADERS += \ 
          ../../include/DarkView/MainWindow.h \ 
          ../../include/DarkView/MainWidget.h \ 
          ../../include/DarkView/ParallelCoordinates.h \ 
          ../../include/DarkView/ScatterPlot.h \ 
          ../../include/DarkView/SmallMultiples.h \ 
          ../../include/DarkView/DataIndirector.h \ 
          ../../include/DarkView/QDimensionWidget.h \
          ../../include/DarkView/Kmean.h \
          ../../include/DarkView/Scatter.h \
          ../../include/DarkView/LeastSquare.h \
    ../../include/DarkView/Shogun_Wrapper.h \
    ../../include/DarkView/DRL_Wrapper.h \
    ../../include/DarkView/DimensionalityReduction.h \
    ../../include/DarkView/structoflxh.h


RESOURCES += \ 
          ../../fonts/arial.qrc




