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


///@todo Implement depth-first search.
template<typename Graph, typename ParentMap>
void depth_first_search(const Graph& g, ParentMap& p) {
    typedef typename Graph::vertex_descriptor vertex_descriptor;
    typedef typename Graph::edge_descriptor edge_descriptor;
    typedef typename Graph::const_vertex_iterator vertex_iterator;
    typedef typename Graph::const_edge_iterator edge_iterator;
    typedef typename Graph::const_adj_edge_iterator adj_edge_iterator;

    //setup: create sets to store the unexplored edges and verticies
    std::unordered_set<edge_descriptor, boost::hash<edge_descriptor>> unex_edgs;
    std::unordered_set<vertex_descriptor> unex_vs;

    //initialize
    p.clear();
    for(vertex_iterator vert_it = g.vertices_cbegin(); vert_it != g.vertices_cend(); ++vert_it) {
      vertex_descriptor vert_desc = (*vert_it)->descriptor();
      unex_vs.emplace(vert_desc);
      p[vert_desc] = -1;
    }
    for(edge_iterator ei = g.edges_cbegin(); ei != g.edges_cend(); ++ei)
      unex_edgs.emplace((*ei)->descriptor());

    //for each CC
    for(vertex_iterator vert_it = g.vertices_cbegin(); vert_it != g.vertices_cend(); ++vert_it) {
      vertex_descriptor vert_desc = (*vert_it)->descriptor();
      if(unex_vs.count(vert_desc)) {
        std::vector<vertex_descriptor> stack{vert_desc};//not actually a stack but works as one. used to pop back and visit unexplored edges in the event of a dead end
        unex_vs.erase(vert_desc);
        while(!stack.empty()) {
          vertex_descriptor vert_desc = stack.back();
          stack.pop_back();
          auto& v = *g.find_vertex(vert_desc);
          for(adj_edge_iterator adj_it = v->begin(); adj_it != v->end(); ++adj_it) {
            auto edg_loc = unex_edgs.find((*adj_it)->descriptor());
            if(edg_loc != unex_edgs.end()) {
              vertex_descriptor t = (*adj_it)->target();
              if(unex_vs.count(t)) {
                //discovery edge
                unex_edgs.erase(edg_loc);
                p[t] = v->descriptor();
                stack.push_back(t);
                unex_vs.erase(t);
              }
              //else cross edge
            }
          }
        }
      }
    }
}



#endif
