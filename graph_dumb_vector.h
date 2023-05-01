#ifndef _GRAPH_VECTOR_H_
#define _GRAPH_VECTOR_H_

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

template <typename VertexProperty, typename EdgeProperty>
class graph_vector
{

  // you have to forward declare these so you can use them in the public
  // section, when they are defined in the private section below. Essentially,
  // the compiler needs to know they exist in order to use them before the
  // definition of them.
  class vertex;
  class edge;

public:
  /// required public types
  typedef size_t vertex_descriptor; // unique vertex identifier

  typedef std::pair<size_t, size_t> edge_descriptor; // unique edge identifier
                                                     // represents pair of vertex
                                                     // descriptors

  typedef std::vector<vertex *> vertex_storage;
  typedef std::vector<edge *> edge_storage;
  typedef std::vector<edge *> adj_edge_storage;
  // vertex container should contain "vertex*" or shared_ptr<vertex>
  typedef typename vertex_storage::iterator vertex_iterator; // vertex iterators
  typedef typename vertex_storage::const_iterator const_vertex_iterator;

  // edge container should contain "edge*" or shared_ptr<edge>
  typedef typename edge_storage::iterator edge_iterator; // edge iterators
  typedef typename edge_storage::const_iterator const_edge_iterator;

  // adjacent edge container should contain "edge*" or shared_ptr<edge>
  typedef typename adj_edge_storage::iterator adj_edge_iterator; // adjacency list iterators
  typedef typename adj_edge_storage::const_iterator const_adj_edge_iterator;

  /// required constructor/destructors
  graph_vector() : m_max_vd(0) {}

  ~graph_vector()
  {
    clear();
  }

  graph_vector(const graph_vector &) = delete;
  graph_vector &operator=(const graph_vector &) = delete;

  /// required graph operations

  // iterators
  vertex_iterator vertices_begin() { return m_vertices.begin(); }
  const_vertex_iterator vertices_cbegin() const { return m_vertices.cbegin(); }
  vertex_iterator vertices_end() { return m_vertices.end(); }
  const_vertex_iterator vertices_cend() const { return m_vertices.cend(); }

  edge_iterator edges_begin() { return m_edges.begin(); }
  const_edge_iterator edges_cbegin() const { return m_edges.cbegin(); }
  edge_iterator edges_end() { return m_edges.end(); }
  const_edge_iterator edges_cend() const { return m_edges.cend(); }

  // accessors
  size_t num_vertices() const { return m_vertices.size(); }
  size_t num_edges() const { return m_edges.size(); }

  vertex_iterator find_vertex(vertex_descriptor vd)
  {
    return std::find_if(m_vertices.begin(), m_vertices.end(),
                        [&](const vertex *const v)
                        {
                          return v->descriptor() == vd;
                        });
  }

  const_vertex_iterator find_vertex(vertex_descriptor vd) const
  {
    return std::find_if(m_vertices.cbegin(), m_vertices.cend(),
                        [&](const vertex *const v)
                        {
                          return v->descriptor() == vd;
                        });
  }

  edge_iterator find_edge(edge_descriptor ed)
  {
    return std::find_if(m_edges.begin(), m_edges.end(),
                        [&](const edge *const e)
                        {
                          return e->descriptor() == ed;
                        });
  }

  const_edge_iterator find_edge(edge_descriptor ed) const
  {
    return std::find_if(m_edges.cbegin(), m_edges.cend(),
                        [&](const edge *e)
                        {
                          return e->descriptor() == ed;
                        });
  }

  vertex_descriptor insert_vertex(const VertexProperty &vp)
  {
    auto v = new vertex(m_max_vd++, vp);
    m_vertices.push_back(v);
    return v->descriptor();
  }

  edge_descriptor insert_edge(vertex_descriptor sd, vertex_descriptor td, const EdgeProperty &ep)
  {
    auto s = find_vertex(sd);
    auto t = find_vertex(td);
    auto e = new edge(*s, *t, ep);
    m_edges.push_back(e);
    s->m_out_edges.push_back(e);
    return e->descriptor();
  }

  void insert_edge_undirected(vertex_descriptor sd, vertex_descriptor td, const EdgeProperty &ep)
  {
    insert_edge(sd, td, ep);
    insert_edge(td, sd, ep);
  }

