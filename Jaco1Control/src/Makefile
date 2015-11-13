####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -g -Wall -W -fPIC $(DEFINES)
CXXFLAGS      = -pipe -g -std=c++0x -Wall -W -fPIC $(DEFINES)
SRC_DIR		= ./
INCPATH       = -I$(SRC_DIR) -I. -I../../Qt/5.5/gcc_64/mkspecs/linux-g++
QMAKE         = /home/smaricha/Qt/5.5/gcc_64/bin/qmake
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
LINK          = g++
LFLAGS        = -Wl,-rpath,/home/smaricha/Qt/5.5/gcc_64
LIBS_PATH_INC = -L/usr/lib/x86_64-linux-gnu/ -L/usr/local/lib
LIBS          = $(SUBLIBS) -lsfml-graphics -lsfml-system -lsfml-window -lboost_timer -lboost_chrono -lboost_system -lboost_thread -lpthread -ldl -llapack -larmadillo -lplplotcxxd -lboost_filesystem -lkindrv
AR            = ar cqs
RANLIB        = 
SED           = sed
STRIP         = strip


####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = $(SRC_DIR)/main.cpp \
		$(SRC_DIR)/devices/driverbot.cpp \
		$(SRC_DIR)/devices/driverbot_contr.cpp \
		$(SRC_DIR)/devices/kinova_controller.cpp \
		$(SRC_DIR)/devices/kinova_controller_openAPI.cpp \
		$(SRC_DIR)/devices/kinova_status.cpp \
		$(SRC_DIR)/devices/kinova_status_openAPI.cpp \
		$(SRC_DIR)/Interface/VREP/extApi.cpp \
		$(SRC_DIR)/Interface/VREP/extApiCustom.cpp \
		$(SRC_DIR)/Interface/VREP/extApiInternal.cpp \
		$(SRC_DIR)/Interface/VREP/extApiPlatform.cpp \
		$(SRC_DIR)/robot/Jaco.cpp \
		$(SRC_DIR)/robot/Lwr.cpp \
		$(SRC_DIR)/common.cpp \
		$(SRC_DIR)/controller.cpp \
		$(SRC_DIR)/geometry.cpp \
		$(SRC_DIR)/model.cpp \
		$(SRC_DIR)/robot.cpp \
		$(SRC_DIR)/safetycheck.cpp \
		$(SRC_DIR)/visualization.cpp 
OBJECTS       = main.o \
		driverbot.o \
		driverbot_contr.o \
		kinova_controller.o \
		kinova_controller_openAPI.o \
		kinova_status.o \
		kinova_status_openAPI.o \
		extApi.o \
		extApiCustom.o \
		extApiInternal.o \
		extApiPlatform.o \
		Jaco.o \
		Lwr.o \
		common.o \
		controller.o \
		geometry.o \
		model.o \
		robot.o \
		safetycheck.o \
		visualization.o

QMAKE_TARGET  = KinovaJacoController
DESTDIR       = #avoid trailing-slash linebreak
TARGET        = KinovaJacoController


first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS_PATH_INC) $(LIBS)

all: $(TARGET)


clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Compile

main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/robot.hpp \
		$(SRC_DIR)/safetycheck.hpp \
		$(SRC_DIR)/geometry.hpp \
		$(SRC_DIR)/common.hpp \
		$(SRC_DIR)/Idevice.hpp \
		$(SRC_DIR)/devices/kinova_status.hpp \
		$(SRC_DIR)/devices/kinova_status_openAPI.hpp \
		$(SRC_DIR)/stats.hpp \
		$(SRC_DIR)/model.hpp \
		$(SRC_DIR)/visualization.hpp \
		$(SRC_DIR)/devices/kinova_controller.hpp \
		$(SRC_DIR)/devices/kinova_controller_openAPI.cpp \
		$(SRC_DIR)/controller.hpp \
		$(SRC_DIR)/devices/driverbot.hpp \
		$(SRC_DIR)/devices/driverbot_contr.hpp \
		$(SRC_DIR)/robotmodel.hpp \
		$(SRC_DIR)/robot/dummybot.hpp \
		$(SRC_DIR)/robot/Jaco.hpp \
		$(SRC_DIR)/robot/Lwr.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o $(SRC_DIR)/main.cpp

