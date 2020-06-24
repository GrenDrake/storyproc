CXXFLAGS=-std=c++14 -I../maddy/include -g
OBJS=story.o utility.o

story: $(OBJS)
	$(CXX) $(OBJS) -o story
