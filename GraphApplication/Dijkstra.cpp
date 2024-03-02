#include "pch.h"
#include "Graph.h"
#include "queue"
using namespace std;

// =============================================================================
// Dijkstra ====================================================================
// =============================================================================

void Dijkstra(CGraph& graph, CVertex* pStart)
{
	for (CVertex& v : graph.m_Vertices) {

		v.m_DijkstraDistance = DBL_MAX;
		v.m_DijkstraVisit = false;
	}


	pStart->m_DijkstraDistance = 0;

	int vertexs_visitats = 0;
	int vertex_visitats_anteriors = -1;
	CVertex* pActual = pStart;

	while (vertexs_visitats > vertex_visitats_anteriors) {
		vertex_visitats_anteriors = vertexs_visitats;
		for (CEdge* e : pActual->m_Edges) {
			if (pActual->m_DijkstraDistance + e->m_Length < e->m_pDestination->m_DijkstraDistance) {
				e->m_pDestination->m_DijkstraDistance = pActual->m_DijkstraDistance + e->m_Length;
			}
		}
		pActual->m_DijkstraVisit = true;

		double valor_min = DBL_MAX;
		vertexs_visitats = 0;
		for (CVertex& v : graph.m_Vertices) {
			if (v.m_DijkstraVisit) vertexs_visitats++;
			else if (v.m_DijkstraDistance < valor_min)
			{
				valor_min = v.m_DijkstraDistance;
				pActual = &v;
			}
		}
	}
}

// =============================================================================
// DijkstraQueue ===============================================================
// =============================================================================

void DijkstraQueue(CGraph& graph, CVertex* pStart)
{
	struct element_cua {
		CVertex* vertex;
		double dist_temp;
	};

	struct comparator {
		bool operator()(element_cua pE1, element_cua pE2) {
			return pE1.dist_temp > pE2.dist_temp;
		}
	};
	priority_queue<element_cua, std::vector<element_cua>, comparator> queue;

	for (CVertex& v : graph.m_Vertices) {

		v.m_DijkstraDistance = DBL_MAX;
		v.m_DijkstraVisit = false;
	}
	pStart->m_DijkstraDistance = 0;
	element_cua element = { pStart ,pStart->m_DijkstraDistance };
	queue.push(element);

	while (!queue.empty())
	{
		element = queue.top();
		queue.pop();
		if (element.vertex->m_DijkstraVisit) continue;
		for (CEdge* e : element.vertex->m_Edges) {
			if (!e->m_pDestination->m_DijkstraVisit && element.vertex->m_DijkstraDistance + e->m_Length < e->m_pDestination->m_DijkstraDistance) {
				e->m_pDestination->m_DijkstraDistance = element.vertex->m_DijkstraDistance + e->m_Length;
				element_cua element_aux = { e->m_pDestination , e->m_pDestination->m_DijkstraDistance };
				queue.push(element_aux);
			}
		}
		element.vertex->m_DijkstraVisit = true;
	}
}
