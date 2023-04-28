#ifndef _GRAPH_ALGORITHMS_H_
#define _GRAPH_ALGORITHMS_H_

#include <queue>
#include <unordered_set>
#include <boost/functional/hash.hpp> // Comment this if you haven't boost installed
#include <stack>


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


/////@todo Implement depth-first search.
/// All I did for the dfs was use the given bfs code and transistion it from queue-based to stack-based.
/// I did have to use #include <stack>, but since #include <queue> is also there, I am guessing it's allowed.
template<typename Graph, typename ParentMap>
void depth_first_search(const Graph& g, ParentMap& p) {

    // declare descriptors and iterators
    typedef typename Graph::vertex_descriptor vertex_descriptor;
    typedef typename Graph::edge_descriptor edge_descriptor;
    typedef typename Graph::const_vertex_iterator vertex_iterator;
    typedef typename Graph::const_edge_iterator edge_iterator;
    typedef typename Graph::const_adj_edge_iterator adj_edge_iterator;

    // sets up the stack and two containers to store unexplored egdes and vertices
    std::stack<vertex_descriptor> s;
    std::unordered_set<edge_descriptor, boost::hash<edge_descriptor>> edges_unexplored;
    std::unordered_set<vertex_descriptor> vertices_unexplored;


    p.clear();
    // The first for loop makes sure that all vertices are visited during the search by putting the
    // vertex descriptor in the vertices_unexplored container using the .emplace method, marking them as unexplored.
    for (vertex_iterator vi = g.vertices_cbegin(); vi != g.vertices_cend(); ++vi) {
        vertex_descriptor vd = (*vi)->descriptor();
        vertices_unexplored.emplace(vd);
        p[vd] = -1;
    }
    // The second for loop is similar to the first one, but this time is makes sure all edges are visited in the search.
    // It does this by going over all edges and inserting the descriptor into the edges_unexplored containers using
    // the .emplace method. 
    for (edge_iterator ei = g.edges_cbegin(); ei != g.edges_cend(); ++ei)
        edges_unexplored.emplace((*ei)->descriptor());


    // The third for loop iterates over all the vertices in the graph, and for each one checks if the vertex has been 
    // visited by seeing if the vertex descriptor is in the vertices_unexplored container. If it is unexplored, the descriptor
    // is pushed onto the stack and is erased from the vertices_unexplored container. While the stack is not empty,
    // the vertex descriptor on the top of the stack is obtained using the .top() method, and then popped off using the
    // .pop() method. We then find the vertex of the popped of vertex descriptor so we can look at its adjacent edges.
    // For all of its adjacent edges, we check if the edge has been explored by seeing it its descriptor is in the edges_unexplored container.
    // If the edge is unexplored, we see if the edge's other vertex is unexplored using the same method. If that other vertex turns out
    // to be unexplored, we mark the current edge as explored by removing it from the edges_unexplored container. The other vertex
    // is pushed onto the stack and removed from the vertices_unexplored container, and the cycle repeats.
    for (vertex_iterator vi = g.vertices_cbegin(); vi != g.vertices_cend(); ++vi) {
        vertex_descriptor vd = (*vi)->descriptor();
        if (vertices_unexplored.count(vd)) {
            s.push(vd);
            vertices_unexplored.erase(vd);
            while (!s.empty()) {
                vertex_descriptor vd = s.top();
                s.pop();
                auto& v = *g.find_vertex(vd);
                for (adj_edge_iterator aei = v->begin(); aei != v->end(); ++aei) {
                    auto el = edges_unexplored.find((*aei)->descriptor());
                    if (el != edges_unexplored.end()) {
                        vertex_descriptor t = (*aei)->target();
                        if (vertices_unexplored.count(t)) {
                            edges_unexplored.erase(el);
                            p[t] = v->descriptor();
                            s.push(t);
                            vertices_unexplored.erase(t);
                        }
                    }
                }
            }
        }
    }
}


#endif
