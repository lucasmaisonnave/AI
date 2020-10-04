#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
using namespace olc;


#define MORPION_SIZE 3
#define X 1
#define O -1
#define WHO_PLAYS_FIRST 0 //1 : AI, 0 : YOU
#define INF 10000

#define COUCHE_MAX 5

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

class Morpion : public PixelGameEngine
{
	struct Action_Value
	{
		v2d_generic<int>* action = new v2d_generic<int>;
		int value;
	};
	struct sEtat
	{
		int tab[MORPION_SIZE * MORPION_SIZE];	//Etat du plateau X O ou 0
		int nb_plein = 0;						//Nombre de cases pleines
	};


	struct sEtat* Etat = nullptr;	// -1 : O ; 0 : vide ; 1 : X
	bool AI_PLAY = WHO_PLAYS_FIRST;
	bool end = false;
public:
	Morpion()
	{
		sAppName = "AI TicTacToe";
	}
	virtual bool OnUserCreate() override
	{
		Etat = new struct sEtat;
		for (int x = 0; x < MORPION_SIZE; x++)
			for (int y = 0; y < MORPION_SIZE; y++)
				Etat->tab[y * MORPION_SIZE + x] = 0;
		return true;
	}

	int nb_O_X(int n, struct sEtat* etat, int XO)	//Retourne le nombre de Colonne ou ligne ou diagonale avec n X (ou O) et seulement des X (ou O)
	{
		int nb_X = 0;
		int cmpt = 0;
		/*Lignes*/
		for (int x = 0; x < MORPION_SIZE; x++)
		{
			bool O_pres = false;
			for (int y = 0; y < MORPION_SIZE; y++)
			{
				if (etat->tab[y * MORPION_SIZE + x] == -XO)
					O_pres = true;
				else if (etat->tab[y * MORPION_SIZE + x] == XO)
					cmpt++;
			}
			nb_X += cmpt == n && !O_pres;
			cmpt = 0;
		}
		/*Colonnes*/
		for (int y = 0; y < MORPION_SIZE; y++)
		{
			bool O_pres = false;
			for (int x = 0; x < MORPION_SIZE; x++)
			{
				if (etat->tab[y * MORPION_SIZE + x] == -XO)
					O_pres = true;
				else if (etat->tab[y * MORPION_SIZE + x] == XO)
					cmpt++;
			}
			nb_X += cmpt == n && !O_pres;
			cmpt = 0;
		}
		/*Diagonales*/
		bool O_pres = false;
		for (int i = 0; i < MORPION_SIZE; i++)
		{
			if (etat->tab[i * MORPION_SIZE + i] == -XO)
				O_pres = true;
			else if (etat->tab[i * MORPION_SIZE + i] == XO)
				cmpt++;
		}
		nb_X += cmpt == n && !O_pres;
		cmpt = 0;
		O_pres = false;
		int y = 0;
		for (int x = MORPION_SIZE - 1; x >=0; x--)
		{
			if (etat->tab[y * MORPION_SIZE + x] == -XO)
				O_pres = true;
			else if (etat->tab[y * MORPION_SIZE + x] == XO)
				cmpt++;
			y++;
		}
		nb_X += cmpt == n && !O_pres;
		return nb_X;
	}

	int Eval(struct sEtat* etat)
	{
		int eval = 0;
		for (int i = 1; i < MORPION_SIZE; i++)
			eval += (2 * (i - 1) + 1) * (nb_O_X(i, etat, X) - nb_O_X(i, etat, O));
		if (nb_O_X(MORPION_SIZE, etat, X) >= 1)
			eval += INF;
		else if (nb_O_X(MORPION_SIZE, etat, O) >= 1)
			eval -= INF;
		return eval;
	}

	bool Test_Arret(struct sEtat* etat, int d)
	{
		return d >= COUCHE_MAX || nb_O_X(MORPION_SIZE, etat, X) >= 1 || nb_O_X(MORPION_SIZE, etat, O) >= 1 || etat->nb_plein == MORPION_SIZE * MORPION_SIZE;
	}

