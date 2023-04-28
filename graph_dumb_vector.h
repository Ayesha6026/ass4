#ifndef _GRAPH_VECTOR_H_
#define _GRAPH_VECTOR_H_

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>


template<typename VertexProperty, typename EdgeProperty>
  class graph_vector {

    //you have to forward declare these so you can use them in the public
    //section, when they are defined in the private section below. Essentially,
    //the compiler needs to know they exist in order to use them before the
    //definition of them.
    class vertex;
    class edge;

    public:
      ///required public types
      typedef size_t vertex_descriptor; //unique vertex identifier

      typedef std::pair<size_t, size_t> edge_descriptor; //unique edge identifier
                                                    //represents pair of vertex
                                                    //descriptors

      typedef std::vector<vertex*> vertex_storage;
      typedef std::vector<edge*> edge_storage;
      typedef std::vector<edge*> adj_edge_storage;
      //vertex container should contain "vertex*" or shared_ptr<vertex>
      typedef typename vertex_storage::iterator vertex_iterator; //vertex iterators
      typedef typename vertex_storage::const_iterator const_vertex_iterator;

      //edge container should contain "edge*" or shared_ptr<edge>
      typedef typename edge_storage::iterator edge_iterator; //edge iterators
      typedef typename edge_storage::const_iterator const_edge_iterator;

      //adjacent edge container should contain "edge*" or shared_ptr<edge>
      typedef typename adj_edge_storage::iterator adj_edge_iterator; //adjacency list iterators
      typedef typename adj_edge_storage::const_iterator const_adj_edge_iterator;

      ///required constructor/destructors
      graph_vector() : m_max_vd(0) { }

      ~graph_vector() {
        clear();
      }

      graph_vector(const graph_vector&) = delete;
      graph_vector& operator=(const graph_vector&) = delete;

      ///required graph operations

      //iterators
      vertex_iterator vertices_begin() {return m_vertices.begin();}
      const_vertex_iterator vertices_cbegin() const {return m_vertices.cbegin();}
      vertex_iterator vertices_end() {return m_vertices.end();}
      const_vertex_iterator vertices_cend() const {return m_vertices.cend();}

      edge_iterator edges_begin() {return m_edges.begin();}
      const_edge_iterator edges_cbegin() const {return m_edges.cbegin();}
      edge_iterator edges_end() {return m_edges.end();}
      const_edge_iterator edges_cend() const {return m_edges.cend();}

      //accessors
      size_t num_vertices() const {return m_vertices.size();}
      size_t num_edges() const {return m_edges.size();}

      vertex_iterator find_vertex(vertex_descriptor vd) {
        return std::find_if(m_vertices.begin(), m_vertices.end(),
            [&](const vertex* const v) {
            return v->descriptor() == vd;
            });
      }

      const_vertex_iterator find_vertex(vertex_descriptor vd) const {
        return std::find_if(m_vertices.cbegin(), m_vertices.cend(),
            [&](const vertex* const v) {
            return v->descriptor() == vd;
            });
      }

      edge_iterator find_edge(edge_descriptor ed) {
        return std::find_if(m_edges.begin(), m_edges.end(),
            [&](const edge* const e) {
            return e->descriptor() == ed;
            });
      }

      const_edge_iterator find_edge(edge_descriptor ed) const {
        return std::find_if(m_edges.cbegin(), m_edges.cend(),
            [&](const edge* e) {
            return e->descriptor() == ed;
            });
      }

      //@todo modifiers

      // Creates vertex object with a descriptor of m_max_vd and the passed in vertex property. This new object is added to
      // the end of the m_vertices container. m_max_vd is incremented by one so that each vertex has a different descriptor. The function
      // returns the descriptor of the new vertex object.
      vertex_descriptor insert_vertex(const VertexProperty& vp) {
          vertex* newVertex = new vertex(m_max_vd, vp);
          m_vertices.push_back(newVertex);
          m_max_vd++;
          return newVertex->descriptor();
      }

      // Creates edge object with passed in descriptors of the source and target vertices, as well as the edge property. This new object
      // is added to the end of the m_edges container. New vertex iterators are created for the source and target vertices of the current edge.
      // If both source and target vertices are found in the m_vertices container, then the edge pointer of the new edge is added to the m_out_edges container of the 
      // source vertex. We then return the descriptor of the new edge.
      edge_descriptor insert_edge(vertex_descriptor sd, vertex_descriptor td,
          const EdgeProperty& ep) {
          edge* newEdge = new edge(sd, td, ep);
          m_edges.push_back(newEdge);

          vertex_iterator startVertexIterator = find_vertex(sd);
          vertex_iterator endVertexIterator = find_vertex(td);

          if (startVertexIterator != m_vertices.end() && endVertexIterator != m_vertices.end()) {
              (*startVertexIterator)->m_out_edges.push_back(newEdge);
          }

          return newEdge->descriptor();
      }

      // Two insert_edge functions are called so that a directed edge is created from the source vertex to the target vertex
      // and also from the target vertex to the source vertex. Since it's two directed edges going to and from the same vertices in opposite directions, the result is an undirected edge.
      void insert_edge_undirected(vertex_descriptor sd, vertex_descriptor td,
          const EdgeProperty& ep) {
          insert_edge(sd, td, ep);
          insert_edge(td, sd, ep);
      }

      // A vertex iterator object is created by finding the descriptor of the object we want to erase. If the vertex iterator is
      // in the m_vertices container, we use a dereferenced iterator to retreive the pointer.
      // We then create an edge iterator using the start of the m_out_edges container. While this created edge iterator, which contains the out edges of the vertex,
      // is still in the set of the vertex's out edges, we erase the edges in the m_out_edges container by usng the descriptor of the current edge pointed to by the created edge iterator.
      // This loop continues until m_out_edges is empty. After that, the vertex iterator is erased from the m_vertices container, and the derefereced iterator is deleted.
      void erase_vertex(vertex_descriptor vd) {
          vertex_iterator vertexIterator = find_vertex(vd);

          if (vertexIterator != m_vertices.end()) {
              vertex* v = *vertexIterator;
              auto edgeIterator = v->m_out_edges.begin();
              while (edgeIterator != v->m_out_edges.end()) {
                  erase_edge((*edgeIterator)->descriptor());
                  edgeIterator = v->m_out_edges.begin();
              }
              m_vertices.erase(vertexIterator);
              delete v;
          }
      }

      // An edge iterator is created by finding the descriptor of the edge we want to erase. If the edge iterator is
      // in the m_edges container, then we retrieve the iterator pointing to the source vertex of the current edge. If this iterator is in the
      // m_edges container, we iterate over the out edges of the source vertex, and for every out edge, if its descriptor matches the one
      // passed into the function, we erase that out edge. If the descriptor doesn't match, then the iterator increments and the loop continues.
      // After all that, the edge object is deleted and the edge iterator is erased from the m_edges container.
      void erase_edge(edge_descriptor ed) {
          edge_iterator edgeIterator = find_edge(ed);

          if (edgeIterator != m_edges.end()) {
              edge* e = *edgeIterator;
              vertex_iterator startVertexIterator = find_vertex(e->source());

              if (startVertexIterator != m_vertices.end()) {
                  adj_edge_storage& outEdges = (*startVertexIterator)->m_out_edges;
                  auto outEdgesIterator = outEdges.begin();
                  while (outEdgesIterator != outEdges.end()) {
                      if ((*outEdgesIterator)->descriptor() == ed) {
                          outEdgesIterator = outEdges.erase(outEdgesIterator);
                          break;
                      }
                      outEdgesIterator++;
                  }
              }

              delete e;
              m_edges.erase(edgeIterator);
          }
      }
      // end @todo
      void clear() {
        m_max_vd = 0;
        for(auto v : m_vertices)
          delete v;
        m_vertices.clear();
        for(auto e : m_edges)
          delete e;
        m_edges.clear();
      }

      template<typename V, typename E>
        friend std::istream& operator>>(std::istream& is, graph<V, E>& g);

      template<typename V, typename E>
        friend std::ostream& operator<<(std::ostream& os, const graph<V, E>& g);

    private:

      size_t m_max_vd; // Id generator for next vertex to be inserted
      vertex_storage m_vertices;  // List of all vertices in the graph
      edge_storage m_edges;    // List of  all edges in the graph

      ///required internal classes

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
          vertex_descriptor m_descriptor; // Unique id assigned during insertion
          VertexProperty m_property;      // Label or weight passed during insertion
          adj_edge_storage m_out_edges;   // Outgoing edges
  
          friend class graph_vector;
      };

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
          vertex_descriptor m_source; // Descriptor of source vertex
          vertex_descriptor m_target;  // Descriptor of target vertex
          EdgeProperty m_property;    // Label or weight on the edge
      };

  };

template<typename V, typename E>
  std::istream& operator>>(std::istream& is, graph_vector<V, E>& g) {
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
  std::ostream& operator<<(std::ostream& os, const graph_vector<V, E>& g) {
    os << g.num_vertices() << " " << g.num_edges() << std::endl;
    for(auto i = g.vertices_cbegin(); i != g.vertices_cend(); ++i)
      os << (*i)->property() << std::endl;
    for(auto i = g.edges_cbegin(); i != g.edges_cend(); ++i)
      os << (*i)->source() << " " << (*i)->target() << " "
        << (*i)->property() << std::endl;
    return os;
  }



#endif
