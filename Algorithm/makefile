COMP = g++-5.3.0

SHARED_OBJS = _311246755_a.so  #_311246755_b.so
OBJS = _311246755_a.o #_311246755_b.o
CPP_COMP_FLAG = -std=c++17 -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC
CPP_LINK_FLAG = -shared

all: $(SHARED_OBJS)

$(SHARED_OBJS): $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

_311246755_a.o: _311246755_a.cpp _311246755_a.h AbstractAlgorithm.h 
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp 

#_311246755_b.o: _311246755_b.cpp _311246755_b.h AbstractAlgorithm.h 
#	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp 
	
clean:
	rm -f $(OBJS) $(SHARED_OBJS)
