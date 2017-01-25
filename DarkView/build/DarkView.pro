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

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \ 
          common_lib
SUBDIRS += \ 
          darkview_app


mac: LIBS += -framework GLUT
else:unix|win32: LIBS += -lGLUT
