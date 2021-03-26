#pragma once
#include "Chess.h"
#include <vector>


#define INF 100000
#define NB_COUPS 6
int COUCHE_MAX = 5;
int nb_coups = 0;
typedef struct Action_Value
{
	Action action;
	int value;
}Action_Value;

class AI {
private:
	int Eval(const Chess& etat)
	{
		//if(nb_coups < NB_COUPS)/*(int)(Actions(etat, AI_SIDE).size() - Actions(etat, !AI_SIDE).size()) + */
			return etat.getScoreMat(AI_SIDE) - etat.getScoreMat(!AI_SIDE);
		//return etat.getScoreMat(AI_SIDE) - etat.getScoreMat(!AI_SIDE);

	}

	bool Test_Arret(const Chess& etat, int d) const
	{
		return d >= COUCHE_MAX || etat.getNb_Piece(BLANC, ROI) == 0 || etat.getNb_Piece(NOIR, ROI) == 0;
	}
	void Action_Tour(vector<Action>& actions, Chess& etat, int i, int j) const {
		//haut
		for (int l = j - 1; l >= 0; l--)
		{
			Action action = { i,j,i,l };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;	
		}
		//bas
		for (int l = j + 1; l < CHESS_SIZE; l++)
		{
			Action action = { i,j,i,l };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;
		}
		//droit
		for (int c = i + 1; c < CHESS_SIZE; c++)
		{
			Action action = { i,j,c,j };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;
		}
		//gauche
		for (int c = i - 1; c >= 0; c--)
		{
			Action action = { i,j,c,j };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;
		}
	}
	void Action_Fou(vector<Action>& actions, Chess& etat, int i, int j) {
		//Diag gauche haut
		int l = j - 1;
		for (int c = i - 1; c >= 0 && l >= 0; c--, l--)
		{
			Action action = { i,j,c,l };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;
		}
		//Diag droit bas
		l = j + 1;
		for (int c = i + 1; c < CHESS_SIZE && l < CHESS_SIZE; c++, l++)
		{
			Action action = { i,j,c,l };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;
		}
		//Diag droit haut
		l = j - 1;
		for (int c = i + 1; c < CHESS_SIZE && l >= 0; c++, l--)
		{
			Action action = { i,j,c,l };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;
		}
		//Diag gauche bas
		l = j + 1;
		for (int c = i - 1; l < CHESS_SIZE && c >= 0; c--, l++)
		{
			Action action = { i,j,c,l };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;
		}
	}

	vector<Action> Actions(Chess etat, int col)	//Retourne les actions possibles sur �tat
	{
		vector<Action> actions;
		for (int i = 0; i < CHESS_SIZE; i++)
			for (int j = 0; j < CHESS_SIZE; j++)
				if (etat.getCase(i, j).type != VIDE && etat.getCase(i, j).couleur == col)
				{
					int s = col == NOIR ? -1 : 1;
					switch (etat.getCase(i, j).type) {
					case PION:
						//On avance de 1
						for (int c = i - 1; c <= i + 1; c++)
						{
							Action action = { i,j,c,j + s };
							if (etat.play(action, true))
								actions.push_back(action);
						}
						//On avance de 2
						if (true) {
							Action action = { i,j,i,j + 2 * s };
							if (etat.play(action, true))
								actions.push_back(action);
						}
						break;

					case FOU:
						Action_Fou(actions, etat, i, j);
						break;
					case TOUR:
						Action_Tour(actions, etat, i, j);
						break;
					case CAVALIER:
						for (int c = i - 1; c <= i + 1; c += 2)
							for (int l = j - 2; l <= j + 2; l += 4)
							{
								Action action = { i,j,c,l };
								if (etat.play(action, true))
									actions.push_back(action);
							}
						for (int l = j - 1; l <= j + 1; l += 2)
							for (int c = i - 2; c <= i + 2; c += 4)
							{
								Action action = { i,j,c,l };
								if (etat.play(action, true))
									actions.push_back(action);
							}
						break;
					case DAME:
						Action_Fou(actions, etat, i, j);
						Action_Tour(actions, etat, i, j);
						break;
					case ROI:
						//On bouge de 1
						for (int c = i - 1; c <= i + 1; c++)
						{
							for (int l = j - 1; l <= j + 1; l++)
							{
								if (c != i || l != j)
								{
									Action action = { i,j,c,l };
									if (etat.play(action, true))
										actions.push_back(action);
								}
							}
						}
						//On rock
						for (int c = i - 2; c <= i + 2; c += 4)
						{
							Action action = { i,j,c,j };
							if (etat.play(action, true))
								actions.push_back(action);
						}
						break;
					}
				}
		return actions;
	}

	Action_Value Valeur_Max(Chess etat, int alpha, int beta, int d)
	{
		if (Test_Arret(etat, d))
		{
			Action_Value act_val;
			act_val.action = { -1,-1,-1,-1 };
			act_val.value = Eval(etat);
			return act_val;
		}

		int v = -INF;
		Action next_action = { -1,-1,-1,-1 };
		vector<Action> actions = Actions(etat, AI_SIDE);
		for (auto action : actions)
		{
			int last_v = v;
			v = fmax(v, Valeur_Min(Result(etat, action), alpha, beta, d + 1).value);
			if (v != last_v)
				next_action = action;
			if (v >= beta)
			{
				Action_Value act_val;
				act_val.action = next_action;
				act_val.value = v;
				return act_val;
			}
			alpha = fmax(alpha, v);
		}
		Action_Value act_val;
		act_val.action = next_action;
		act_val.value = v;
		return act_val;
	}

	Action_Value Valeur_Min(Chess etat, int alpha, int beta, int d)
	{
		if (Test_Arret(etat, d))
		{
			Action_Value act_val;
			act_val.action = { -1,-1,-1,-1 };
			act_val.value = Eval(etat);
			return act_val;
		}

		int v = INF;
		Action next_action = { -1,-1,-1,-1 };
		vector<Action> actions = Actions(etat, !AI_SIDE);
		for (auto action : actions)
		{
			int last_v = v;
			v = fmin(v, Valeur_Max(Result(etat, action), alpha, beta, d + 1).value);
			if (v != last_v)
				next_action = action;
			if (v <= alpha)
			{
				Action_Value act_val;
				act_val.action = next_action;
				act_val.value = v;
				return act_val;
			}
			beta = fmin(beta, v);
		}
		Action_Value act_val;
		act_val.action = next_action;
		act_val.value = v;
		return act_val;
	}


	Chess Result(const Chess& etat, const Action& action) const
	{
		Chess next_etat = etat;
		next_etat.play(action);
		return next_etat;
	}
public:
	Action AI_Play(Chess etat)	//Retourne l'action � effectuer
	{
		return Valeur_Max(etat, -INF, INF, 0).action;
	}
};