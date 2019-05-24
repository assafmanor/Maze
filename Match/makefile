COMP = g++-8.3.0

OBJS = main.o AlgorithmRegistration.o AlgorithmRegistrar.o GameManager.o MatchManager.o
EXEC = match
CPP_COMP_FLAG = -std=c++17 -Wall -Wextra -Werror -pedantic-errors -DNDEBUG 
CPP_LINK_FLAG = -ldl -export-dynamic -rdynamic -pthread
FILESYSTEM_FLAG = -lstdc++fs

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) $(FILESYSTEM_FLAG) -o $@ 

AlgorithmRegistration.o: AlgorithmRegistration.cpp AlgorithmRegistration.h AlgorithmRegistrar.h AbstractAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

AlgorithmRegistrar.o: AlgorithmRegistrar.cpp AlgorithmRegistrar.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

GameManager.o: GameManager.cpp GameManager.h AbstractAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

MatchManager.o: MatchManager.cpp MatchManager.h AbstractAlgorithm.h GameManager.h AlgorithmRegistrar.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

main.o: main.cpp GameManager.h MatchManager.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
	
clean:
	rm -f $(OBJS) $(EXEC)
