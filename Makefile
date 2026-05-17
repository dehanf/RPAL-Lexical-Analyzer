CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
SRC_DIR := src
TARGET := rpal20

SOURCES := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/lexical.cpp \
	$(SRC_DIR)/parser.cpp \
	$(SRC_DIR)/ASTNode.cpp \
	$(SRC_DIR)/Standardizer.cpp \
	$(SRC_DIR)/RuntimeValue.cpp \
	$(SRC_DIR)/Environment.cpp \
	$(SRC_DIR)/Builtins.cpp \
	$(SRC_DIR)/CSEMachine.cpp

OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all clean test package

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

test: all
	./scripts/run_tests.sh

package: test
	./scripts/package_submission.sh
