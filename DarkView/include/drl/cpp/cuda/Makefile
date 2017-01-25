#***************************************************************************
#                               Makefile
#                          -------------------
#
#  _________________________________________________________________________
#    Build for the CUDA Utilities
#
#  _________________________________________________________________________
#
#    begin                : Tue Jan 27 2009
#    copyright            : (C) 2009 by W. Michael Brown
#    email                : wmbrown@sandia.gov
# ***************************************************************************/

include ../Configuration.mk
OBJ_DIR = $(HOBJ_DIR)/cuda

# Objects for this project
THIS_OBJ = $(MATH_LIB) $(ALL_LIB)

# Headers for CUDA Stuff
CUDA_H  = cuda_calls.h cuda_device.h cuda_timer.h

ifeq ($(USE_CUDA),1)
  EXECS = $(BIN_DIR)/get_cuda_devices
  OBJS = $(OBJ_DIR)/cuda_device.cu_o $(OBJ_DIR)/cuda_knn.cu_o
endif

all: $(CUDA_LIB) $(OBJS) $(EXECS)

$(OBJ_DIR)/cuda_device.cu_o: cuda_device.cu $(CUDA_H)
	$(CUDA) -o $@ -c cuda_device.cu

$(OBJ_DIR)/cuda_knn.cu_o: cuda_knn.cu cuda_knn.h $(CUDA_H) cuda_knn_texture.h 
	$(CUDA) -o $@ -c cuda_knn.cu

$(OBJ_DIR)/get_cuda_devices.cu_o: get_cuda_devices.cu $(CUDA_H) 
	$(CUDA) -o $@ -c get_cuda_devices.cu

$(BIN_DIR)/get_cuda_devices: $(OBJ_DIR)/get_cuda_devices.cu_o
	$(CPP) -o $@ $(OBJ_DIR)/get_cuda_devices.cu_o $(CUDALNK)

$(CUDA_LIB): $(OBJS)
	$(AR) -crusv $(CUDA_LIB) $(OBJS)

clean:
	rm -rf $(EXECS) $(CUDA_LIB); \
	cd $(OBJ_DIR); rm -f *.o *.cu_o

veryclean: clean
	rm -rf *~ ./api ./manpages