	std::vector<v2d_generic<int>*> Actions(struct sEtat* etat)	//Retourne les actions possibles sur état
	{
		std::vector<v2d_generic<int>*> actions;
		for (int x = 0; x < MORPION_SIZE; x++)
		{
			for (int y = 0; y < MORPION_SIZE; y++)
			{
				if (etat->tab[y * MORPION_SIZE + x] == 0)
				{
					v2d_generic<int>* action = new v2d_generic<int>;
					action->x = x;
					action->y = y;
					actions.push_back(action);
				}
			}
		}
		return actions;
	}

	struct sEtat* Result(struct sEtat* etat, v2d_generic<int>* action, const int XO)
	{
		struct sEtat* next_etat = new struct sEtat;
		for (int pos = 0; pos < MORPION_SIZE * MORPION_SIZE; pos++)
			next_etat->tab[pos] = etat->tab[pos];
		next_etat->tab[action->y * MORPION_SIZE + action->x] = XO;
		next_etat->nb_plein = etat->nb_plein + 1;
		return next_etat;
	}

	struct Action_Value* Valeur_Max(struct sEtat* etat, int alpha, int beta, int d)
	{
		if (Test_Arret(etat, d))
		{ 
			struct Action_Value* act_val = new struct Action_Value;
			act_val->action->x = -1;
			act_val->action->y = -1;
			act_val->value = Eval(etat);
			return act_val;
		}
			
		int v = -INF;
		v2d_generic<int>* next_action = nullptr;
		std::vector<v2d_generic<int>*> actions = Actions(etat);
		for (auto action : actions)
		{
			int last_v = v;
			v = std::fmax(v, Valeur_Min(Result(etat, action, X), alpha, beta, d + 1)->value);
			if (v != last_v)
				next_action = action;
			if (v >= beta)
			{
				struct Action_Value* act_val = new struct Action_Value;
				act_val->action->x = next_action->x;
				act_val->action->y = next_action->y;
				act_val->value = v;
				return act_val;
			}
			alpha = std::fmax(alpha, v);
		}
		struct Action_Value* act_val = new struct Action_Value;
		if (next_action)
		{
			act_val->action->x = next_action->x;
			act_val->action->y = next_action->y;
			act_val->value = v;
		}
		return act_val;
	}

	struct Action_Value* Valeur_Min(struct sEtat* etat, int alpha, int beta, int d)
	{
		if (Test_Arret(etat, d))
		{
			struct Action_Value* act_val = new struct Action_Value;
			act_val->action->x = -1;
			act_val->action->y = -1;
			act_val->value = Eval(etat);
			return act_val;
		}

		int v = INF;
		v2d_generic<int>* next_action = nullptr;
		std::vector<v2d_generic<int>*> actions = Actions(etat);
		for (auto action : actions)
		{
			int last_v = v;
			v = std::fmin(v, Valeur_Max(Result(etat, action, O), alpha, beta, d + 1)->value);
			if (v != last_v)
				next_action = action;
			if (v <= alpha)
			{
				struct Action_Value* act_val = new struct Action_Value;
				act_val->action->x = next_action->x;
				act_val->action->y = next_action->y;
				act_val->value = v;
				return act_val;
			}
			beta = std::fmin(beta, v);
		}
		struct Action_Value* act_val = new struct Action_Value;
		if (next_action)
		{
			act_val->action->x = next_action->x;
			act_val->action->y = next_action->y;
			act_val->value = v;
		}
		return act_val;
	}

	v2d_generic<int>* AI_Play(struct sEtat* etat)	//Retourne l'action à effectuer
	{
		return Valeur_Max(etat, -INF, INF, 0)->action;
	}

