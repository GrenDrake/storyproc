CXXFLAGS=-std=c++14 -I../maddy/include -g
OBJS=storyproc.o utility.o

storyproc: $(OBJS)
	$(CXX) $(OBJS) -o storyproc
