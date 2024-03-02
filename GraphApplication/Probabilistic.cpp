#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <random>
#include <chrono>
#include "MAtrix.h"
// SalesmanTrackProbabilistic ==================================================

int myrandom (int i) { return std::rand()%i;}
vector<int> order; // millor ordre del intent actual
int num_visites;
double dist;
Matrix<double> matrix_dist;
vector<int> ConfiguracioAleatoria() {
	vector<int> order_auxiliar(num_visites -1);
	for (int i = 0; i < num_visites - 1; i++) order_auxiliar[i] = i;
	random_shuffle ( ++order_auxiliar.begin(), order_auxiliar.end(), myrandom);
	order_auxiliar.emplace_back(num_visites - 1);
	return order_auxiliar;
}

bool TrackProbabilistic() {
	vector<int> order_aux = order; // ordre amb la combinació auxiliar
	vector<int> order_base = order; // ordre inicial a partir del que trobem les combinacions
	double dist_aux, dist_ini = dist;
	for (int i = 1; i < num_visites - 2; i++) {
		for (int j = i + 1; j < num_visites - 1; j++) {
			order_aux = order_base;
			order_aux[j] = order_base[i];
			order_aux[i] = order_base[j];
			// TODO DISTANCIA VECTOR AUX
			dist_aux = 0;
			for (int k = 0; k < num_visites - 1; k++) dist_aux += matrix_dist(order_aux[k], order_aux[k + 1]);

			// TODO SI DIST_AUX < DIST GUARDEN NOU VECTOR A ORDER
			if (dist_aux < dist) {
				order = order_aux;
				dist = dist_aux;
			}
		}
	}
	if (dist == dist_ini) return false;
	else return true;
}

CTrack SalesmanTrackProbabilistic(CGraph& graph, CVisits& visits){
	num_visites = visits.m_Vertices.size();
	vector<int> order_optim(num_visites); // millor ordre de tots els intents
	srand (unsigned(time(0)));
	CTrack track_b(&graph);
	int j, i = 0;
	matrix_dist.Resize(num_visites, num_visites);
	Matrix<list<CEdge*>> matrix_t(num_visites, num_visites);

	CVertex* pAux;
	for (CVertex* v : visits.m_Vertices) {
		if (i == num_visites - 1) continue; // No calculem el temps de l'última visita
		DijkstraGreedy(graph, v);
		j = 0;
		for (CVertex* v_aux : visits.m_Vertices) {
			//Omplim la matriu de distàncies
			matrix_dist(i, j) = v_aux->m_DijkstraDistance;
			//Omplim la matriu de camins (arestes)
			pAux = v_aux;
			while (pAux->m_pDijkstraPrevious != NULL) {
				matrix_t(i, j).push_front(pAux->m_pDijkstraPrevious); // Afegim l'aresta amb la que hem arribat al vèrtex 
				pAux = pAux->m_pDijkstraPrevious->m_pOrigin; // Obtenim el vèrtex anterior
			}
			j++;
		}
		i++;
	}
	int intents = 100000;
	if (num_visites < 4){
		for (int i = 0; i < num_visites; i++) order_optim[i] = i;
		goto solucio;
	}
	double dist_optima = numeric_limits<double>::max();
	for(int i=0; i < intents; i++){
		dist = numeric_limits<double>::max();
		order = ConfiguracioAleatoria();
		while (TrackProbabilistic());
		if (dist <= dist_optima) {
			order_optim = order;
			dist_optima = dist;
		}
	}
solucio:;
	list<int> order_list;
	copy(order_optim.begin(), order_optim.end(), back_inserter(order_list));
	int inici = order_list.front(); order_list.pop_front();
	while (inici != num_visites - 1) {
		int desti = order_list.front(); order_list.pop_front();
		for (CEdge* e : matrix_t(inici, desti)) {
			track_b.AddLast(e);
		}
		inici = desti;
	}

	return track_b;
}
