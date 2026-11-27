CXX?=g++
FLAGS=-std=c++17 -O2 -Wall -Wextra
ifeq ($(OS),Windows_NT)
LDLIBS=-static -lws2_32
TARGET=cyberdeck.exe
TEST_RUN=tests/test_runner.exe
else
LDLIBS=
TARGET=cyberdeck
TEST_RUN=tests/test_runner
endif
all: $(TARGET)
$(TARGET): src/main.cpp $(wildcard src/*/*.hpp)
	$(CXX) $(FLAGS) src/main.cpp -o $@ $(LDLIBS)
test: tests/test.cpp $(wildcard src/*/*.hpp)
	$(CXX) $(FLAGS) tests/test.cpp -o $(TEST_RUN) $(LDLIBS) && ./$(TEST_RUN)
clean:
ifeq ($(OS),Windows_NT)
	@if exist cyberdeck.exe del /q cyberdeck.exe
	@if exist cyberdeck del /q cyberdeck
	@if exist tests\test_runner.exe del /q tests\test_runner.exe
	@if exist tests\test_runner del /q tests\test_runner
	@if exist cyberdeck.history del /q cyberdeck.history
else
	rm -f cyberdeck cyberdeck.exe tests/test_runner tests/test_runner.exe cyberdeck.history
endif
