#include "graph.h"
#include "graph_dumb_vector.h"
#include <iostream>

using namespace std;

template <typename graphID>
void test_graph()
{
  typedef typename graphID::edge_descriptor ED;
  typedef typename graphID::vertex_iterator VI;
  typedef typename graphID::const_vertex_iterator CVI;
  typedef typename graphID::edge_iterator EI;
  typedef typename graphID::const_edge_iterator CEI;

  graphID g;
  g.insert_vertex(5);
  g.insert_vertex(4);
  g.insert_edge(0, 1, 0.5);
  g.insert_edge(1, 0, 0.25);

  for (VI vi = g.vertices_begin(); vi != g.vertices_end(); ++vi)
    cout << (*vi)->descriptor() << " " << (*vi)->property() << endl;
  for (CVI vi = g.vertices_cbegin(); vi != g.vertices_cend(); ++vi)
    cout << (*vi)->descriptor() << " " << (*vi)->property() << endl;
  for (EI ei = g.edges_begin(); ei != g.edges_end(); ++ei)
    cout << (*ei)->source() << " " << (*ei)->target() << " " << (*ei)->property() << endl;
  for (CEI ei = g.edges_cbegin(); ei != g.edges_cend(); ++ei)
    cout << (*ei)->source() << " " << (*ei)->target() << " " << (*ei)->property() << endl;

  cout << (*g.find_vertex(0))->property() << " " << (*g.find_edge(ED(0, 1)))->property() << endl;

  g.erase_edge(ED(0, 1));
  g.erase_vertex(0);

  cout << g;
}

int main()
{
  typedef graph<int, double> setGraph;
  typedef graph_vector<int, double> vectorGraph;

  test_graph<vectorGraph>();

  test_graph<setGraph>();
}
