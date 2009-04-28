# Created by the script cgal_create_makefile
# This is the makefile for compiling a CGAL application.

#---------------------------------------------------------------------#
#                    include platform specific settings
#---------------------------------------------------------------------#
# Choose the right include file from the <cgalroot>/make directory.

# CGAL_MAKEFILE = ENTER_YOUR_INCLUDE_MAKEFILE_HERE
ifndef CGAL_MAKEFILE
CGAL_MAKEFILE = /usr/share/cgal/cgal.mk
endif
include $(CGAL_MAKEFILE)

KOKOMPE_INCLUDE_DIR=/home/sl/make2/kokompe/engine
KOKOMPE_BUILD_DIR=/home/sl/make2/kokompe/engine
KOKOMPE_CXX_FLAGS = -I$(KOKOMPE_INCLUDE_DIR)
KOKOMPE_LINK_FLAGS = $(KOKOMPE_BUILD_DIR)/libengine.a



#---------------------------------------------------------------------#
#                    compiler flags
#---------------------------------------------------------------------#

CXXFLAGS = \
           $(TESTSUITE_CXXFLAGS) \
           $(EXTRA_FLAGS) \
           $(CGAL_CXXFLAGS) \
           $(LONG_NAME_PROBLEM_CXXFLAGS)

#---------------------------------------------------------------------#
#                    linker flags
#---------------------------------------------------------------------#

LIBPATH = \
          $(TESTSUITE_LIBPATH) \
          $(CGAL_LIBPATH)

LDFLAGS = \
          $(TESTSUITE_LDFLAGS) \
          $(LONG_NAME_PROBLEM_LDFLAGS) \
          $(CGAL_LDFLAGS)

#---------------------------------------------------------------------#
#                    target entries
#---------------------------------------------------------------------#

all:            \
                test2$(EXE_EXT) 

test1$(EXE_EXT): test1$(OBJ_EXT)
	$(CGAL_CXX) $(LIBPATH) $(EXE_OPT)test1 test1$(OBJ_EXT) $(LDFLAGS)

test2$(EXE_EXT): test2$(OBJ_EXT)
	$(CGAL_CXX) $(KOKOMPE_CXX_FLAGS) $(LIBPATH) $(EXE_OPT)test2 test2$(OBJ_EXT) $(LDFLAGS) $(KOKOMPE_LINK_FLAGS)

clean: \
                   test1.clean \
                   test2.clean 

#---------------------------------------------------------------------#
#                    suffix rules
#---------------------------------------------------------------------#

.C$(OBJ_EXT):
	$(CGAL_CXX) $(CXXFLAGS) $(OBJ_OPT) $<


.cpp$(OBJ_EXT):
	$(CGAL_CXX) $(CXXFLAGS) $(KOKOMPE_CXX_FLAGS) $(OBJ_OPT) $<

