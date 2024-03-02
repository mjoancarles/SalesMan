#include "pch.h"
#include "Graph.h"
#include "MAtrix.h"
#include <set>
#include <queue>



// =============================================================================
// SalesmanTrackBacktracking ===================================================
// =============================================================================
struct NodeCami {
	NodeCami* m_pAnterior;
	CEdge* m_pEdge;
};

/*
list<CEdge*> cami_optim;
list<CEdge*> cami;
*/
CTrack track(NULL);
double dist_max;
CVertex* desti;
list<CVertex*> visites;

void SalesmanTrackBacktrackingRecursiu(NodeCami* pAnterior, CVertex* pActual, double dist_actual)
{
	if (dist_actual > dist_max) { // Comprovem que la longitud del camí actual no sigui més gran que la millor solució
		return;
	}
	if (pActual == desti) {
		//Mirem si hem visitat tots els vèrtex a visitar
		for (CVertex* v_aux : visites) {
			for (CEdge* e : v_aux->m_Edges) {
				if (e->m_Processed) {
					goto seguent;
				}// si trobem una aresta processada anem a la següent iteració
			}
			goto sortida; // Si sortim sense haver trobat cap aresta processada, seguim
		seguent:;
		}
		// En cas d'haver-ho fet actualitzem el camí òptim i sortim de la funció
		//cami_optim = cami;
		track.Clear();
		while (pAnterior) {
			track.m_Edges.push_front(pAnterior->m_pEdge);
			pAnterior = pAnterior->m_pAnterior;
		}
		dist_max = dist_actual;
		return;
	}
sortida:// Seguim trobant el camí
	NodeCami pAux;
	pAux.m_pAnterior = pAnterior;
	for (CEdge* e : pActual->m_Edges) {
		if (e->m_Processed) continue;
		e->m_Processed = true;
		pAux.m_pEdge = e;
		//cami.push_back(e);
		SalesmanTrackBacktrackingRecursiu(&pAux, e->m_pDestination, dist_actual + e->m_Length);
		e->m_Processed = false;
		//cami.pop_back();
	}
	return;
}


CTrack SalesmanTrackBacktracking(CGraph& graph, CVisits& visits)
{
	//cami.clear();
	track.Clear();
	track.SetGraph(&graph);
	visites = visits.m_Vertices;
	dist_max = numeric_limits<double>::max();
	for (CEdge& e : graph.m_Edges) e.m_Processed = false; //Inicialitar totes les arestes com no procesades
	CVertex* inici = visites.front();
	visites.pop_front();
	desti = visites.back();
	visites.pop_back();
	SalesmanTrackBacktrackingRecursiu(NULL, inici, 0);
	//CTrack track(&graph);
	//track.m_Edges = cami_optim;
	return track;
}

// =============================================================================
// SalesmanTrackBacktrackingGreedy =============================================
// =============================================================================
Matrix<double> matrix;
list<int> order_optim;
vector<bool> visitats;

struct Node {
	Node* m_pAnterior;
	int n_Actual;
};

void SalesmanTrackBacktrackingGreedyRecursiu(double dist_actual, Node* anterior)
{
	if (dist_actual > dist_max) return;
	if (anterior->n_Actual == visitats.size() - 1) { // Comprovem haver arribat a una solució
		if (!equal(visitats.begin() + 1, visitats.end(), visitats.begin())) return; // Tots seran inicialment false menys el primer element
		dist_max = dist_actual;
		order_optim.clear();
		//Construim la solució
		while (anterior->m_pAnterior) {
			order_optim.push_front(anterior->n_Actual);
			anterior = anterior->m_pAnterior;
		}
		order_optim.push_front(0);
		return;
	}
	Node pAux = { anterior, 0 };
	for (int i = 0; i < visitats.size(); i++) {
		pAux.n_Actual = i;
		if (visitats[i]) continue;
		visitats[i] = true;
		SalesmanTrackBacktrackingGreedyRecursiu(dist_actual + matrix(anterior->n_Actual, i), &pAux);
		visitats[i] = false;
	}
	return;
}

CTrack SalesmanTrackBacktrackingGreedy(CGraph& graph, CVisits& visits)
{
	track.Clear();
	track.SetGraph(&graph);
	if (visits.m_Vertices.size() == 2) { // Tractem el cas trivial, executant un sol greedy i tornant el CTrack
		DijkstraGreedy(graph, visits.m_Vertices.front());
		CVertex* pAux = visits.m_Vertices.back();
		while (pAux->m_pDijkstraPrevious != NULL) {
			track.AddFirst(pAux->m_pDijkstraPrevious); // Afegim l'aresta amb la que hem arribat al vèrtex 
			pAux = pAux->m_pDijkstraPrevious->m_pOrigin; // Obtenim el vèrtex anterior
		}
		return track;
	}
	matrix.Resize(visits.m_Vertices.size(), visits.m_Vertices.size());
	dist_max = numeric_limits<double>::max();
	// Construim les matrius amb els valors de les distàncies
	int j, i = 0;
	for (CVertex* v : visits.m_Vertices) {
		DijkstraQueue(graph, v);
		j = 0;
		for (CVertex* v_aux : visits.m_Vertices) {
			matrix(i, j) = v_aux->m_DijkstraDistance;
			j++;
		}
		i++;
	}

	visitats.resize(visits.m_Vertices.size(), false);
	visitats[0] = true;
	Node anterior = { NULL, 0 }; // NULL perquè no apunta a cap altre node i -1 per indicar que no existeix el node que apuntem
	SalesmanTrackBacktrackingGreedyRecursiu(0.0, &anterior); // Fem la crida del backtracking

	// Un cop sabem l'ordre de les visites, executem els greedys per tal de formar els camins
	vector<CVertex*> visits_vector(visits.m_Vertices.begin(), visits.m_Vertices.end()); // vector de visites
	list<CEdge*> cami;
	CVertex* visita_anterior = visits_vector[order_optim.front()];
	order_optim.pop_front();
	for (int visita : order_optim) {
		DijkstraGreedy(graph, visita_anterior);
		CVertex* pAux = visits_vector[visita];
		while (pAux->m_pDijkstraPrevious != NULL) {
			cami.push_front(pAux->m_pDijkstraPrevious); // Afegim l'aresta amb la que hem arribat al vèrtex 
			pAux = pAux->m_pDijkstraPrevious->m_pOrigin; // Obtenim el vèrtex anterior
		}
		for (CEdge* e : cami) {
			track.AddLast(e);
		}
		cami.clear();
		visita_anterior = visits_vector[visita];
	}
	return track;
}
