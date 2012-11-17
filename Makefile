CXXFLAGS := -Wall -Wextra -g -std=c++11

BIN  := ssoaag
OBJS := main.o html_writer.o bitmap.o

$(BIN): $(OBJS)
	$(CXX) -o $@ $^ $(LDLIBS)

bitmap.o: bitmap.cc bitmap.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

html_writer.o: html_writer.cc html_writer.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<
