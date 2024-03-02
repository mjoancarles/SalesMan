#include "pch.h"
#include "Graph.h"
#include "GraphApplicationDlg.h"
#include <set>



double PosicioRespeteRecta(CGPoint& a, CGPoint& b, CGPoint& c) {
	return (a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y);
}
double AreaTriangle(CGPoint& a, CGPoint& b, CGPoint c) {
	return abs((a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y)) / 2.0;
}

// =============================================================================
// CONVEX HULL =================================================================
// =============================================================================
list<CVertex*> ConvexHull(CVertex* A, CVertex* B,
	list<CVertex*> punts)
{
	list<CVertex*> punts_aux = { A,B };
	if (punts.empty()) return punts_aux;
	CVertex* C = NULL;
	float vmax = 0;
	float valor;
	for (CVertex* v : punts) {
		valor = AreaTriangle(A->m_Point, B->m_Point, v->m_Point);
		if (valor > vmax) {
			vmax = valor;
			C = v;
		}
	}
	list<CVertex*> AC; // Noms orientatius, primera i segona divisió que pot ser adalt i a baix
	list<CVertex*> CB;
	//Separar punts en les dues llistes
	for (CVertex* p : punts) {
		if (PosicioRespeteRecta(A->m_Point, C->m_Point, p->m_Point) > 0) {
			AC.push_front(p);
		}
		else if (PosicioRespeteRecta(C->m_Point, B->m_Point, p->m_Point) > 0) {
			CB.push_front(p);
		}
	}

	list<CVertex*> CHLeft = ConvexHull(A, C, AC);
	list<CVertex*> CHRight = ConvexHull(C, B, CB);

	CHLeft.pop_back();
	CHLeft.splice(CHLeft.end(), CHRight, CHRight.begin(), CHRight.end());
	return CHLeft;
}
// QuickHull ===================================================================

CConvexHull QuickHull(CGraph& graph)
{
	CConvexHull convex_hull(&graph);
	if (graph.m_Vertices.empty()) return convex_hull;

	list<CVertex*> punts;
	CVertex* A = &graph.m_Vertices.front();
	CVertex* B = &graph.m_Vertices.back();

	if (A->m_Point.m_X != B->m_Point.m_X) {
		for (CVertex& v : graph.m_Vertices) {
			if (v.m_Point.m_X < A->m_Point.m_X) A = &v;
			else if (v.m_Point.m_X > B->m_Point.m_X) B = &v;
		}
	}
	else {
		for (CVertex& v : graph.m_Vertices) {
			if (v.m_Point.m_Y < A->m_Point.m_Y) A = &v;
			else if (v.m_Point.m_Y > B->m_Point.m_Y) B = &v;
		}
	}

	if (A->m_Point.m_X == B->m_Point.m_X && A->m_Point.m_Y == B->m_Point.m_Y) {
		convex_hull.m_Vertices = { A };
		return convex_hull;
	}

	list<CVertex*> left; // Noms orientatius, primera i segona divisió que pot ser adalt i a baix
	list<CVertex*> right;
	float valor;
	for (CVertex& v : graph.m_Vertices) {
		valor = PosicioRespeteRecta(A->m_Point, B->m_Point, v.m_Point);
		if (valor > 0) {
			left.push_front(&v);
		}
		else if (valor < 0) {
			right.push_front(&v);
		}
	}

	list<CVertex*> CHDown = ConvexHull(A, B, left);
	list<CVertex*> CHUp = ConvexHull(B, A, right);

	CHUp.pop_back();
	CHUp.pop_front();

	CHDown.splice(CHDown.end(), CHUp, CHUp.begin(), CHUp.end());
	convex_hull.m_Vertices = CHDown;
	return convex_hull;
}