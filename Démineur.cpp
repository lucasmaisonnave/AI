// Démineur.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <random>
using namespace olc;

#define DEM_SIZE 30
const int NB_BOMB = (int)DEM_SIZE * DEM_SIZE * 0.15;
#define AI 1


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

class Demineur : public PixelGameEngine
{
public:
	Demineur()
	{
		sAppName = "Demineur IA";
	}

	int nb_flags = 0;
	struct sCase
	{
		int nb_bomb_arround = 0;		//Nombre de bombes autour de cette case
		bool flag = false;				//True si on a mit un drapeau sur cette case
		bool bomb = false;				//True s'il y a une bombe dans cette case
		bool revealed = false;			//True si la case doit être révélé
	};

	struct sCase tdemineur[DEM_SIZE * DEM_SIZE];


	int* KparmiN(int x, int y)	//Return le nombre de bombes parmi le nombre de case non découverte autour de la case x, y
	{
		int ret[2]; //(k,n)
		int nb_unkown_bombe = tdemineur[y * DEM_SIZE + x].nb_bomb_arround;
		int nb_unrevealed = 8;

		for(int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
			{
				if (!(y == 0 && j == 0) && !(x == 0 && i == 0) && !(y == DEM_SIZE - 1 && j == 2) && !(x == DEM_SIZE - 1 && i == 2))
				{
					if (tdemineur[(y - 1 + j) * DEM_SIZE + x - 1 + i].flag && !(j == 1 && i == 1))
						nb_unkown_bombe--;
					if ((tdemineur[(y - 1 + j) * DEM_SIZE + x - 1 + i].revealed || tdemineur[(y - 1 + j) * DEM_SIZE + x - 1 + i].flag) && !(j == 1 && i == 1))
						nb_unrevealed--;
				}
				else
					nb_unrevealed--;
			}
		ret[0] = nb_unkown_bombe;
		ret[1] = nb_unrevealed;
		return ret;
	}

	bool UnrevealedNeighboors(int x, int y)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				if (!(y == 0 && j == 0) && !(x == 0 && i == 0) && !(y == DEM_SIZE - 1 && j == 2) && !(x == DEM_SIZE - 1 && i == 2))
					if (!tdemineur[(y - 1 + j) * DEM_SIZE + x - 1 + i].revealed && !tdemineur[(y - 1 + j) * DEM_SIZE + x - 1 + i].flag && !(j == 1 && i == 1))
						return true;
		return false;
	}

	bool OnUserCreate() override
	{
		//Assignation des bombes aléatoirement
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, DEM_SIZE*DEM_SIZE - 1);
		for (int i = 0; i < NB_BOMB; i++)
		{
			int new_bomb = dis(gen);
			while(tdemineur[new_bomb].bomb)
				new_bomb = dis(gen);
			tdemineur[new_bomb].bomb = true;
			tdemineur[new_bomb].nb_bomb_arround = -1;
		}

		//Calcul du nombre de bombes adjacentes
		for(int x = 0; x < DEM_SIZE; x++)
			for (int y = 0; y < DEM_SIZE; y++)
			{
				if (!tdemineur[y * DEM_SIZE + x].bomb)
				{
					if (x < DEM_SIZE - 1)
						tdemineur[y * DEM_SIZE + x].nb_bomb_arround += tdemineur[y * DEM_SIZE + x + 1].bomb;
					if (x > 0)
						tdemineur[y * DEM_SIZE + x].nb_bomb_arround += tdemineur[y * DEM_SIZE + x - 1].bomb;
					if (y > 0)
						tdemineur[y * DEM_SIZE + x].nb_bomb_arround += tdemineur[(y - 1) * DEM_SIZE + x].bomb;
					if (y < DEM_SIZE - 1)
						tdemineur[y * DEM_SIZE + x].nb_bomb_arround += tdemineur[(y + 1) * DEM_SIZE + x].bomb;
					if (x > 0 && y > 0)
						tdemineur[y * DEM_SIZE + x].nb_bomb_arround += tdemineur[(y - 1) * DEM_SIZE + x - 1].bomb;
					if (x > 0 && y < DEM_SIZE - 1)
						tdemineur[y * DEM_SIZE + x].nb_bomb_arround += tdemineur[(y + 1) * DEM_SIZE + x - 1].bomb;
					if (x < DEM_SIZE - 1 && y < DEM_SIZE - 1)
						tdemineur[y * DEM_SIZE + x].nb_bomb_arround += tdemineur[(y + 1) * DEM_SIZE + x + 1].bomb;
					if (x < DEM_SIZE - 1 && y > 0)
						tdemineur[y * DEM_SIZE + x].nb_bomb_arround += tdemineur[(y - 1) * DEM_SIZE + x + 1].bomb;
				}
				
			}
		//Révele une première zone vide
		int cmpt = 0;
		for (int x = 0; x < DEM_SIZE; x++)
			for (int y = 0; y < DEM_SIZE; y++)
				if (tdemineur[y * DEM_SIZE + x].nb_bomb_arround == 0 && !tdemineur[y * DEM_SIZE + x].revealed)
				{
					Reveal(x, y);
					cmpt++;
					if (cmpt == 2)
					{
						x = DEM_SIZE;
						y = DEM_SIZE;
					}
				}
		