driverbot.o: $(SRC_DIR)/devices/driverbot.cpp $(SRC_DIR)/devices/driverbot.hpp \
		$(SRC_DIR)/stats.hpp \
		$(SRC_DIR)/common.hpp \
		$(SRC_DIR)/model.hpp \
		$(SRC_DIR)/Interface/VREP/extApi.hpp \
		$(SRC_DIR)/Interface/VREP/extApiPlatform.hpp \
		$(SRC_DIR)/Interface/VREP/include/v_repConst.h \
		$(SRC_DIR)/Interface/VREP/extApiCustom.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o driverbot.o $(SRC_DIR)/devices/driverbot.cpp

driverbot_contr.o: $(SRC_DIR)/devices/driverbot_contr.cpp $(SRC_DIR)/devices/driverbot_contr.hpp \
		$(SRC_DIR)/controller.hpp \
		$(SRC_DIR)/common.hpp \
		$(SRC_DIR)/model.hpp \
		$(SRC_DIR)/Interface/VREP/extApi.hpp \
		$(SRC_DIR)/Interface/VREP/extApiPlatform.hpp \
		$(SRC_DIR)/Interface/VREP/include/v_repConst.h \
		$(SRC_DIR)/Interface/VREP/extApiCustom.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o driverbot_contr.o $(SRC_DIR)/devices/driverbot_contr.cpp

kinova_controller.o: $(SRC_DIR)/devices/kinova_controller.cpp $(SRC_DIR)/devices/kinova_controller.hpp \
		$(SRC_DIR)/controller.hpp \
		$(SRC_DIR)/common.hpp \
		$(SRC_DIR)/model.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o kinova_controller.o $(SRC_DIR)/devices/kinova_controller.cpp

kinova_controller_openAPI.o: $(SRC_DIR)/devices/kinova_controller_openAPI.cpp $(SRC_DIR)/devices/kinova_controller_openAPI.hpp \
		$(SRC_DIR)/controller.hpp \
		$(SRC_DIR)/common.hpp \
		$(SRC_DIR)/model.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o kinova_controller_openAPI.o $(SRC_DIR)/devices/kinova_controller_openAPI.cpp

kinova_status.o: $(SRC_DIR)/devices/kinova_status.cpp $(SRC_DIR)/devices/kinova_status.hpp \
		$(SRC_DIR)/stats.hpp \
		$(SRC_DIR)/common.hpp \
		$(SRC_DIR)/model.hpp \
		$(SRC_DIR)/visualization.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o kinova_status.o $(SRC_DIR)/devices/kinova_status.cpp

kinova_status_openAPI.o: $(SRC_DIR)/devices/kinova_status_openAPI.cpp $(SRC_DIR)/devices/kinova_status_openAPI.hpp \
		$(SRC_DIR)/stats.hpp \
		$(SRC_DIR)/common.hpp \
		$(SRC_DIR)/model.hpp \
		$(SRC_DIR)/visualization.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o kinova_status_openAPI.o $(SRC_DIR)/devices/kinova_status_openAPI.cpp

extApi.o: $(SRC_DIR)/Interface/VREP/extApi.cpp $(SRC_DIR)/Interface/VREP/extApi.hpp \
		$(SRC_DIR)/Interface/VREP/extApiPlatform.hpp \
		$(SRC_DIR)/Interface/VREP/include/v_repConst.h \
		$(SRC_DIR)/Interface/VREP/extApiInternal.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o extApi.o $(SRC_DIR)/Interface/VREP/extApi.cpp

