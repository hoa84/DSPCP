#***************************************************************************
#                                  Makefile 
#                             -------------------
#
#  Build for general CPP utils
#
#  __________________________________________________________________________
#
#    begin                : Thu Oct 9 2003
#    copyright            : (C) 2003 by W. Michael Brown
#    email                : wmbrown@sandia.gov
#***************************************************************************/

include ../Configuration.mk

OBJ_DIR = $(HOBJ_DIR)/all

OBJS = $(OBJ_DIR)/error.o $(OBJ_DIR)/commandline.o \
       $(OBJ_DIR)/misc.o 

EXECS = $(BIN_DIR)/mdiff

all: $(OBJS) $(ALL_LIB) $(EXECS)

$(OBJ_DIR)/error.o: error.h error.cpp
	$(CPP) $(CFLAGS) -o $@ error.cpp

$(OBJ_DIR)/commandline.o: commandline.h commandline.cpp
	$(CPP) $(CFLAGS) -o $@ commandline.cpp
	
$(OBJ_DIR)/misc.o: misc.h misc.cpp
	$(CPP) $(CFLAGS) -o $@ misc.cpp

$(OBJ_DIR)/miscmath.o: miscmath.h miscmath.cpp
	$(CPP) $(CFLAGS) -o $@ miscmath.cpp

$(ALL_LIB): $(OBJS)
	$(AR) -crusv $(ALL_LIB) $(OBJS)
	
$(OBJ_DIR)/mdiff.o: mdiff.cpp
	$(CPP) $(CFLAGS) -o $@ mdiff.cpp

$(BIN_DIR)/mdiff: $(OBJ_DIR)/mdiff.o $(ALL_LIB)
	$(CPP) -o $@ $(OBJ_DIR)/mdiff.o $(ALL_LIB) \
            $(LFLAGS) $(LLIBS)

clean:
	rm -rf $(OBJS) $(ALL_LIB) $(EXECS) a.out core \
               $(OBJ_DIR)/mdiff.o

veryclean: clean
	rm -rf *~ ./api
