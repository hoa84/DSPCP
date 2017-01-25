#***************************************************************************
#                               Makefile
#                          -------------------
#
#  _________________________________________________________________________
#    Build for the Math Utilities
#
#  _________________________________________________________________________
#
#    begin                : Fri Oct 27 2006
#    copyright            : (C) 2006 by W. Michael Brown
#    email                : wmbrown@sandia.gov
# ***************************************************************************/

include ../Configuration.mk
OBJ_DIR = $(HOBJ_DIR)/mathutils

# Objects for this project
THIS_OBJ = $(ALL_LIB) $(CUDA_LIB)
EXECS  = $(BIN_DIR)/vm_slice $(BIN_DIR)/cml_drl

all: libraries $(EXECS)

test: all
	tcsh ./run_tests.sh $(BIN_DIR)

libraries:
	cd $(ALL_DIR); make; cd $(CUDA_DIR); \
        make; cd ../mathutils

$(OBJ_DIR)/vm_slice.o: vm_slice.cpp
	$(CPP) $(CFLAGS) -o $@ vm_slice.cpp

$(OBJ_DIR)/cml_drl.o: cml_drl.cpp
	$(CPP) $(CFLAGS) -o $@ cml_drl.cpp

$(BIN_DIR)/cml_drl: $(OBJ_DIR)/cml_drl.o $(THIS_OBJ)
	$(CPP) -o $@ $(OBJ_DIR)/cml_drl.o $(THIS_OBJ) $(LFLAGS) \
                     $(LLIBS) $(CBLAS) $(CUDALNK)

$(BIN_DIR)/vm_slice: $(OBJ_DIR)/vm_slice.o $(THIS_OBJ)
	$(CPP) -o $@ $(OBJ_DIR)/vm_slice.o $(THIS_OBJ) $(LFLAGS) $(LLIBS) $(CBLAS)

#
#  Documentation
#
manpages: all
	/bin/tcsh make_manpages.sh

#
# Create a .tar distribution file
#
dist: all manpages
	/bin/tcsh makedistribution.sh

#
#  Remove objects, cores, etc.
#

clean:
	rm -rf $(EXECS); \
	cd $(OBJ_DIR); rm -f *.o

veryclean: clean
	rm -rf *~ ./api ./manpages

cleanproject: clean
	cd $(ALL_DIR); make clean; \
	cd $(CUDA_DIR); make clean; cd ../mathutils; make clean
