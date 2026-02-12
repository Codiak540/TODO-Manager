CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = todo
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) priority_todo.txt regular_todo.txt

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run