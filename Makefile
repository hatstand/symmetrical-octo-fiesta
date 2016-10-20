CXX=g++
CXXFLAGS=--std=c++0x
LDFLAGS=-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_ml

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

words: words.o scrabble.o knearest.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
