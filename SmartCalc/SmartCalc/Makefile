CXX = clang++
SDL = -framework SDL2 -framework SDL2_image -framework SDL2_ttf
CXXFLAGS = -Wall -c -std=c++11 
LDFLAGS = $(SDL) -F /Library/Frameworks/

SmartCalc : main.o LocationHandler.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) LocationHandler.o main.o -o SmartCalc

LocationHandler.o : LocationHandler.cpp LocationHandler.h
	$(CXX) $(LDFLAGS) -c LocationHandler.cpp

main.o : main.cpp LocationHandler.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c main.cpp
