CXXFLAGS := -Wall -Wextra -g -std=c++11

BIN  := ssoaag
OBJS := main.o bitmap.o

$(BIN): $(OBJS)
	$(CXX) -o $@ $^ $(LDLIBS)

bitmap.o: bitmap.cc bitmap.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
