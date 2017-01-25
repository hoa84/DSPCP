#-------------------------------------------------------------------------------
#    Data Scalable Approach for Parallel Coordinates
#    Copyright (C) 2016 - Hoa Nguyen, Paul Rosen
#-------------------------------------------------------------------------------

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \ 
          common_lib
SUBDIRS += \ 
          darkview_app


mac: LIBS += -framework GLUT
else:unix|win32: LIBS += -lGLUT