extApiCustom.o: $(SRC_DIR)/Interface/VREP/extApiCustom.cpp $(SRC_DIR)/Interface/VREP/extApiCustom.hpp \
		$(SRC_DIR)/Interface/VREP/extApiPlatform.hpp \
		$(SRC_DIR)/Interface/VREP/extApi.hpp \
		$(SRC_DIR)/Interface/VREP/include/v_repConst.h \
		$(SRC_DIR)/Interface/VREP/extApiInternal.hpp \
		$(SRC_DIR)/Interface/VREP/include/extApiCustomConst.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o extApiCustom.o $(SRC_DIR)/Interface/VREP/extApiCustom.cpp

extApiInternal.o: $(SRC_DIR)/Interface/VREP/extApiInternal.cpp $(SRC_DIR)/Interface/VREP/extApiInternal.hpp \
		$(SRC_DIR)/Interface/VREP/extApiPlatform.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o extApiInternal.o $(SRC_DIR)/Interface/VREP/extApiInternal.cpp

extApiPlatform.o: $(SRC_DIR)/Interface/VREP/extApiPlatform.cpp $(SRC_DIR)/Interface/VREP/extApiPlatform.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o extApiPlatform.o $(SRC_DIR)/Interface/VREP/extApiPlatform.cpp

Jaco.o: $(SRC_DIR)/robot/Jaco.cpp $(SRC_DIR)/robot/Jaco.hpp \
		$(SRC_DIR)/model.hpp \
		$(SRC_DIR)/common.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Jaco.o $(SRC_DIR)/robot/Jaco.cpp

Lwr.o: $(SRC_DIR)/robot/Lwr.cpp $(SRC_DIR)/robot/Lwr.hpp \
		$(SRC_DIR)/model.hpp \
		$(SRC_DIR)/common.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Lwr.o $(SRC_DIR)/robot/Lwr.cpp

common.o: $(SRC_DIR)/common.cpp $(SRC_DIR)/common.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o common.o $(SRC_DIR)/common.cpp

controller.o: $(SRC_DIR)/controller.cpp $(SRC_DIR)/controller.hpp \
		$(SRC_DIR)/common.hpp \
		$(SRC_DIR)/model.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o controller.o $(SRC_DIR)/controller.cpp

geometry.o: $(SRC_DIR)/geometry.cpp $(SRC_DIR)/geometry.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o geometry.o $(SRC_DIR)/geometry.cpp

model.o: $(SRC_DIR)/model.cpp $(SRC_DIR)/model.hpp \
		$(SRC_DIR)/common.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o model.o $(SRC_DIR)/model.cpp

robot.o: $(SRC_DIR)/robot.cpp $(SRC_DIR)/robot.hpp \
		$(SRC_DIR)/safetycheck.hpp \
		$(SRC_DIR)/geometry.hpp \
		$(SRC_DIR)/common.hpp \
		$(SRC_DIR)/Idevice.hpp \
		$(SRC_DIR)/devices/kinova_status.hpp \
		$(SRC_DIR)/devices/kinova_status_openAPI.hpp \
		$(SRC_DIR)/stats.hpp \
		$(SRC_DIR)/model.hpp \
		$(SRC_DIR)/visualization.hpp \
		$(SRC_DIR)/devices/kinova_controller.hpp \
		$(SRC_DIR)/devices/kinova_controller_openAPI.hpp \
		$(SRC_DIR)/controller.hpp \
		$(SRC_DIR)/devices/driverbot.hpp \
		$(SRC_DIR)/devices/driverbot_contr.hpp \
		$(SRC_DIR)/robotmodel.hpp \
		$(SRC_DIR)/robot/dummybot.hpp \
		$(SRC_DIR)/robot/Jaco.hpp \
		$(SRC_DIR)/robot/Lwr.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o robot.o $(SRC_DIR)/robot.cpp

safetycheck.o: $(SRC_DIR)/safetycheck.cpp $(SRC_DIR)/safetycheck.hpp \
		$(SRC_DIR)/geometry.hpp \
		$(SRC_DIR)/common.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o safetycheck.o $(SRC_DIR)/safetycheck.cpp

visualization.o: $(SRC_DIR)/visualization.cpp $(SRC_DIR)/visualization.hpp \
		$(SRC_DIR)/common.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o visualization.o $(SRC_DIR)/visualization.cpp