		return true;
	}

	void Reveal(int x, int y)
	{
		tdemineur[y * DEM_SIZE + x].revealed = true;
		if (tdemineur[y * DEM_SIZE + x].flag)
		{
			tdemineur[y * DEM_SIZE + x].flag = false;
			nb_flags--;
		}
		
		if (tdemineur[y * DEM_SIZE + x].nb_bomb_arround == 0)
		{
			if (x < DEM_SIZE - 1)
				if(!tdemineur[y * DEM_SIZE + x + 1].revealed)
					Reveal(x + 1, y);
			if (x > 0)
				if (!tdemineur[y * DEM_SIZE + x - 1].revealed)
					Reveal(x - 1 , y);
			if (y > 0)
				if (!tdemineur[(y - 1) * DEM_SIZE + x].revealed)
					Reveal(x, y - 1);
			if (y < DEM_SIZE - 1)
				if (!tdemineur[(y + 1) * DEM_SIZE + x].revealed)
					Reveal(x, y + 1);
			if (x > 0 && y > 0)
				if (!tdemineur[(y - 1) * DEM_SIZE + x - 1].revealed)
					Reveal(x - 1, y - 1);
			if (x > 0 && y < DEM_SIZE - 1)
				if (!tdemineur[(y + 1) * DEM_SIZE + x - 1].revealed)
					Reveal(x - 1, y + 1);
			if (x < DEM_SIZE - 1 && y < DEM_SIZE - 1)
				if (!tdemineur[(y + 1) * DEM_SIZE + x + 1].revealed)
					Reveal(x + 1, y + 1);
			if (x < DEM_SIZE - 1 && y > 0)
				if (!tdemineur[(y - 1) * DEM_SIZE + x + 1].revealed)
					Reveal(x + 1, y - 1);
		}
	}

	void Flag(int x, int y)
	{
		tdemineur[y * DEM_SIZE + x].flag = !tdemineur[y * DEM_SIZE + x].flag;
		if (tdemineur[y * DEM_SIZE + x].flag && !tdemineur[y * DEM_SIZE + x].revealed)
			nb_flags++;
		else if (!tdemineur[y * DEM_SIZE + x].flag && !tdemineur[y * DEM_SIZE + x].revealed)
			nb_flags--;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		int nsquare_size = 20;
		int x0 = ScreenWidth() / 2 - (int)(nsquare_size * (DEM_SIZE / 2.0f));
		int y0 = ScreenHeight() / 2 - (int)(nsquare_size * (DEM_SIZE / 2.0f));

		struct Rect Plateau;
		Plateau.x = x0; Plateau.y = y0;	Plateau.w = DEM_SIZE * nsquare_size; Plateau.h = DEM_SIZE * nsquare_size;
		v2d_generic<int>  mousePos;
		mousePos.x = GetMouseX(); mousePos.y = GetMouseY();

		//Évenement
		if (!AI)
		{
			if (GetMouse(0).bPressed && PointInRect(&mousePos, &Plateau))
			{
				int x = (mousePos.x - x0) / nsquare_size;
				int y = (mousePos.y - y0) / nsquare_size;
				if (!tdemineur[y * DEM_SIZE + x].revealed)
					Reveal(x, y);
			}
			else if (GetMouse(1).bPressed && PointInRect(&mousePos, &Plateau))
			{
				int x = (mousePos.x - x0) / nsquare_size;
				int y = (mousePos.y - y0) / nsquare_size;
				Flag(x, y);
				
			}
		}
		else //Première version de l'IA qui ne cherche que les cas évident où il y a k bombes parmi k cases ou 0 parmi n
		{
			for(int x = 0; x < DEM_SIZE; x++)
				for (int y = 0; y < DEM_SIZE; y++)
				{
					if (tdemineur[y * DEM_SIZE + x].revealed && UnrevealedNeighboors(x, y))
					{
						int* KN = KparmiN(x, y);
						if (KN[0] == KN[1])	//Cas où il y k bomdes parmi k cases donc elles doivent être toutes flaged
						{
							for (int i = 0; i < 3; i++)
								for (int j = 0; j < 3; j++)
								{
									if (!(y == 0 && j == 0) && !(x == 0 && i == 0) && !(y == DEM_SIZE - 1 && j == 2) && !(x == DEM_SIZE - 1 && i == 2))
										if (!tdemineur[(y - 1 + j) * DEM_SIZE + x - 1 + i].flag && !tdemineur[(y - 1 + j) * DEM_SIZE + x - 1 + i].revealed && !(j == 1 && i == 1))
										{
											Flag(x - 1 + i, y - 1 + j);
										}
								}
									
						}
						else if (KN[0] == 0)
						{
							for (int i = 0; i < 3; i++)
								for (int j = 0; j < 3; j++)
									if (!(y == 0 && j == 0) && !(x == 0 && i == 0) && !(y == DEM_SIZE - 1 && j == 2) && !(x == DEM_SIZE - 1 && i == 2))
										if (!tdemineur[(y - 1 + j) * DEM_SIZE + x - 1 + i].revealed && !tdemineur[(y - 1 + j) * DEM_SIZE + x - 1 + i].flag && !(j == 1 && i == 1))
										{
											Reveal(x - 1 + i, y - 1 + j);
										}
						}
					}
				}
		}
		//Affichage
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), WHITE);
			
		for (int x = 0; x < DEM_SIZE; x++)
			for (int y = 0; y < DEM_SIZE; y++)
			{
				if (tdemineur[y * DEM_SIZE + x].revealed)
				{
					if (tdemineur[y * DEM_SIZE + x].bomb)
						FillRect(x0 + x * nsquare_size, y0 + y * nsquare_size, nsquare_size, nsquare_size, RED);
					else if (tdemineur[y * DEM_SIZE + x].nb_bomb_arround != 0)
						DrawString(x0 + nsquare_size / 3 + x * nsquare_size, y0 + nsquare_size / 3 + y * nsquare_size, std::to_string(tdemineur[y * DEM_SIZE + x].nb_bomb_arround), BLACK, 1);
				}
				else if (tdemineur[y * DEM_SIZE + x].flag)
					FillRect(x0 + x * nsquare_size, y0 + y * nsquare_size, nsquare_size, nsquare_size, YELLOW);
				else
					FillRect(x0 + x * nsquare_size, y0 + y * nsquare_size, nsquare_size, nsquare_size, BLUE);
			}
		//Draw Lines
		for (int x = 0; x <= DEM_SIZE; x++)
			DrawLine(x0 + x * nsquare_size, y0, x0 + x * nsquare_size, y0 + DEM_SIZE * nsquare_size, BLACK);
		for (int y = 0; y <= DEM_SIZE; y++)
			DrawLine(x0, y0 + y * nsquare_size, x0 + DEM_SIZE * nsquare_size, y0 + y * nsquare_size, BLACK);
		//Affichage du nombre de flags
		DrawString(x0, y0 - 20, "Flags : " + std::to_string(nb_flags) + "/" + std::to_string(NB_BOMB), BLACK, 2);

		return true;
	}
};

int main()
{
	Demineur game;
	game.Construct(1000, 700, 1, 1);
	game.Start();
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