  void erase_vertex(vertex_descriptor vd)
  {
    auto v = find_vertex(vd);
    if (v == m_vertices.end())
      return;
    auto e_it = v->m_out_edges.begin();
    while (e_it != v->m_out_edges.end())
    {
      erase_edge((*e_it)->descriptor());
    }
    m_vertices.erase(v);
    delete *v;
  }

  void erase_edge(edge_descriptor ed)
  {
    auto e = find_edge(ed);
    if (e == m_edges.end())
      return;
    auto s = find_vertex(ed.first);
    s->m_out_edges.erase(std::remove(s->m_out_edges.begin(), s->m_out_edges.end(), *e), s->m_out_edges.end());
    m_edges.erase(e);
    delete *e;
  }
  void clear()
  {
    m_max_vd = 0;
    for (auto v : m_vertices)
      delete v;
    m_vertices.clear();
    for (auto e : m_edges)
      delete e;
    m_edges.clear();
  }

  template <typename V, typename E>
  friend std::istream &operator>>(std::istream &is, graph<V, E> &g);

  template <typename V, typename E>
  friend std::ostream &operator<<(std::ostream &os, const graph<V, E> &g);

private:
  size_t m_max_vd;           // Id generator for next vertex to be inserted
  vertex_storage m_vertices; // List of all vertices in the graph
  edge_storage m_edges;      // List of  all edges in the graph

  /// required internal classes

  class vertex
  {
  public:
    /// required constructors/destructors
    vertex(vertex_descriptor vd, const VertexProperty &v) : m_descriptor(vd), m_property(v) {}

    /// required vertex operations

    // iterators
    adj_edge_iterator begin() { return m_out_edges.begin(); }
    const_adj_edge_iterator cbegin() const { return m_out_edges.cbegin(); }
    adj_edge_iterator end() { return m_out_edges.end(); }
    const_adj_edge_iterator cend() const { return m_out_edges.cend(); }

    // accessors
    const vertex_descriptor descriptor() const { return m_descriptor; }
    VertexProperty &property() { return m_property; }
    const VertexProperty &property() const { return m_property; }

  private:
    vertex_descriptor m_descriptor; // Unique id assigned during insertion
    VertexProperty m_property;      // Label or weight passed during insertion
    adj_edge_storage m_out_edges;   // Outgoing edges

    friend class graph_vector;
  };

  class edge
  {
  public:
    /// required constructors/destructors
    edge(vertex_descriptor s, vertex_descriptor t,
         const EdgeProperty &w) : m_source(s), m_target(t), m_property(w) {}

    /// required edge operations

    // accessors
    const vertex_descriptor source() const { return m_source; }
    const vertex_descriptor target() const { return m_target; }
    const edge_descriptor descriptor() const { return {m_source, m_target}; }
    EdgeProperty &property() { return m_property; }
    const EdgeProperty &property() const { return m_property; }

  private:
    vertex_descriptor m_source; // Descriptor of source vertex
    vertex_descriptor m_target; // Descriptor of target vertex
    EdgeProperty m_property;    // Label or weight on the edge
  };
};

template <typename V, typename E>
std::istream &operator>>(std::istream &is, graph_vector<V, E> &g)
{
  size_t num_verts, num_edges;
  is >> num_verts >> num_edges;
  g.m_vertices.reserve(num_verts);
  g.m_vertices.reserve(num_edges);
  for (size_t i = 0; i < num_verts; ++i)
  {
    V v;
    is >> v;
    g.insert_vertex(v);
  }
  for (size_t i = 0; i < num_edges; ++i)
  {
    typename graph<V, E>::vertex_descriptor s, t;
    E e;
    is >> s >> t >> e;
    g.insert_edge(s, t, e);
  }
  return is;
}

template <typename V, typename E>
std::ostream &operator<<(std::ostream &os, const graph_vector<V, E> &g)
{
  os << g.num_vertices() << " " << g.num_edges() << std::endl;
  for (auto i = g.vertices_cbegin(); i != g.vertices_cend(); ++i)
    os << (*i)->property() << std::endl;
  for (auto i = g.edges_cbegin(); i != g.edges_cend(); ++i)
    os << (*i)->source() << " " << (*i)->target() << " "
       << (*i)->property() << std::endl;
  return os;
}

#endif
