CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra -pedantic

CORE_OBJS = Block.o Board.o Wall.o Exit.o Gate.o FileParser.o GameState.o \
            AStarSolver.o BestCostTable.o MinHeap.o NodeArena.o

TEST_EXES = test_block.exe test_wall.exe test_exit.exe test_gate.exe \
            test_board.exe test_fileparser.exe test_gamestate.exe \
            test_bestcosttable.exe test_astarnode.exe test_minheap.exe \
            test_nodearena.exe test_astarsolver.exe

all: solver.exe

solver.exe: main.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) main.o $(CORE_OBJS) -o solver.exe

test: $(TEST_EXES)
	./test_block.exe
	./test_wall.exe
	./test_exit.exe
	./test_gate.exe
	./test_board.exe
	./test_fileparser.exe
	./test_gamestate.exe
	./test_bestcosttable.exe
	./test_astarnode.exe
	./test_minheap.exe
	./test_nodearena.exe
	./test_astarsolver.exe

test_block.exe: test_block.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_block.o $(CORE_OBJS) -o test_block.exe

test_wall.exe: test_wall.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_wall.o $(CORE_OBJS) -o test_wall.exe

test_exit.exe: test_exit.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_exit.o $(CORE_OBJS) -o test_exit.exe

test_gate.exe: test_gate.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_gate.o $(CORE_OBJS) -o test_gate.exe

test_board.exe: test_board.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_board.o $(CORE_OBJS) -o test_board.exe

test_fileparser.exe: test_fileparser.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_fileparser.o $(CORE_OBJS) -o test_fileparser.exe

test_gamestate.exe: test_gamestate.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_gamestate.o $(CORE_OBJS) -o test_gamestate.exe

test_bestcosttable.exe: test_bestcosttable.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_bestcosttable.o $(CORE_OBJS) -o test_bestcosttable.exe

test_astarnode.exe: test_astarnode.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_astarnode.o $(CORE_OBJS) -o test_astarnode.exe

test_minheap.exe: test_minheap.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_minheap.o $(CORE_OBJS) -o test_minheap.exe

test_nodearena.exe: test_nodearena.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_nodearena.o $(CORE_OBJS) -o test_nodearena.exe

test_astarsolver.exe: test_astarsolver.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) test_astarsolver.o $(CORE_OBJS) -o test_astarsolver.exe

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o *.exe

.PHONY: all test clean
