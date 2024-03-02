#include "pch.h"
#include "Graph.h"
#include "queue"
using namespace std;


struct element_cua {
	CVertex* vertex;
	double dist_temp;
};

struct comparator {
	bool operator()(element_cua pE1, element_cua pE2) {
		return pE1.dist_temp > pE2.dist_temp;
	}
};

void DijkstraGreedy(CGraph& graph, CVertex* pStart) {

	priority_queue<element_cua, std::vector<element_cua>, comparator> queue;

	for (CVertex& v : graph.m_Vertices) {

		v.m_DijkstraDistance = std::numeric_limits<double>::max();
		v.m_DijkstraVisit = false;
	}

	pStart->m_DijkstraDistance = 0;
	pStart->m_pDijkstraPrevious = NULL;

	element_cua element = { pStart, pStart->m_DijkstraDistance };

	queue.push(element);

	while (!queue.empty())
	{
		element = queue.top();
		queue.pop();
		if (element.vertex->m_DijkstraVisit) continue;
		for (CEdge* e : element.vertex->m_Edges) {
			if (!e->m_pDestination->m_DijkstraVisit && element.vertex->m_DijkstraDistance + e->m_Length < e->m_pDestination->m_DijkstraDistance) {
				e->m_pDestination->m_DijkstraDistance = element.vertex->m_DijkstraDistance + e->m_Length;
				e->m_pDestination->m_pDijkstraPrevious = e; // Actualitzem l'aresta anterior
				element_cua element_aux = { e->m_pDestination , e->m_pDestination->m_DijkstraDistance };
				queue.push(element_aux);
			}
		}
		element.vertex->m_DijkstraVisit = true;
	}
}

// SalesmanTrackGreedy =========================================================

CTrack SalesmanTrackGreedy(CGraph& graph, CVisits& visits)
{
	CTrack cami(&graph);

	CVertex* pStart = visits.m_Vertices.front();
	for (CVertex* v : visits.m_Vertices) v->m_JaHePassat = false;
	list<CEdge*> recorregut;
	int i = 1;

	while (i < visits.m_Vertices.size())
	{
		pStart->m_JaHePassat = true;
		DijkstraGreedy(graph, pStart);
		double min = DBL_MAX;
		for (CVertex* v : visits.m_Vertices) {
			if (!v->m_JaHePassat && v->m_DijkstraDistance < min && v != visits.m_Vertices.back()) {
				min = v->m_DijkstraDistance;
				pStart = v;
			}
		}



		CVertex* pAux;
		pAux = pStart;
		while (pAux->m_pDijkstraPrevious != NULL) {
			recorregut.push_front(pAux->m_pDijkstraPrevious); // Afegim l'aresta amb la que hem arribat al vèrtex 
			pAux = pAux->m_pDijkstraPrevious->m_pOrigin; // Obtenim el vèrtex anterior
		}
		for (CEdge* e : recorregut) {
			cami.AddLast(e);
		}
		recorregut.clear();
		i++;
	}
	DijkstraGreedy(graph, pStart);

	pStart = visits.m_Vertices.back();
	while (pStart->m_pDijkstraPrevious != NULL) {
		recorregut.push_front(pStart->m_pDijkstraPrevious); // Afegim l'aresta amb la que hem arribat al vèrtex 
		pStart = pStart->m_pDijkstraPrevious->m_pOrigin; // Obtenim el vèrtex anterior
	}
	for (CEdge* e : recorregut) {
		cami.AddLast(e);
	}
	return cami;
}
