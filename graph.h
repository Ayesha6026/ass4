#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <iostream>
#include <list>
#include <utility>
#include <algorithm>
#include <memory>
#include <unordered_set>
#include <set>

#include <boost/functional/hash.hpp>



////////////////////////////////////////////////////////////////////////////////
/// A generic adjacency-list graph where each vertex stores a VertexProperty and
/// each edge stores an EdgeProperty.
////////////////////////////////////////////////////////////////////////////////
template<typename VertexProperty, typename EdgeProperty>
class graph {

  // The vertex and edge classes are forward-declared to allow their use in the
  // public section below. Their definitions follow in the private section
  // afterward.
  class vertex;
  class edge;
  
  struct vertex_hash;
  struct edge_hash;
  struct vertex_eq;
  struct edge_eq;
  struct edge_comp;

  public:

    // Required public types

    /// Unique vertex identifier
    typedef size_t vertex_descriptor;

    /// Unique edge identifier represents pair of vertex descriptors
    typedef std::pair<size_t, size_t> edge_descriptor;

    ///@brief A container for the vertices. It should contain "vertex*" or
    ///      shared_ptr<vertex>.
    typedef std::unordered_set<vertex*, vertex_hash, vertex_eq> MyVertexContainer;

    ///@brief A container for the edges. It should contain "edge*" or
    ///      shared_ptr<edge>.
    typedef std::unordered_set<edge*, edge_hash, edge_eq> MyEdgeContainer;
//	typedef std::set<edge*, edge_comp> MyEdgeContainer;

    ///@brief A container for the adjacency lists. It should contain
    ///      "edge*" or shared_ptr<edge>.
    typedef std::unordered_set<edge*, edge_hash, edge_eq> MyAdjEdgeContainer;
//	typedef std::set<edge*, edge_comp> MyAdjEdgeContainer;
	
	///@brief A container for adjacency matrix. It should contain 
	///       "edge*" or shared_ptr<edge>. 
	typedef std::unordered_set<edge*, vertex_hash, vertex_eq> MyAdjMatrixContainer;

    // Vertex iterators
    typedef typename MyVertexContainer::iterator vertex_iterator;
    typedef typename MyVertexContainer::const_iterator const_vertex_iterator;

    // Edge iterators
    typedef typename MyEdgeContainer::iterator edge_iterator;
    typedef typename MyEdgeContainer::const_iterator const_edge_iterator;

    // Adjacency list iterators
    typedef typename MyAdjEdgeContainer::iterator adj_edge_iterator;
    typedef typename MyAdjEdgeContainer::const_iterator const_adj_edge_iterator;

    // Required graph operations

    ///@brief Constructor/destructor
    graph() : m_max_vd(0) { }

    ~graph() {
        clear();
    }

    graph(const graph&) = delete;             ///< Copy is disabled.
    graph& operator=(const graph&) = delete;  ///< Copy is disabled.

    ///@brief vertex iterator operations
    vertex_iterator vertices_begin() {return m_vertices.begin();}
    const_vertex_iterator vertices_cbegin() const {return m_vertices.cbegin();}
    vertex_iterator vertices_end() {return m_vertices.end();}
    const_vertex_iterator vertices_cend() const {return m_vertices.cend();}
	  
    ///@brief  edge iterator operations
    edge_iterator edges_begin() {return m_edges.begin();}
    const_edge_iterator edges_cbegin() const {return m_edges.cbegin();}
    edge_iterator edges_end() {return m_edges.end();}
    const_edge_iterator edges_cend() const {return m_edges.cend();}

    ///@brief Define accessors
    size_t num_vertices() const {return m_vertices.size();}
    size_t num_edges() const {return m_edges.size();}

    vertex_iterator find_vertex(vertex_descriptor vd) {
      vertex v(vd, VertexProperty());
      return m_vertices.find(&v);
    }

    const_vertex_iterator find_vertex(vertex_descriptor vd) const {
      vertex v(vd, VertexProperty());
      return m_vertices.find(&v);
    }

    edge_iterator find_edge(edge_descriptor ed) {
      edge e(ed.first, ed.second, EdgeProperty());
      return m_edges.find(&e);
    }

    const_edge_iterator find_edge(edge_descriptor ed) const {
      edge e(ed.first, ed.second, EdgeProperty());
      return m_edges.find(&e);
    }

    ///@todo Define modifiers
    vertex_descriptor insert_vertex(const VertexProperty& vp) {
        m_vertices.insert(new vertex(m_max_vd, vp));
        m_max_vd += 1;
	  return m_max_vd;
	}
    edge_descriptor insert_edge(vertex_descriptor sd, vertex_descriptor td,
        const EdgeProperty& ep){
        m_edges.insert(new edge(sd, td, ep));
		return std::make_pair(sd, td);	
	}
    void insert_edge_undirected(vertex_descriptor sd, vertex_descriptor td,
        const EdgeProperty& ep){
			m_edges.insert(new edge(sd, td, ep));
	}
    void erase_vertex(vertex_descriptor vd){
		auto found = find_vertex(vd);
        m_vertices.erase(found);
        m_max_vd -= 1;
	}
    void erase_edge(edge_descriptor ed){
		auto found = find_edge(ed);
        m_edges.erase(found);
	}
	////end of @todo
	
