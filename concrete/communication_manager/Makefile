
# make clean/all CONFIG=Debug/Release ARCH=linux64/raspi ECHO=0/1

# the parrent dir of "arch" dir
ROOT_DIR = ../../../..

#PROJ_SRCS = $(wildcard *.c)
PROJ_SRCS = communication_manager.cpp communication_manager_test.cpp

# list of directories containing header files
#PROJ_INC_DIR = $(ROOT_DIR)/include
PROJ_INC_DIR =	../../framework/reactor/\
				../nbd_buse/\
				../atlas/\
				../slicer/\
				../nbd_communication/

# the executable file
PROJ_OUT = communication_manager.out

# list of ADDITIONAL compiler flags (prepended with "-" sign)
#PROJ_CFLAGS = -Wunused-but-set-variable
PROJ_CFLAGS =
PROJ_CPPFLAGS = -std=c++11

#PROJ_MACROS = _BSD_SOURCE
PROJ_MACROS =

# additional object files (created by other projects)
PROJ_ADD_OBJS = imonitor.obj buse.o slicer.obj nbd_communication.obj

# additional libraries
PROJ_LIBS =

# linker flags
PROJ_LDFLAGS =

# list of projects (makefiles) this project depends on
PROJ_DEPENDS = ../../framework/reactor/Makefile ../nbd_buse/Makefile ../slicer/Makefile ../nbd_communication/Makefile

include $(ROOT_DIR)/make/Project.mak
