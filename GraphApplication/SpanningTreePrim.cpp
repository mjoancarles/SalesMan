#include "pch.h"
#include "Graph.h"
#include <vector>
#include <queue>
using namespace std;

// =============================================================================
// SpanningTreePrim ============================================================
// =============================================================================


CSpanningTree SpanningTreePrim(CGraph& graph)
{
	struct comparator {
		bool operator()(CEdge* pE1, CEdge* pE2) {
			return pE1->m_Length > pE2->m_Length;
		}
	};
	priority_queue<CEdge*, std::vector<CEdge*>, comparator> queue;

	CSpanningTree tree(&graph);

	for (CVertex& v : graph.m_Vertices) v.m_PrimInTree = false;
	CVertex* vertex = &graph.m_Vertices.front();
	vertex->m_PrimInTree = true;
	for (CEdge* e : vertex->m_Edges) queue.push(e);
	while (!queue.empty())
	{
		CEdge* aresta = queue.top();
		queue.pop();
		if (aresta->m_pDestination->m_PrimInTree) continue;
		tree.Add(aresta);
		vertex = aresta->m_pDestination;
		vertex->m_PrimInTree = true;
		for (CEdge* e : vertex->m_Edges) if (!e->m_pDestination->m_PrimInTree) queue.push(e);
	}
	return tree;
}
