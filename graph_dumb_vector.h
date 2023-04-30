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

      /*
        This function inserts a new vertex into the graph by taking in a constant reference to a VertexProperty object as its input parameter vp. It initializes a new vertex_descriptor named new_vertex_descriptor, setting it to the current maximum vertex descriptor m_max_vd. An important step is m_max_vd  is then incremented for future insertions. A new vertex object, new_v is then created using new_vertex_descriptor and vp as arguments. The new vertex is then added to the vertex container m_vertices. Where finally, the function returns the descriptor new_vertex_descriptor for the newly inserted vertex.
      */

      vertex_descriptor insert_vertex(const VertexProperty& vp) {
        // It initializes a new vertex_descriptor named new_vertex_descriptor, setting it to the current maximum vertex descriptor m_max_vd. An important step is m_max_vd  is then incremented for future insertions.
        vertex_descriptor new_vertex_descriptor = m_max_vd++;
        // A new vertex object, new_v is then created using new_vertex_descriptor and vp as arguments.
        vertex* new_v = new vertex(new_vertex_descriptor, vp);
        // The new vertex is then added to the vertex container m_vertices.
        m_vertices.push_back(new_v);
        // Where finally, the function returns the descriptor new_vertex_descriptor for the newly inserted vertex.
        return new_vertex_descriptor;
      }

      /*
        This function inserts a new edge into the graph by taking two vertex descriptors sd and td which represent the source and target vertices, and a constant reference to an EdgeProperty object ep. Firstly, this function begins by creating a new edge object, new_edge, using sd, td, and ep as arguments. The new edge is then added to the edge container m_edges. The function searches for the source vertex sd using the find_vertex method, where if found adds the new edge to the source vertex's adjacency list m_out_edges. Finally, the function returns a pair of vertex descriptors, (sd, td), as the edge descriptor for the newly inserted edge.
      */

      edge_descriptor insert_edge(vertex_descriptor sd, vertex_descriptor td, const EdgeProperty& ep) {
        // Firstly, this function begins by creating a new edge object, new_edge, using sd, td, and ep as arguments.
        edge* new_edge = new edge(sd, td, ep);
        // The new edge is then added to the edge container m_edges.
        m_edges.push_back(new_edge);
        // The function searches for the source vertex sd using the find_vertex method
        vertex_iterator source_iterator = find_vertex(sd);
        // where if found adds the new edge to the source vertex's adjacency list m_out_edges.
        if (source_iterator != m_vertices.end()) {
          (*source_iterator)->m_out_edges.push_back(new_edge);
        }
        // Finally, the function returns a pair of vertex descriptors, (sd, td), as the edge descriptor for the newly inserted edge.
        return std::make_pair(sd, td);
      }

      /*
        This function inserts an undirected edge into the graph, taking three input parameters being two vertex_descriptor objects, sd and td, representing the source and target vertices of the edge, and a constant reference to an EdgeProperty object, ep. Now, to create an undirected edge, the function calls the insert_edge() function twice. The reasoning for this is in the first call it creates an edge from the source vertex sd to the target vertex td with the given edge property ep. Now with the second call it creates another edge in the opposite direction, from the target vertex td to the source vertex sd, also with the edge property ep. This result ensures that the graph represents an undirected edge between the two vertices.
      */
      void insert_edge_undirected(vertex_descriptor sd, vertex_descriptor td, const EdgeProperty& ep) {
        // Now, to create an undirected edge, the function calls the insert_edge() function twice.
        // The reasoning for this is in the first call it creates an edge from the source vertex sd to the target vertex td with the given edge property ep.
        insert_edge(sd, td, ep);
        // Now with the second call it creates another edge in the opposite direction, from the target vertex td to the source vertex sd, also with the edge property ep.
        insert_edge(td, sd, ep);
        // This result ensures that the graph represents an undirected edge between the two vertices.
      }

      /*
        This function erases a vertex from the graph by taking a vertex descriptor vd as its only input. It starts by finding the vertex iterator vertex_it for the given vertex descriptor. If the vertex iterator is not at the end of the vertex container (!=), this means the vertex exists currently in the graph. The function then iterates through all the outgoing edges of the vertex v and calls the erase_edge function for each edge descriptor. After removing all edges connected to the vertex, it then deletes the vertex object v and erases the vertex iterator vertex_it from the vertex container m_vertices.
      */

      void erase_vertex(vertex_descriptor vd) {
        // It starts by finding the vertex iterator vertex_it for the given vertex descriptor.
        vertex_iterator vertex_it = find_vertex(vd);
        // If the vertex iterator is not at the end of the vertex container (!=), this means the vertex exists currently in the graph.
        if (vertex_it != m_vertices.end()) {
          // The function then iterates through all the outgoing edges of the vertex v
          vertex* v = *vertex_it;
          for (auto e : v -> m_out_edges) {
            // calls the erase_edge function for each edge descriptor.
            erase_edge(e -> descriptor());
          }
          // After removing all edges connected to the vertex, it then deletes the vertex object v
          delete v;
          // erases the vertex iterator vertex_it from the vertex container m_vertices.
          m_vertices.erase(vertex_it);
        }
      }

      /*
        This function erases an edge from the graph by taking a single input parameter edge_descriptor ed, which highlights the edge to be removed. The function starts by finding the edge iterator e_iterator using the find_edge() function. If the edge is found in the graph then the function continues to remove the edge from the source vertex's adjacency list. The way this is down is by obtaining the vertex iterator v_iterator of the source vertex by calling the find_vertex() function with e->source(). If the source vertex is successfully found then the function accesses the vertex's m_out_edges container and removes the specified edge e. Lastly, the function eliminates the edge from the graph's edge container m_edges and deallocates the memory used by the edge object using delete e.
      */


      void erase_edge(edge_descriptor ed) {
        // The function starts by finding the edge iterator e_iterator using the find_edge() function.
        edge_iterator e_iterator = find_edge(ed);
        // If the edge is found in the graph then the function continues to remove the edge from the source vertex's adjacency list.
        if (e_iterator != m_edges.end()) {
          edge* e = *e_iterator;
          // The way this is down is by obtaining the vertex iterator v_iterator of the source vertex by calling the find_vertex() function with e->source().
          vertex_iterator v_iterator = find_vertex(e->source());
          if (v_iterator != m_vertices.end()) {
            vertex* v = *v_iterator;
            //  If the source vertex is successfully found then the function accesses the vertex's m_out_edges container and removes the specified edge e.
            auto edge_to_remove = std::find(v->m_out_edges.begin(), v->m_out_edges.end(), e);
            if (edge_to_remove != v->m_out_edges.end()) {
              v->m_out_edges.erase(edge_to_remove);
            }
          }

          // Lastly, the function eliminates the edge from the graph's edge container m_edges and deallocates the memory used by the edge object using delete e.
          m_edges.erase(e_iterator);
          delete e;
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
