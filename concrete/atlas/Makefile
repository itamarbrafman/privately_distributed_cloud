
# make clean/all CONFIG=Debug/Release ARCH=linux64/raspi ECHO=0/1

# the parrent dir of "arch" dir
ROOT_DIR = ../../../..

#PROJ_SRCS = $(shell xmlstarlet select -t -v //Unit/@filename xx.cbp | grep -e '\.c')
#PROJ_SRCS = iot_server_SA.cpp
PROJ_SRCS = iotserver_simple.cpp

# list of directories containing header files
PROJ_INC_DIR =

# the executable file
PROJ_OUT = iotservers_simple.out

# list of compiler flags (prepended with "-" sign)
#PROJ_CFLAGS = -Wunused-but-set-variable
PROJ_CPPFLAGS = -std=c++11 -Wno-old-style-cast

#PROJ_MACROS = _BSD_SOURCE
PROJ_MACROS =

# additional object files (created by other projects)
PROJ_ADD_OBJS =

# additional libraries
PROJ_LIBS =

# linker flags
PROJ_LDFLAGS =

# list of projects (makefiles) this project depends on
PROJ_DEPENDS =

include $(ROOT_DIR)/make/Project.mak
