CXX=clang++
CXXFLAGS=-std=c++17 -O2 -g -ferror-limit=800
main: ../src/glsl.h main.cc all.h
	$(CXX) -DNDEBUG -I../src/ $(CXXFLAGS) main.cc -o main
	#$(CXX) -DNDEBUG -Is $(CXXFLAGS) main.cc -o main
all.h:
	./parse
clean:
	rm main
