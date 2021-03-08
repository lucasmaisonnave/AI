#pragma once
#include "Chess.h"
#include <vector>
#define AI_SIDE NOIR

#define INF 10000

#define COUCHE_MAX 6
struct Action_Value
{
	Action action;
	int value;
};
class AI {
	int Eval(const Chess& etat)
	{
		int eval = etat.getNb_Piece(AI_SIDE, PION) + etat.getNb_Piece(AI_SIDE, CAVALIER) * 3 + etat.getNb_Piece(AI_SIDE, FOU) * 3 + etat.getNb_Piece(AI_SIDE, TOUR) * 5 + etat.getNb_Piece(AI_SIDE, DAME) * 9
			- (etat.getNb_Piece(!AI_SIDE, PION) + etat.getNb_Piece(!AI_SIDE, CAVALIER) * 3 + etat.getNb_Piece(!AI_SIDE, FOU) * 3 + etat.getNb_Piece(!AI_SIDE, TOUR) * 5 + etat.getNb_Piece(!AI_SIDE, DAME) * 9);
		return (etat.getNb_Piece(AI_SIDE, ROI) < 1 ? -INF : (etat.getNb_Piece(!AI_SIDE, ROI) < 1 ? INF : eval));
	}const

		bool Test_Arret(const Chess& etat, int d) const
	{
		return d >= COUCHE_MAX || etat.getNb_Piece(BLANC, ROI) < 1 || etat.getNb_Piece(NOIR, ROI) < 1;
	}
	void Action_Tour(std::vector<Action>& actions, Chess& etat, int i, int j) {
		//haut
		for (int l = j - 1; l >= 0; l--)
		{
			Action action = { i,j,i,l };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;	//Potentiellement source de conflit avec le break du switch
		}
		//bas
		for (int l = j + 1; l >= 0; l--)
		{
			Action action = { i,j,i,l };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;	//Potentiellement source de conflit avec le break du switch
		}
		//droit
		for (int c = i + 1; c < CHESS_SIZE; c++)
		{
			Action action = { i,j,c,j };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;	//Potentiellement source de conflit avec le break du switch
		}
		//gauche
		for (int c = i - 1; c >= 0; c--)
		{
			Action action = { i,j,c,j };
			if (etat.play(action, true))
				actions.push_back(action);
			else
				break;	//Potentiellement source de conflit avec le break du switch
		}
	}
	void Action_Fou(std::vector<Action>& actions, Chess& etat, int i, int j) {
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

	std::vector<Action> Actions(Chess& etat, int col)	//Retourne les actions possibles sur état
	{
		std::vector<Action> actions;
		for (int i = 0; i < CHESS_SIZE; i++)
			for (int j = 0; j < CHESS_SIZE; j++)
				if (etat.getCase(i, j).type != VIDE && etat.getCase(i, j).couleur == col)
				{
					int s = etat.getCase(i, j).couleur == NOIR ? -1 : 1;
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
						for (int l = i - 1; l <= i + 1; l += 2)
							for (int c = j - 2; c <= j + 2; c += 4)
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

	struct Action_Value Valeur_Max(Chess etat, int alpha, int beta, int d)
	{
		if (Test_Arret(etat, d))
		{
			struct Action_Value act_val;
			act_val.action = { -1,-1,-1,-1 };
			act_val.value = Eval(etat);
			return act_val;
		}

		int v = -INF;
		Action next_action;
		std::vector<Action> actions = Actions(etat, AI_SIDE);
		for (auto action : actions)
		{
			int last_v = v;
			v = std::fmax(v, Valeur_Min(Result(etat, action), alpha, beta, d + 1).value);
			if (v != last_v)
				next_action = action;
			if (v >= beta)
			{
				struct Action_Value act_val;
				act_val.action = next_action;
				act_val.value = v;
				return act_val;
			}
			alpha = std::fmax(alpha, v);
		}
		struct Action_Value act_val;
		if (&next_action)
		{
			act_val.action = next_action;
			act_val.value = v;
		}
		return act_val;
	}

	struct Action_Value Valeur_Min(Chess etat, int alpha, int beta, int d)
	{
		if (Test_Arret(etat, d))
		{
			struct Action_Value act_val;
			act_val.action = { -1,-1,-1,-1 };
			act_val.value = Eval(etat);
			return act_val;
		}

		int v = INF;
		Action next_action;
		std::vector<Action> actions = Actions(etat, 1 - AI_SIDE);
		for (auto action : actions)
		{
			int last_v = v;
			v = std::fmin(v, Valeur_Max(Result(etat, action), alpha, beta, d + 1).value);
			if (v != last_v)
				next_action = action;
			if (v <= alpha)
			{
				struct Action_Value act_val;
				act_val.action = next_action;
				act_val.value = v;
				return act_val;
			}
			beta = std::fmin(beta, v);
		}
		struct Action_Value act_val;
		if (&next_action)
		{
			act_val.action = next_action;
			act_val.value = v;
		}
		return act_val;
	}


	Chess Result(Chess& etat, const Action& action) const
	{
		Chess next_etat = etat;
		next_etat.play(action.c1, action.l1, action.c2, action.l2);
		return next_etat;
	}
public:
	Action AI_Play(Chess etat)	//Retourne l'action à effectuer
	{
		return Valeur_Max(etat, -INF, INF, 0).action;
	}
};