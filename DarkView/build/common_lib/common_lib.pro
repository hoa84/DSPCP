#-------------------------------------------------------------------------------
#    Common Library
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

TEMPLATE = lib
CONFIG += core staticlib
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = common

DESTDIR     = ../../bin/libs
OBJECTS_DIR = ../../bin/build/common_lib/.obj
MOC_DIR     = ../../bin/build/common_lib/.moc
RCC_DIR     = ../../bin/build/common_lib/.rcc
UI_DIR      = ../../bin/build/common_lib/.ui

win32:DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../../include

SOURCES += \ 
          ../../src/GL/oglCommon.cpp \ 
          ../../src/GL/oglFont.cpp \ 
          ../../src/QT/QExtendedMainWindow.cpp \ 
          ../../src/SCI/Mat4.cpp \ 
          ../../src/SCI/Graphics/RenderElements.cpp \ 
          ../../src/Data/PhysicsData.cpp \ 
          ../../src/Data/MultiDimensionalData.cpp \ 
          ../../src/Data/DenseMultiDimensionalData.cpp \ 
          ../../src/GL/oglTexture2D.cpp \ 
          ../../src/GL/oglTexture.cpp


HEADERS += \ 
          ../../include/SCI/Vex4.h \ 
          ../../include/SCI/Vex3.h \ 
          ../../include/SCI/Vex2.h \ 
          ../../include/SCI/Mat4.h \ 
          ../../include/SCI/Utility.h \ 
          ../../include/SCI/Graphics/RenderElements.h \ 
          ../../include/GL/oglCommon.h \ 
          ../../include/GL/oglFont.h \ 
          ../../include/QT/QExtendedMainWindow.h \ 
          ../../include/Data/PhysicsData.h \ 
          ../../include/SCI/VexN.h \ 
          ../../include/Data/MultiDimensionalData.h \ 
          ../../include/Data/DenseMultiDimensionalData.h \ 
          ../../include/GL/oglTexture2D.h \ 
          ../../include/GL/oglTexture.h \ 
          ../../include/GL/glext.h \ 
          ../../include/SCI/BoundingBox.h