    void clear() {
      m_max_vd = 0;
      for(auto v : m_vertices)
        delete v;
      m_vertices.clear();
      for(auto e : m_edges)
        delete e;
      m_edges.clear();
    }

    // Friend declarations for input/output.
    template<typename V, typename E>
    friend std::istream& operator>>(std::istream&, graph<V, E>&);
    template<typename V, typename E>
    friend std::ostream& operator<<(std::ostream&, const graph<V, E>&);

  private:
	size_t m_max_vd; //< Maximum vertex descriptor assigned
    MyVertexContainer m_vertices; //<Contains all vertices
    MyEdgeContainer m_edges;    //<Contains all edges
    // Required internal classes

    class vertex {
        public:
          ///required constructors/destructors
          vertex(vertex_descriptor vd, const VertexProperty& v) :
            m_descriptor(vd), m_property(v) { }

          ///required vertex operations

          //iterators
          adj_edge_iterator begin() {return m_out_edges.begin();}
          const_adj_edge_iterator cbegin() const {return m_out_edges.cbegin();}
          adj_edge_iterator end() {return m_out_edges.end();}
          const_adj_edge_iterator cend() const {return m_out_edges.cend();}

          //accessors
          const vertex_descriptor descriptor() const {return m_descriptor;}
          VertexProperty& property() {return m_property;}
          const VertexProperty& property() const {return m_property;}

        private:

          vertex_descriptor m_descriptor; // Unique id for the vertex - assigned during insertion
          VertexProperty m_property;      // Label or property of the vertex - passed during insertion
          MyAdjEdgeContainer m_out_edges; // Container that includes the out edges

          friend class graph;
    };


    ////////////////////////////////////////////////////////////////////////////
    /// Edges represent the connections between nodes in the graph.
    ////////////////////////////////////////////////////////////////////////////
    class edge {
      public:
        ///required constructors/destructors
        edge(vertex_descriptor s, vertex_descriptor t,
              const EdgeProperty& w) : m_source(s), m_target(t), m_property(w) { }

        ///required edge operations

        //accessors
        const vertex_descriptor source() const {return m_source;}
        const vertex_descriptor target() const {return m_target;}
        const edge_descriptor descriptor() const {return {m_source, m_target};}
        EdgeProperty& property() {return m_property;}
        const EdgeProperty& property() const {return m_property;}

      private:
        vertex_descriptor m_source; // Unique id of the source vertex
        vertex_descriptor m_target; // Unique id of the target vertex 
        EdgeProperty m_property;    // Label or weight of the edge 
    };
	
	struct vertex_hash {
        size_t operator()(vertex* const& v) const {
          return h(v->descriptor());
        }
        std::hash<vertex_descriptor> h;
    };

    struct edge_hash {
	  // You can re-write this function to create the hash-value for a pair i.e., edge descriptor
	  // instead of using boost::hash
      size_t operator()(edge* const& e) const {
        return h(e->descriptor());
	  }
      boost::hash<edge_descriptor> h;
    };

    struct vertex_eq {
      bool operator()(vertex* const& u, vertex* const& v) const {
        return u->descriptor() == v->descriptor();
      }
    };

    struct edge_eq {
      bool operator()(edge* const& e, edge* const& f) const {
        return e->descriptor() == f->descriptor();
      }
    };
	
	struct edge_comp {
	  bool operator()(edge* const& e, edge* const& f) const {
        return e->descriptor() < f->descriptor();
      }
	};
};

///@brief Define io operations for the graph.
template<typename V, typename E>
std::istream& operator>>(std::istream& is, graph<V, E>& g) {
    size_t num_verts, num_edges;
    is >> num_verts >> num_edges;
    g.m_vertices.reserve(num_verts);
    g.m_vertices.reserve(num_edges);
    for(size_t i = 0; i < num_verts; ++i) {
      V v;
      is >> v;
      g.insert_vertex(v);
    }
    for(size_t i = 0; i < num_edges; ++i) {
      typename graph<V, E>::vertex_descriptor s, t;
      E e;
      is >> s >> t >> e;
      g.insert_edge(s, t, e);
    }
    return is;
}

template<typename V, typename E>
std::ostream& operator<<(std::ostream& os, const graph<V, E>& g) {
    os << g.num_vertices() << " " << g.num_edges() << std::endl;
    for(auto i = g.vertices_cbegin(); i != g.vertices_cend(); ++i)
      os << (*i)->property() << std::endl;
    for(auto i = g.edges_cbegin(); i != g.edges_cend(); ++i)
      os << (*i)->source() << " " << (*i)->target() << " "
        << (*i)->property() << std::endl;
    return os;
}


#endif