	virtual bool OnUserUpdate(float fElapsedtime) override
	{
		if (AI_PLAY && !end)
		{
			Etat = Result(Etat, AI_Play(Etat), X);
			AI_PLAY = false;
		}

		int nsquare_size = 15;
		int x0 = ScreenWidth() / 2 - (int)(nsquare_size * (MORPION_SIZE / 2.0f));
		int y0 = ScreenHeight() / 2 - (int)(nsquare_size * (MORPION_SIZE / 2.0f));

		struct Rect Plateau;
		Plateau.x = x0; Plateau.y = y0;	Plateau.w = MORPION_SIZE * nsquare_size; Plateau.h = MORPION_SIZE * nsquare_size;
		v2d_generic<int> mousePos;
		mousePos.x = GetMouseX(); mousePos.y = GetMouseY();

		FillRect(0, 0, ScreenWidth(), ScreenHeight(), WHITE);

		if (GetMouse(0).bPressed && PointInRect(&mousePos, &Plateau))
		{
			int x = (mousePos.x - x0) / nsquare_size;
			int y = (mousePos.y - y0) / nsquare_size;
			if (Etat->tab[y * MORPION_SIZE + x] == 0)
			{
				Etat->tab[y * MORPION_SIZE + x] = O;
				Etat->nb_plein++;
				AI_PLAY = !AI_PLAY;
			}			
		}
		if (GetMouse(1).bPressed)
		{
			Etat->nb_plein = 0;
			for (int x = 0; x < MORPION_SIZE; x++)
				for (int y = 0; y < MORPION_SIZE; y++)
					Etat->tab[y * MORPION_SIZE + x] = 0;
			AI_PLAY = WHO_PLAYS_FIRST;
		}


		//Draw X : red, O : blue
		for (int x = 0; x < MORPION_SIZE; x++)
		{
			for (int y = 0; y < MORPION_SIZE; y++)
			{
				if (Etat->tab[y * MORPION_SIZE + x] == O)
					FillRect(x0 + x * nsquare_size, y0 + y * nsquare_size, nsquare_size, nsquare_size, BLUE);
				else if (Etat->tab[y * MORPION_SIZE + x] == X)
					FillRect(x0 + x * nsquare_size, y0 + y * nsquare_size, nsquare_size, nsquare_size, RED);
			}
		}

		//Draw lines
		for(int x = 0; x <= MORPION_SIZE; x++)
			DrawLine(x0 + x * nsquare_size, y0, x0 + x * nsquare_size, y0 + MORPION_SIZE * nsquare_size, BLACK);
		for (int y = 0; y <= MORPION_SIZE; y++)
			DrawLine(x0 , y0 + y*nsquare_size, x0 + MORPION_SIZE * nsquare_size, y0 + y * nsquare_size, BLACK);

		//Test victoire ou defaite
		end = true;
		if (nb_O_X(MORPION_SIZE, Etat, X))
			DrawString(x0 + (int)((MORPION_SIZE/2.0f)*nsquare_size - 15), y0 - 10, "Lose", BLACK);
		else if (nb_O_X(MORPION_SIZE, Etat, O))
			DrawString(x0 + (int)((MORPION_SIZE / 2.0f) * nsquare_size - 15), y0 - 10, "Win!", BLACK);
		else if (Etat->nb_plein == MORPION_SIZE * MORPION_SIZE)
			DrawString(x0 + (int)((MORPION_SIZE / 2.0f) * nsquare_size - 15), y0 - 10, "Draw", BLACK);
		else
			end = false;

		return true;
	}

	void printEtat(struct sEtat* etat)
	{
		std::cout << std::endl;
		for (int y = 0; y < MORPION_SIZE; y++)
		{
			std::cout << "[";
			for (int x = 0; x < MORPION_SIZE; x++)
			{
				std::cout << etat->tab[y * MORPION_SIZE + x];
				if (x < MORPION_SIZE - 1)
					std::cout << ",";
			}
			std::cout << "]" << std::endl;
		}
		std::cout << std::endl;
	}
};

int main()
{
	Morpion game;
	game.Construct(140, 90, 8, 8);
	game.Start();
}
