#ifndef _GRAPH_ALGORITHMS_H_
#define _GRAPH_ALGORITHMS_H_

#include <queue>
#include <unordered_set>
#include <boost/functional/hash.hpp> // Comment this if you haven't boost installed
#include <functional>


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


///@todo Implement depth-first search.
template<typename Graph, typename ParentMap>
void depth_first_search(const Graph& g, ParentMap& p){
  typedef typename Graph::vertex_descriptor vertex_descriptor;
  typedef typename Graph::edge_descriptor edge_descriptor;
  typedef typename Graph::const_vertex_iterator VI;
  //typedef typename Graph::const_edge_iterator EI;
  typedef typename Graph::const_adj_edge_iterator AEI;

  std::unordered_set<vertex_descriptor> visited_vertices;
  std::unordered_set<edge_descriptor, boost::hash<edge_descriptor>> back_edges;
  std::unordered_set<edge_descriptor, boost::hash<edge_descriptor>> discovery_edges;

  std::function<void(VI)> search = [&](VI v) {
    if (!visited_vertices.count((*v)->descriptor())) {
      visited_vertices.insert((*v)->descriptor());
      for (AEI adj_edg = (*v)->begin(); adj_edg != (*v)->end(); adj_edg++) {
        
        if (!(discovery_edges.count((*adj_edg)->descriptor()))) {
          search(g.find_vertex((*adj_edg)->target()));
          discovery_edges.insert(((*adj_edg)->descriptor()));
        
        } else if (!(back_edges.count((*adj_edg)->descriptor()))) {
          back_edges.insert((*adj_edg)->descriptor());
        }
      
      }
    }
  };

  for (VI v = g.vertices_cbegin(); v != g.vertices_cend(); v++) {
    search(v);
  }
}

#endif
