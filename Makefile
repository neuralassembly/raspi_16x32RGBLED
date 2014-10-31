CXX = g++

CXXFLAGS = -O2 -Wall -I/usr/local/include/ 
LFLAGS_CAM = -lraspicam -lraspicam_cv 
LFLAGS_CV = -lopencv_core -lopencv_highgui -lopencv_imgproc

SRC1 = image2LEDmatrix.cpp
SRC2 = camera2LEDmatrix.cpp

OBJ1 := $(SRC1:.cpp=.o)
OBJ2 := $(SRC2:.cpp=.o)

TARGET1 = $(SRC1:.cpp=)
TARGET2 = $(SRC2:.cpp=)

all:	$(TARGET1) $(TARGET2)

$(TARGET1):	$(OBJ1)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ1) $(LFLAGS_CV)

$(TARGET2):	$(OBJ2)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ2) $(LFLAGS_CV) $(LFLAGS_CAM)

clean:
	rm -f $(OBJ1) $(OBJ2) $(TARGET1) $(TARGET2)
