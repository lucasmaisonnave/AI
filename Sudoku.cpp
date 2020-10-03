// Sudoku.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <queue>
using namespace olc;
using namespace std;

struct Rect
{
	int x;
	int y;
	int w;
	int h;
};

bool PointInRect(v2d_generic<int>* p, struct Rect* r)
{
	return ((p->x >= r->x) && (p->x < (r->x + r->w)) &&
		(p->y >= r->y) && (p->y < (r->y + r->h))) ? true : false;
}


class Sudoku : public PixelGameEngine
{
public:
	Sudoku()
	{
		sAppName = "AI Sudoku";
	}

	struct sVar
	{
		int value = 0;	//Not define yet
		std::vector<struct sVar*> voisins;
		int ind;	//Indice dans le tableau
	};
	struct sVar* X[81];

	std::list<int> D[81];


	virtual bool OnUserCreate() override
	{
		//Init D
		for (int i = 0; i < 81; i++)
			for (int j = 1; j <= 9; j++)
				D[i].push_back(j);
		//Here you can choose your sudoku, this algorithm is able to solve easy sudoku
		int init[81] = {3,8,6, 0,0,0, 0,5,4,
						9,0,0, 0,0,6, 0,0,0,
						0,0,0, 0,9,4, 0,1,3,

						7,0,1, 6,4,0, 0,0,0,
						0,0,0, 5,0,0, 0,4,0,
						4,0,3, 0,0,0, 7,8,6,

						6,4,7, 9,0,2, 5,3,8,
						0,3,0, 0,0,5, 0,9,0,
						5,0,0, 7,0,0, 4,0,0 };
		for (int k = 0; k < 81; k++)
		{
			if (init[k] != 0)
			{
				D[k].clear();
				D[k].push_back(init[k]);
			}			
		}
		//Init X
		for (int x = 0; x < 9; x++)
			for (int y = 0; y < 9; y++)
			{
				X[y * 9 + x] = new struct sVar;
				X[y * 9 + x]->ind = y * 9 + x;
			}
		

		for (int x = 0; x < 9; x++)
			for (int y = 0; y < 9; y++)
			{
				//ligne
				for (int i = 0; i < 9; i++)
					if(x != i)
						X[y * 9 + x]->voisins.push_back(X[y * 9 + i]);
				//Colonne
				for (int j = 0; j < 9; j++)
					if (y != j)
						X[y * 9 + x]->voisins.push_back(X[j * 9 + x]);
				//Carré
				int X_c = x / 3;	//Coordonnées du carré
				int Y_c = y / 3;
				for (int i = 0; i < 3; i++)
					for (int j = 0; j < 3; j++)
						if (X_c * 3 + i != x && Y_c * 3 + j != y)
							X[y * 9 + x]->voisins.push_back(X[(Y_c * 3 + j) * 9 + X_c * 3 + i]);
			}
		

		return true;
	}

	bool AC_3(struct sVar** X, std::list<int>* D)
	{
		queue<pair<struct sVar*, struct sVar*>> m_queue;
		//Init stack avec tous les arcs du CSP
		//Ici je considère qu'un arc n'est pas commutatif (Xi, Xj) != (Xj, Xi)
		for(int k = 0; k < 81; k++)
			for(auto voisin : X[k]->voisins)
				m_queue.push(make_pair(X[k], voisin));

		while (m_queue.size() != 0)
		{
			pair<struct sVar*, struct sVar*> arc = m_queue.front();
			m_queue.pop();
			if (Reviser(D, arc))
			{
				if (D[arc.first->ind].size() == 0)
					return false;
				for (auto voisin : arc.first->voisins)
					if (voisin != arc.second)
						m_queue.push(make_pair(voisin, arc.first));
			}
		}
		return true;
	}
	bool Reviser(std::list<int>* D, pair<struct sVar*, struct sVar*> arc)
	{
		bool modifie = false;
		std::list<int> D_i = D[arc.first->ind];
		while (D_i.size() != 0)
		{
			int x = D_i.front();
			D_i.pop_front();
			std::list<int> D_j = D[arc.second->ind];
			bool arc_coherent = false;
			while (D_j.size() != 0)
			{
				int y = D_j.front();
				D_j.pop_front();
				/*if (arc.first->ind == 6 && arc.second->ind == 13)
					std::cout << std::endl;*/
				if (x != y)
					arc_coherent = true;
			}
			if (!arc_coherent)
			{
				D[arc.first->ind].remove(x); 
				modifie = true;
			}
		}
		return modifie;
	}

	

	virtual bool OnUserUpdate(float ElapsedTime) override
	{
		int nsquare_size = 25;
		int x0 = ScreenWidth() / 2 - (int)(nsquare_size * (9 / 2.0f));
		int y0 = ScreenHeight() / 2 - (int)(nsquare_size * (9 / 2.0f));

		struct Rect Plateau;
		Plateau.x = x0; Plateau.y = y0;	Plateau.w = 9 * nsquare_size; Plateau.h = 9 * nsquare_size;
		v2d_generic<int> mousePos;
		mousePos.x = GetMouseX(); mousePos.y = GetMouseY();

		if (GetMouse(0).bPressed && PointInRect(&mousePos, &Plateau))
		{
			int x = (mousePos.x - x0) / nsquare_size;
			int y = (mousePos.y - y0) / nsquare_size;
			if (X[y * 9 + x]->value < 9)
				X[y * 9 + x]->value++;
			else
				X[y * 9 + x]->value -= 9;
			D[y * 9 + x].clear();
			if(X[y * 9 + x]->value != 0)
				D[y * 9 + x].push_back(X[y * 9 + x]->value);
			else
				for (int j = 1; j <= 9; j++)
					D[y * 9 + x].push_back(j);
		}
		if (GetMouse(1).bPressed)//Reset
		{
			for (int k = 0; k < 9 * 9; k++)
			{
				X[k]->value = 0;
				D[k].clear();
				for (int j = 1; j <= 9; j++)
					D[k].push_back(j);
			}
				
		}
		if (GetKey(SPACE).bPressed)
		{
			if (!AC_3(X, D))
				std::cout << "incoherence detecte" << std::endl;
		}
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), WHITE);

		//Display numbers
		for(int x = 0; x < 9; x++)
			for (int y = 0; y < 9; y++)
				if(D[y * 9 + x].size() == 1)
				{
					X[y * 9 + x]->value = (D[y * 9 + x]).front();
					DrawString(x0 + x * nsquare_size + (int)(nsquare_size/2.0f) - 2, y0 + y * nsquare_size + (int)(nsquare_size / 2.0f) - 2, std::to_string(X[y * 9 + x]->value),BLACK);
				}
					
		

		//Draw Lines
		for (int x = 0; x <= 9; x++)
			DrawLine(x0 + x * nsquare_size, y0, x0 + x * nsquare_size, y0 + 9 * nsquare_size,x % 3 == 0 ? BLACK : GREY);
		for (int y = 0; y <= 9; y++)
			DrawLine(x0, y0 + y * nsquare_size, x0 + 9 * nsquare_size, y0 + y * nsquare_size,y % 3 == 0 ? BLACK : GREY);
		
		return true;
	}
};

int main()
{
	Sudoku game;
	game.Construct(400, 325, 2, 2);
	game.Start();
}

