CXX := g++
CXXFLAGS := -std=c++17 -Iinclude
SOURCES := $(wildcard src/*.cpp)
HEADERS := $(wildcard include/*.h)
TARGET := phonebook.exe

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	.\$(TARGET)

clean:
	if exist $(TARGET) del /Q $(TARGET)
