#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip>
#include "MAtrix.h"


struct element_cua {
	int vertex;
	double dist;
	element_cua* anterior;
	vector<bool> visitat;
	double longitud_cami;
	double cota_min;
	double cota_max;

	element_cua(int vertex, double dist, element_cua* anterior, int num_visites) { // Constructor exercici 1
		this->vertex = vertex;
		this->anterior = anterior;
		this->dist = dist;
		this->visitat = vector<bool>(num_visites, false);
	}
	element_cua(int vertex, double dist, element_cua* anterior, int num_visites, double cota_min, double cota_max) { // Constructor exercici 2
		this->vertex = vertex;
		this->anterior = anterior;
		this->dist = dist;
		this->visitat = vector<bool>(num_visites, false);
		this->cota_min = cota_min;
		this->cota_max = cota_max;
	}
	element_cua(int vertex, double dist, element_cua* anterior, int num_visites, double longitud_cami) { // Constructor exercici 3
		this->vertex = vertex;
		this->anterior = anterior;
		this->dist = dist;
		this->visitat = vector<bool>(num_visites, false);
		this->longitud_cami = longitud_cami;
	}
};

struct comparator {
	bool operator()(element_cua* pE1, element_cua* pE2) {
		return pE1->dist > pE2->dist;
	}
};

// SalesmanTrackBranchAndBound1 ===================================================

