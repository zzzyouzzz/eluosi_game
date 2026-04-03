
MINGW_PATH := C:/PROGRA~1/mingw64/bin
SFML_PATH := C:/PROGRA~1/SFML-2.5.1


CXX := "$(MINGW_PATH)/g++.exe"
CXXFLAGS := -fdiagnostics-color=always -g -std=c++17 -I"$(SFML_PATH)/include" -mconsole
LDFLAGS := -L"$(SFML_PATH)/lib" -lsfml-audio -leasyx -pthread 


SRCS := main.cpp eluosi.cpp
OBJS := $(SRCS:.cpp=.o)
TARGET := main.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@  

clean:
	del /F /Q $(OBJS) $(TARGET)

.PHONY: all clean