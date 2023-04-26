Compilation instructions in Unix, Linux system:
make
Note: You need to have boost installed in your system to compile the current version. Refer: https://www.boost.org/
If you do not have boost, you can use set instead of unordered_set as the container for edges and adjacency edges in graph.h file and edges_unexplored in graph_algorithm.h

Execute test file: ./test_graph.o
Execute timing file: ./timing.o <complete_graph_size> <mesh_graph_size> <random_graph_size>

Note complete_graph_size need to be smaller than mesh and random. 


Files
----------
graph.h - Adjancency graph implementation using unordered_set containers. You need to complete the implementation of insert_edge, insert_edge_undirected, insert_vertex, erase_edge, erase_vertex functions.

graph_dumb_vector.h - Adjancency graph implementation using vector containers. You need to complete the implementation of insert_edge, insert_edge_undirected, insert_vertex, erase_edge, erase_vertex functions.

graph_algorithms.h - Implementations of graph search methods. BFS implementation is provide. You need to complete the implementation of DFS. 

test_graph.cpp - Testing algorithm to test both container based graph implementation including insertion and erase.

timing.cpp - Code to produce the timing results