CTrack SalesmanTrackBranchAndBound1(CGraph& graph, CVisits& visits) {
	CTrack track_b(&graph);
	list<int> order;
	priority_queue<element_cua*, std::vector<element_cua*>, comparator> queue;
	int num_visites = visits.m_Vertices.size();
	if (num_visites == 2) {
		DijkstraGreedy(graph, visits.m_Vertices.front());
		CVertex* pAux = visits.m_Vertices.back();
		while (pAux->m_pDijkstraPrevious != NULL) {
			track_b.AddFirst(pAux->m_pDijkstraPrevious); // Afegim l'aresta amb la que hem arribat al vèrtex 
			pAux = pAux->m_pDijkstraPrevious->m_pOrigin; // Obtenim el vèrtex anterior
		}
		return track_b;
	}
	Matrix<double> matrix_b(num_visites, num_visites);
	Matrix<list<CEdge*>> matrix_t(num_visites, num_visites);
	// Construim les matrius amb els valors de les distàncies
	int j, i = 0;
	CVertex* pfinal = visits.m_Vertices.back();
	CVertex* pAux;
	for (CVertex* v : visits.m_Vertices) {
		if (i == num_visites - 1) continue; // No calculem el temps de l'última visita
		DijkstraGreedy(graph, v);
		j = 0;
		for (CVertex* v_aux : visits.m_Vertices) {
			//Omplim la matriu de distàncies
			matrix_b(i, j) = v_aux->m_DijkstraDistance;
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
	element_cua* element = new element_cua(0, 0.0, NULL, num_visites - 2);
	queue.push(element);
	//element->visitat[0] = true;
	while (!queue.empty())
	{
		element = queue.top();
		queue.pop();

		if (element->vertex == num_visites - 1) { // Si ha sortir de la cua visitant l'últim element
			while (element != NULL) {
				order.push_front(element->vertex);
				element = element->anterior;
			}
			break;
		}
		if (equal(element->visitat.begin() + 1, element->visitat.end(), element->visitat.begin()) && element->visitat[0]) { //Hem visitat tots menys el final
			//element_cua* aux = element;
			element_cua* aux = new element_cua(num_visites - 1, matrix_b(element->vertex, num_visites - 1) + element->dist, element, num_visites - 2);
			queue.push(aux);
		}
		else {
			for (int i = 0; i < num_visites - 2; i++) {
				if (element->visitat[i]) continue;
				element_cua* aux = new element_cua(i + 1, matrix_b(element->vertex, i + 1) + element->dist, element, num_visites - 2);
				aux->visitat = element->visitat;
				aux->visitat[i] = true;
				queue.push(aux);
			}
		}
	}

	//Construim els track amb la matriu de camins (arestes)
	int inici = order.front(); order.pop_front();
	while (inici != num_visites - 1) {
		int desti = order.front(); order.pop_front();
		for (CEdge* e : matrix_t(inici, desti)) {
			track_b.AddLast(e);
		}
		inici = desti;
	}

	return track_b;
}

// SalesmanTrackBranchAndBound2 ===================================================

CTrack SalesmanTrackBranchAndBound2(CGraph& graph, CVisits& visits) {

	CTrack track_b(&graph);
	int num_visites = visits.m_Vertices.size();
	if (num_visites == 2) {
		DijkstraGreedy(graph, visits.m_Vertices.front());
		CVertex* pAux = visits.m_Vertices.back();
		while (pAux->m_pDijkstraPrevious != NULL) {
			track_b.AddFirst(pAux->m_pDijkstraPrevious); // Afegim l'aresta amb la que hem arribat al vèrtex 
			pAux = pAux->m_pDijkstraPrevious->m_pOrigin; // Obtenim el vèrtex anterior
		}
		return track_b;
	}
	priority_queue<element_cua*, std::vector<element_cua*>, comparator> queue;
	Matrix<double> matrix_b(num_visites, num_visites);
	Matrix<list<CEdge*>> matrix_t(num_visites, num_visites);
	list<int> order;
	vector<double> cota_min(num_visites, numeric_limits<double>::max());
	vector<double> cota_max(num_visites, 0.0);
	// Construim les matrius amb els valors de les distàncies
	int j, i = 0; // "i" cada visita
	CVertex* pfinal = visits.m_Vertices.back();
	CVertex* pAux;
	for (CVertex* v : visits.m_Vertices) {
		if (i == num_visites - 1) continue; // No calculem el temps de l'última visita
		DijkstraGreedy(graph, v);
		j = 0; // "j" cada visita aux
		for (CVertex* v_aux : visits.m_Vertices) {
			if (i == j) {
				j++;
				continue;
			}
			if (v_aux->m_DijkstraDistance < cota_min[j] && j != 0) cota_min[j] = v_aux->m_DijkstraDistance; //Cota mínima d'arribar al node j, menys arribar al primer
			if (v_aux->m_DijkstraDistance > cota_max[j] && j != 0) cota_max[j] = v_aux->m_DijkstraDistance; //Cota màxima d'arribar al node j, menys arribar al primer
			//Omplim la matriu de distàncies
			matrix_b(i, j) = v_aux->m_DijkstraDistance;
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
	double v_cota_min = 0;
	double v_cota_max = 0;
	double cota_min_max = std::numeric_limits<double>::max();
	for (int j = 1; j < num_visites; j++) {
		v_cota_min += cota_min[j]; // Fem la cota mínima amb totes les visites menys les dels extrems (d'anar a la visita j+1)
		v_cota_max += cota_max[j];
	}

	element_cua* element = new element_cua(0, 0.0, NULL, num_visites - 2, v_cota_min, v_cota_max);
	queue.push(element);
	while (!queue.empty())
	{
		element = queue.top();
		queue.pop();

		if (element->vertex == num_visites - 1) { // Si ha sortir de la cua visitant l'últim element
			while (element != NULL) {
				order.push_front(element->vertex);
				element = element->anterior;
			}
			break;
		}

		if (equal(element->visitat.begin() + 1, element->visitat.end(), element->visitat.begin()) && element->visitat[0]) { //Hem visitat tots menys el final
			double longitud_cami = matrix_b(element->vertex, num_visites - 1);
			//Lower(a->c)=L(u)-min(c)+long(a->c) i per Upper iguals
			double nova_min = element->cota_min - cota_min[num_visites - 1] + longitud_cami;
			double nova_max = element->cota_max - cota_max[num_visites - 1] + longitud_cami;
			if (nova_max < cota_min_max) cota_min_max = nova_max + 1e-5;
			if (nova_min > cota_min_max) continue;
			element_cua* aux = new element_cua(num_visites - 1, nova_min, element, num_visites - 2, nova_min, nova_max);
			queue.push(aux);
		}
		else {

			for (int i = 0; i < num_visites - 2; i++) {
				if (element->visitat[i]) continue;
				double longitud_cami = matrix_b(element->vertex, i + 1);
				//Lower(a->c)=L(u)-min(c)+long(a->c) i per Upper igual
				double nova_min = element->cota_min - cota_min[i + 1] + longitud_cami;
				double nova_max = element->cota_max - cota_max[i + 1] + longitud_cami;
				if (nova_max < cota_min_max) cota_min_max = nova_max + 1e-5;
				if (nova_min > cota_min_max) continue;
				element_cua* aux = new element_cua(i + 1, nova_min, element, num_visites - 2, nova_min, nova_max);
				aux->visitat = element->visitat;
				aux->visitat[i] = true;
				queue.push(aux);
			}
		}
	}


	//Construim els track amb la matriu de camins (arestes)
	int inici = order.front(); order.pop_front();
	while (inici != num_visites - 1) {
		int desti = order.front(); order.pop_front();
		for (CEdge* e : matrix_t(inici, desti)) {
			track_b.AddLast(e);
		}
		inici = desti;
	}

	return track_b;
}

// SalesmanTrackBranchAndBound3 ===================================================


CTrack SalesmanTrackBranchAndBound3(CGraph& graph, CVisits& visits) {
	CTrack track_b(&graph);
	int num_visites = visits.m_Vertices.size();
	if (num_visites == 2) {
		DijkstraGreedy(graph, visits.m_Vertices.front());
		CVertex* pAux = visits.m_Vertices.back();
		while (pAux->m_pDijkstraPrevious != NULL) {
			track_b.AddFirst(pAux->m_pDijkstraPrevious); // Afegim l'aresta amb la que hem arribat al vèrtex 
			pAux = pAux->m_pDijkstraPrevious->m_pOrigin; // Obtenim el vèrtex anterior
		}
		return track_b;
	}
	priority_queue<element_cua*, std::vector<element_cua*>, comparator> queue;
	Matrix<double> matrix_b(num_visites, num_visites);
	Matrix<list<CEdge*>> matrix_t(num_visites, num_visites);
	list<int> order;
	// Construim les matrius amb els valors de les distàncies
	int j, i = 0; // "i" cada visita
	CVertex* pfinal = visits.m_Vertices.back();
	CVertex* pAux;
	for (CVertex* v : visits.m_Vertices) {
		if (i == num_visites - 1) continue; // No calculem el temps de l'última visita
		DijkstraGreedy(graph, v);
		j = 0; // "j" cada visita aux
		for (CVertex* v_aux : visits.m_Vertices) {
			if (i == j) {
				j++;
				continue;
			}
			//Omplim la matriu de distàncies
			matrix_b(i, j) = v_aux->m_DijkstraDistance;
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
	vector<double> cota_min(num_visites, numeric_limits<double>::max());
	vector<double> cota_max(num_visites, 0.0);
	for (int k = 0; k < num_visites - 1; k++) { // k d'on venim (node inici a final-1)
		for (int j = 1; j < num_visites; j++) { // j on anem (node inici+1 a final), desde j=1
			if (k == j) continue; // No podem anar on ja estem
			if (j == num_visites - 1 && 0 == k) continue; // No podem anar del node actual al node final (mo hem visitat tos els nodes encara)
			if (matrix_b(k, j) < cota_min[j]) cota_min[j] = matrix_b(k, j);
			if (matrix_b(k, j) > cota_max[j]) cota_max[j] = matrix_b(k, j);
		}
	}
	double v_cota_min = 0;
	double v_cota_max = 0;
	for (int j = 1; j < num_visites; j++) { //Cotes del node j on volem anar (inici+1 final)
		v_cota_min += cota_min[j]; // Fem la cota mínima amb totes les visites menys les dels extrems (d'anar a la visita j+1)
		v_cota_max += cota_max[j];
	}
	double cota_min_max = v_cota_max;
	element_cua* element = new element_cua(0, v_cota_min, NULL, num_visites - 2, 0.0);
	queue.push(element);
	while (!queue.empty())
	{
		element = queue.top();
		queue.pop();

		if (element->vertex == num_visites - 1) { // Si ha sortir de la cua visitant l'últim element
			while (element != NULL) {
				order.push_front(element->vertex);
				element = element->anterior;
			}
			break;
		}

		if (equal(element->visitat.begin() + 1, element->visitat.end(), element->visitat.begin()) && element->visitat[0]) { //Hem visitat tots menys el final
			double longitud_aresta = matrix_b(element->vertex, num_visites - 1);
			element_cua* aux = new element_cua(num_visites - 1, 0.0, element, num_visites - 2, element->longitud_cami + longitud_aresta);
			//Lower(a->c)=L(u)-min(c)+long(a->c) i per Upper iguals
			queue.push(aux);
		}
		else {
			//Càlcul cotes
			//if (element->vertex == 0) goto bucle;

			for (int i = 0; i < num_visites - 2; i++) { // i+1 on anem (node inici+1 a final)
				if (element->visitat[i]) continue; //si hem visitat on anirem
				cota_min.assign(num_visites, numeric_limits<double>::max());
				cota_max.assign(num_visites, 0.0);
				//Planetjem anar al vèrtex i+1
				double longitud_aresta = matrix_b(element->vertex, i + 1);
				element_cua* aux = new element_cua(i + 1, element->dist, element, num_visites - 2, element->longitud_cami + longitud_aresta);
				aux->visitat = element->visitat;
				aux->visitat[i] = true;
				if (equal(aux->visitat.begin() + 1, aux->visitat.end(), aux->visitat.begin())) { goto push; }
				for (int k = 1; k < num_visites - 1; k++) { // k d'on venim (node inici+1 a final-1)
					for (int j = 1; j < num_visites; j++) { // j on anem 
						if (k == j) continue; // No podem anar on ja estem
						if (aux->visitat[k - 1] && aux->vertex != k) continue; // Si d'on venim no està visitat o si d'on venim està visitat perquè és on estem ara
						if (j != num_visites - 1 && aux->visitat[j - 1]) continue; // No podem anar a un node ja visitat (excepte al final)
						if (j == num_visites - 1 && aux->vertex == k) continue; // No podem anar del node actual al node final (No hem visitat tos els nodes encara)

						if (matrix_b(k, j) < cota_min[j]) cota_min[j] = matrix_b(k, j);
						if (matrix_b(k, j) > cota_max[j]) cota_max[j] = matrix_b(k, j);
					}
				}
				v_cota_min = 0;
				v_cota_max = 0;
				for (int j = 1; j < num_visites; j++) { //Cotes del node j on volem anar (inici+1 final)
					if (j != num_visites - 1 && aux->visitat[j - 1])continue; //Si ja hem vistat el node j
					v_cota_min += cota_min[j]; // Fem la cota mínima amb totes les visites menys les dels extrems (d'anar a la visita j+1)
					v_cota_max += cota_max[j];
				}
				//Lower(a->c)=L(u)-min(c)+long(a->c) i per Upper igual
				double nova_min = v_cota_min + aux->longitud_cami;
				double nova_max = v_cota_max + aux->longitud_cami;
				if (nova_max < cota_min_max) cota_min_max = nova_max + 1e-5;
				if (nova_min > cota_min_max) continue;
				aux->dist = nova_min;
			push:;
				queue.push(aux);
			}
		}
	}

	//Construim els track amb la matriu de camins (arestes)
	int inici = order.front(); order.pop_front();
	while (inici != num_visites - 1) {
		int desti = order.front(); order.pop_front();
		for (CEdge* e : matrix_t(inici, desti)) {
			track_b.AddLast(e);
		}
		inici = desti;
	}

	return track_b;
}