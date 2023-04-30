#ifndef _GRAPH_ALGORITHMS_H_
#define _GRAPH_ALGORITHMS_H_

#include <queue>
#include <unordered_set>
#include <boost/functional/hash.hpp> // Comment this if you haven't boost installed


// This is an example list of the basic algorithms we will work with in class.
//
// In general this is what the following template parameters are:
//
//  - Graph: type of graph, literally your adjacency list graph.
//
//  - ParentMap: associative container between vertex_descriptors and parent
//               vertex_descriptors. This is a representation of the free
//               trees/forests created by these search methods.
//
//  - DistanceMap: associative container between vertex_descriptors and
//                 EdgeProperties. This represents the summation of the path
//                 weights to the vertex itself.
//
// Assume EdgeProperties can be added and compared with less than/less than or
// equal.
//


///@brief Implement breadth-first search.
template<typename Graph, typename ParentMap>
  void breadth_first_search(const Graph& g, ParentMap& p) {
    typedef typename Graph::vertex_descriptor vertex_descriptor;
    typedef typename Graph::edge_descriptor edge_descriptor;
    typedef typename Graph::const_vertex_iterator vertex_iterator;
    typedef typename Graph::const_edge_iterator edge_iterator;
    typedef typename Graph::const_adj_edge_iterator adj_edge_iterator;

    //setup
    std::queue<vertex_descriptor> q;
    std::unordered_set<edge_descriptor, boost::hash<edge_descriptor>> edges_unexplored;
    std::unordered_set<vertex_descriptor> vertices_unexplored;

    //initialize
    p.clear();
    for(vertex_iterator vi = g.vertices_cbegin(); vi != g.vertices_cend(); ++vi) {
      vertex_descriptor vd = (*vi)->descriptor();
      vertices_unexplored.emplace(vd);
      p[vd] = -1;
    }
    for(edge_iterator ei = g.edges_cbegin(); ei != g.edges_cend(); ++ei)
      edges_unexplored.emplace((*ei)->descriptor());

    //for each CC
    for(vertex_iterator vi = g.vertices_cbegin(); vi != g.vertices_cend(); ++vi) {
      vertex_descriptor vd = (*vi)->descriptor();
      if(vertices_unexplored.count(vd)) {
        q.push(vd);
        vertices_unexplored.erase(vd);
        while(!q.empty()) {
          vertex_descriptor vd = q.front();
          q.pop();
          auto& v = *g.find_vertex(vd);
          for(adj_edge_iterator aei = v->begin(); aei != v->end(); ++aei) {
            auto el = edges_unexplored.find((*aei)->descriptor());
            if(el != edges_unexplored.end()) {
              vertex_descriptor t = (*aei)->target();
              if(vertices_unexplored.count(t)) {
                //discovery edge
                edges_unexplored.erase(el);
                p[t] = v->descriptor();
                q.push(t);
                vertices_unexplored.erase(t);
              }
              //else cross edge
            }
          }
        }
      }
    }
  }

/*
  The template function depth_first_search takes in two input arguments, those being a graph g of type Graph and a ParentMap p. It performs a DFS on the input graph and populates the ParentMap with the parent-child relationships discovered during the search. The function initializes aliases for vertex_descriptor, vertex_iterator, and const_adj_edge_iterator types, which are used throughout the function.
  In lines 100-107, the function clears the ParentMap and creates an unordered set called visited_vertices that stores the vertices that have been explored. It then iterates through all the vertices in the graph where it sets the initial parent value of each vertex to -1.
  In lines 110-122,  a helper function named perform_dfs is defined as what’s known as a lambda function which in this case takes a vertex_descriptor u as its input. This function is responsible for the core functionality of the depth-first search as when it’s called with a vertex it first inserts the vertex into the visited set. Then, it finds the corresponding vertex object in the graph and iterates through its adjacent edges using adj_edge_iterator. For each adjacent edge, it will check if the target vertex t has already been visited. If it has not then it updates the ParentMap by setting the parent of the target vertex t to the current vertex u and calls the dfs_visit function recursively for the target vertex t.
  Finally, in lines 125-131, the depth_first_search iterates over all vertices that are in the graph, and for each unvisited vertex it will call the dfs_visit function. This makes sure that the depth-first search algorithm explores all connected components in the graph. After the traversal is complete, the ParentMap will contain the parent-child relationships discovered during the Depth-First Search.

*/


///@todo Implement depth-first search.
// The template function depth_first_search takes in two input arguments, those being a graph g of type Graph and a ParentMap p. It performs a DFS on the input graph and populates the ParentMap with the parent-child relationships discovered during the search. 
template<typename Graph, typename ParentMap>
void depth_first_search(const Graph& g, ParentMap& p) {
  // The function initializes aliases for vertex_descriptor, vertex_iterator, and const_adj_edge_iterator types, which are used throughout the function.
  typedef typename Graph::vertex_descriptor vertex_descriptor;
  typedef typename Graph::const_vertex_iterator vertex_iterator;
  typedef typename Graph::const_adj_edge_iterator adj_edge_iterator;

  // In lines 100-107, the function clears the ParentMap  
  p.clear();
  // creates an unordered set called visited_vertices that stores the vertices that have been explored.
  std::unordered_set<vertex_descriptor> visited_vertices;
  // It then iterates through all the vertices in the graph where it sets the initial parent value of each vertex to -1.
  for (vertex_iterator vi = g.vertices_cbegin(); vi != g.vertices_cend(); ++vi) {
    vertex_descriptor vd = (*vi)->descriptor();
    p[vd] = -1;
  }

  // In lines 110-122,  a helper function named perform_dfs is defined as what’s known as a lambda function which in this case takes a vertex_descriptor u as its input. This function is responsible for the core functionality of the depth-first search as when it’s called with a vertex it first inserts the vertex into the visited set.
  std::function<void(vertex_descriptor)> perform_dfs = [&](vertex_descriptor u) {
    visited_vertices.insert(u);
    auto& v = *g.find_vertex(u);
    // Then, it finds the corresponding vertex object in the graph and iterates through its adjacent edges using adj_edge_iterator.
    for (adj_edge_iterator aei = v->begin(); aei != v->end(); ++aei) {
      vertex_descriptor t = (*aei)->target();
      // For each adjacent edge, it will check if the target vertex t has already been visited. If it has not then it updates the ParentMap by setting the parent of the target vertex t to the current vertex u and calls the dfs_visit function recursively for the target vertex t.
      if (visited_vertices.count(t) == 0) {
        p[t] = u;
        perform_dfs(t);
      }
    }
  };

  // Finally, in lines 125-131, the depth_first_search iterates over all vertices that are in the graph. 
  for (vertex_iterator vi = g.vertices_cbegin(); vi != g.vertices_cend(); ++vi) {
    vertex_descriptor vd = (*vi)->descriptor();
    if (visited_vertices.count(vd) == 0) {
      // For each unvisited vertex it will call the dfs_visit function. This makes sure that the depth-first search algorithm explores all connected components in the graph. After the traversal is complete, the ParentMap will contain the parent-child relationships discovered during the Depth-First Search.
      perform_dfs(vd);
    }
  }
}


#endif
