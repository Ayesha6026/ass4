#include "graph.h"
#include "graph_algorithms.h"
#include "graph_dumb_vector.h"

#include <chrono>
#include <climits>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>
using namespace std;
using namespace chrono;

/// @brief create a complete graph of size n
template<typename graph_id>
void initialize_complete_graph(graph_id& g, size_t n) {
  // add vertices
  for(size_t i = 0; i < n; ++i)
    g.insert_vertex(i);

  //add edges

  typedef typename graph_id::const_vertex_iterator CVI;

  for(CVI vi1 = g.vertices_cbegin(); vi1 != g.vertices_cend(); ++vi1)
    for(CVI vi2 = g.vertices_cbegin(); vi2 != g.vertices_cend(); ++vi2)
      if((*vi1)->descriptor() != (*vi2)->descriptor())
        g.insert_edge((*vi1)->descriptor(), (*vi2)->descriptor(), double(rand()) / RAND_MAX);
}

/// @brief create a mesh of size n
template<typename graph_id>
void initialize_mesh_graph(graph_id& g, size_t n) {
  //make n a square number
  size_t rootn = sqrt(n);
  n = rootn*rootn;

  // add vertices
  for(size_t i = 0; i < n; ++i)
    g.insert_vertex(i);

  // add edges
  for(size_t i = 0; i < n; ++i) {
    size_t x = i + 1;
    size_t y = i + rootn;

    if(x % rootn != 0)
      g.insert_edge_undirected(i, x, double(rand()) / RAND_MAX);

    if(y < n)
      g.insert_edge_undirected(i, y, double(rand()) / RAND_MAX);
  }
}

/// @brief create a mesh of size n
template<typename graph_id>
void initialize_random_graph(graph_id& g, size_t n) {
  // add vertices
  for(size_t i = 0; i < n; ++i)
    g.insert_vertex(i);

  // add edges for connectivity
  for(size_t i=0; i < n - 1; ++i)
    g.insert_edge_undirected(i, i+1, double(rand()) / RAND_MAX);

  size_t num_edges = n*sqrt(n)/2;
  for(size_t i=0; i < num_edges; ++i) {
    size_t s = rand() % n;
    size_t t = rand() % n;
    if(s != t)
      g.insert_edge_undirected(s, t, double(rand()) / RAND_MAX);
    else
      --i;
  }
}

///@brief Time functions of a graph
template<typename graph_id, typename Initializer>
void time_graph(Initializer i, size_t n) {
  cout << "\tGraph Size: "<< n; 
  high_resolution_clock::time_point create_start = high_resolution_clock::now();
  //create graph, testing insertion
  graph_id g;
  i(g, n);
  high_resolution_clock::time_point create_stop = high_resolution_clock::now();
  cout << "\tCreate: " << duration_cast<duration<double>>(create_stop - create_start).count();

  high_resolution_clock::time_point bfs_start = high_resolution_clock::now();
  //run BFS
  typedef typename graph_id::vertex_descriptor vertex_descriptor;
  unordered_map<vertex_descriptor, vertex_descriptor> parent_map;
  breadth_first_search(g, parent_map);
  high_resolution_clock::time_point bfs_stop = high_resolution_clock::now();
  cout << "\tBFS: " << duration_cast<duration<double>>(bfs_stop - bfs_start).count();

  high_resolution_clock::time_point erase_start = high_resolution_clock::now();
  
  //run DFS
  high_resolution_clock::time_point dfs_start = high_resolution_clock::now();
  parent_map.clear();
  depth_first_search(g, parent_map);
  high_resolution_clock::time_point dfs_stop = high_resolution_clock::now();
  cout << "\tDFS: " << duration_cast<duration<double>>(dfs_stop - dfs_start).count();
  
  //test erase operations
  size_t quarter_edge = g.num_edges() / 4;
  for(size_t i = 0; i < quarter_edge; ++i) {
      typename graph_id::edge_iterator ei = g.find_edge(
          make_pair(rand() % g.num_vertices(), rand() % g.num_vertices())
          );
      if(ei != g.edges_end())
        g.erase_edge((*ei)->descriptor());
      else
        --i;
  }
  size_t quarter_nodes = g.num_vertices() / 4;
  for(size_t i = 0; i < quarter_nodes; ++i) {
      typename graph_id::vertex_iterator vi = g.find_vertex(rand() % g.num_vertices());
      if(vi != g.vertices_end())
        g.erase_vertex((*vi)->descriptor());
      else
        --i;
  }
  high_resolution_clock::time_point erase_stop = high_resolution_clock::now();
  cout << "\tErase: " << duration_cast<duration<double>>(erase_stop - erase_start).count() << endl;
  
}

/// @brief Control timing of a single function
/// @tparam Func Function type
/// @param f Function taking a single size_t parameter
/// @param graph_size Max vertex numbers of the graph
/// @param name Name of function for nice output

template<typename graphID, typename Func>
void time_function(Func f, size_t graph_size, string name) {
  cout << "Graph type: "<< name << endl;

  for(size_t gs = 20; gs <= graph_size; gs += 20) 
    time_graph<graphID, Func>(f, gs);

}

/// @brief Main function to time all your functions
int main(int argc, char** argv) {
  if(argc != 4) {
    cerr << "Error. Wrong number of arguments. Run program like:" << endl;
    cerr << "./timing.o <complete_graph_size> <mesh_graph_size> <random_graph_size>" << endl;
    cerr << "Example: ./timing.o 300 1750 800" << endl;
    exit(-1);
  }

  srand(0);

  size_t complete_size = atoi(argv[1]);
  size_t     mesh_size = atoi(argv[2]);
  size_t   random_size = atoi(argv[3]);
  
  typedef graph<int, double> graph_set_type;
  typedef graph_vector<int, double> graph_vector_type;
  cout<<"--------------\nSET GRAPH:\n--------------\n";
  time_function<graph_set_type>(initialize_complete_graph<graph_set_type>, complete_size, "Complete");
  time_function<graph_set_type>(    initialize_mesh_graph<graph_set_type>,     mesh_size,     "Mesh");
  time_function<graph_set_type>(  initialize_random_graph<graph_set_type>,   random_size,   "Random");
  
  cout<<"\n\n--------------\nVECTOR GRAPH:\n--------------\n";
  time_function<graph_vector_type>(initialize_complete_graph<graph_vector_type>, complete_size, "Complete");
  time_function<graph_vector_type>(    initialize_mesh_graph<graph_vector_type>,     mesh_size,     "Mesh");
  time_function<graph_vector_type>(  initialize_random_graph<graph_vector_type>,   random_size,   "Random");
}
