CXX = g++ -std=c++14
OPTS = -g -O2
WARN = -Wall -Werror
DEPS = -MMD -MF $*.d
INCL =

OBJS = test_graph.o timing.o

default: $(OBJS)

clean:
	rm -rf Dependencies $(OBJS)

%.o: %.cpp
	$(CXX) $(OPTS) $(WARN) $(DEPS) $(INCL) $< -o $@
	cat $*.d >> Dependencies
	rm -f $*.d

-include Dependencies

