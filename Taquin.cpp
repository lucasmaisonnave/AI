// Taquin.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"

#include <ctime>

using namespace olc;
using namespace std;         
#include <random>
#define TAQUIN_SIZE 3


/*
	Here you can chose to create a random sliding puzzle or type your own puzzle in "init"
	You can also change the goal to reach in the variable "goal"
*/
#define TAQUIN_ALEATOIRE 1	//1 : random puzzle, 0 : you can chose

int goal[TAQUIN_SIZE* TAQUIN_SIZE] = {  1,2,3,
										4,5,6,
										7,8,0};



int init[TAQUIN_SIZE* TAQUIN_SIZE] = {	1,2,3,
										4,5,6,
										7,8,0};

class Taquin : public PixelGameEngine
{
public:
	Taquin()
	{
		sAppName = "Solver Taquin";
	}

	enum DIRECTION{HAUT, BAS, DROITE, GAUCHE, DIR_END};

	struct sNode {
		bool bVisited = false;			// Have we searched this node before?
		float fGlobalGoal;				// Distance to goal so far, use heristic
		float fLocalGoal;				// Distance to goal if we took the alternative route
		int nIndice_x0;	int nIndice_y0;				//	Indice du chiffre 0 dans le tableau
		int state[TAQUIN_SIZE * TAQUIN_SIZE];				//	State of the Node
		std::vector<sNode*> vecNeighbours;	// Connections to neighbours
		sNode* parent;					// Node connecting to this node that offers shortest parent
	};

	struct Path
	{
		std::vector<sNode*> path;
		int nbStep = 0;
	};

	std::vector<sNode*> nodes;

	sNode* nodeStart = nullptr;	
	sNode* nodeEnd = nullptr;
	Path chemin;
	int nCaseSize = 40;
	int currentNodePath = 0;

	bool Move(sNode* node, int Direction)
	{
		int x0 = node->nIndice_x0;
		int y0 = node->nIndice_y0;
		int c;
		if (Direction == HAUT && y0 > 0)
		{
			c = node->state[(y0 - 1) * TAQUIN_SIZE + x0];
			node->state[(y0 - 1) * TAQUIN_SIZE + x0] = 0;
			node->state[y0 * TAQUIN_SIZE + x0] = c;
			node->nIndice_y0--;
			return true;
		}
		if (Direction == BAS && y0 < TAQUIN_SIZE - 1)
		{
			c = node->state[(y0 + 1) * TAQUIN_SIZE + x0];
			node->state[(y0 + 1) * TAQUIN_SIZE + x0] = 0;
			node->state[y0 * TAQUIN_SIZE + x0] = c;
			node->nIndice_y0++;
			return true;
		}
		if (Direction == DROITE && x0 < TAQUIN_SIZE - 1)
		{
			c = node->state[y0* TAQUIN_SIZE + x0 + 1];
			node->state[y0 * TAQUIN_SIZE + x0 + 1] = 0;
			node->state[y0 * TAQUIN_SIZE + x0] = c;
			node->nIndice_x0++;
			return true;
		}
		if (Direction == GAUCHE && x0 > 0)
		{
			c = node->state[y0 * TAQUIN_SIZE + x0 - 1];
			node->state[y0 * TAQUIN_SIZE + x0 - 1] = 0;
			node->state[y0 * TAQUIN_SIZE + x0] = c;
			node->nIndice_x0--;
			return true;
		}
		return false;
	}


	void CreateStartNode()
	{
		nodeStart->bVisited = false;
		nodeStart->fGlobalGoal = INFINITY;
		nodeStart->fLocalGoal = INFINITY;
		nodeStart->parent = nullptr;
		nodeStart->vecNeighbours.clear();
		//Création de l'état de départ
		if (TAQUIN_ALEATOIRE)
		{
			int nbMove = 101;	//Nombre de mouvement aléatoire pour créer l'état de départ
			int cmpt = 0;		//Compteur de mouvements
			//Distribution uniforme
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(0, DIR_END - 1);
			//Movement aléatoire
			while (cmpt != nbMove)
				cmpt += Move(nodeStart, dis(gen));
		}
		else
		{
			for (int i = 0; i < 9; i++)
				nodeStart->state[i] = init[i];
			for(int x = 0; x < TAQUIN_SIZE; x++)
				for(int y = 0; y < TAQUIN_SIZE; y++)
					if (nodeStart->state[y * TAQUIN_SIZE + x] == 0)
					{
						nodeStart->nIndice_x0 = x;
						nodeStart->nIndice_y0 = y;
					}
			
		}
		nodes.push_back(nodeStart);
	}

	virtual bool OnUserCreate() override
	{
		nodeEnd = new sNode;
		nodeStart = new sNode;
		
		for (int i = 0; i < TAQUIN_SIZE*TAQUIN_SIZE; i++)
		{
			nodeEnd->state[i] = goal[i];
			nodeEnd->fGlobalGoal = INFINITY;
			nodeEnd->fLocalGoal = INFINITY;
			nodeEnd->parent = nullptr;

			nodeStart->state[i] = goal[i];
			nodeStart->fGlobalGoal = INFINITY;
			nodeStart->fLocalGoal = INFINITY;
			nodeStart->parent = nullptr;
		}
		for (int x = 0; x < TAQUIN_SIZE; x++)
			for (int y = 0; y < TAQUIN_SIZE; y++)
				if (nodeStart->state[y * TAQUIN_SIZE + x] == 0)
				{
					nodeStart->nIndice_x0 = x;
					nodeStart->nIndice_y0 = y;
					nodeEnd->nIndice_x0 = x;
					nodeEnd->nIndice_y0 = y;
				}
		chemin.path.push_back(nodeEnd);
		nodes.push_back(nodeStart);
		nodes.push_back(nodeEnd);
		//CreateStartNode();	

		return true;
	}

	void DrawNode(sNode* node, int x, int y)
	{
		for (int i = 0; i < TAQUIN_SIZE; i++)
		{
			for (int j = 0; j < TAQUIN_SIZE; j++)
			{
				FillRect(x + i * nCaseSize, y + j * nCaseSize,  nCaseSize, nCaseSize,BLACK);
				if(node->state[j * TAQUIN_SIZE + i] != 0)
				{ 
					DrawString(x + i * nCaseSize + nCaseSize / 2, y + j * nCaseSize + nCaseSize / 2, to_string(node->state[j * TAQUIN_SIZE + i]),WHITE);
				}
				
			}
		}
		//Draw borders
		for (int k = 0; k <= TAQUIN_SIZE; k++)
		{
			DrawLine(x + k * nCaseSize, y + 0 * nCaseSize, x + k *nCaseSize, y + TAQUIN_SIZE * nCaseSize, WHITE);
			DrawLine(x + 0 * nCaseSize, y + k * nCaseSize, x + TAQUIN_SIZE * nCaseSize, y + k * nCaseSize, WHITE);
		}
	}
	bool Same(sNode* a, sNode* b) // retourne true s'ils ont le même état
	{
		for (int x = 0; x < TAQUIN_SIZE; x++)
			for (int y = 0; y < TAQUIN_SIZE; y++)
				if (a->state[y * TAQUIN_SIZE + x] != b->state[y * TAQUIN_SIZE + x])
					return false;
		return true;
	}
	sNode* IsIn(sNode* gNode, std::vector<sNode*> Nodes) // retourne le pointeur correspondant si gNode est dans Nodes, en comparant state sinon nullptr
	{
		for (auto node : Nodes)
		{
			if (Same(gNode, node))
				return node;
		}
		return nullptr;
	};

	void Solve_Astar()
	{
		auto WrongPiece = [](sNode* a, sNode* b) // Compte le nombre de pièces mal placés
		{
			int cmpt = 0;
			for (int x = 0; x < TAQUIN_SIZE; x++)
				for (int y = 0; y < TAQUIN_SIZE; y++)
					cmpt += (a->state[y * TAQUIN_SIZE + x] != b->state[y * TAQUIN_SIZE + x]);
			return cmpt;
		};
		auto Manhattan = [](sNode* a, sNode* b) // Somme des distances pour chaque pièce à sa position but (horizontal + vertical)
		{
			int cmpt = 0;
			int _posGoal[TAQUIN_SIZE * TAQUIN_SIZE];
			for (int x = 0; x < TAQUIN_SIZE; x++)
				for (int y = 0; y < TAQUIN_SIZE; y++)
					_posGoal[b->state[y * TAQUIN_SIZE + x]] = y * TAQUIN_SIZE + x;
			for (int x = 0; x < TAQUIN_SIZE; x++)
			{
				for (int y = 0; y < TAQUIN_SIZE; y++)
				{
					int val = a->state[y * TAQUIN_SIZE + x];
					cmpt += std::abs(val / TAQUIN_SIZE - _posGoal[val] / TAQUIN_SIZE) + std::abs(val % TAQUIN_SIZE - _posGoal[val] % TAQUIN_SIZE);
				}
			}
			return cmpt;
		};
		auto heuristic = [Manhattan](sNode* a, sNode* b) // So we can experiment with heuristic
		{
			return Manhattan(a, b);
		};

		sNode* nodeCurrent = nodeStart;
		nodeStart->fLocalGoal = 0.0f;
		nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);

		std::list<sNode*> listNotTestedNodes;
		listNotTestedNodes.push_back(nodeStart);

		while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)// Find absolutely shortest path // && nodeCurrent != nodeEnd)
		{
			std::cout << listNotTestedNodes.size() << std::endl;
			// Sort Untested nodes by global goal, so lowest is first
			listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs) { return lhs->fGlobalGoal < rhs->fGlobalGoal; });
			while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
				listNotTestedNodes.pop_front();
			if (listNotTestedNodes.empty())
				break;

			nodeCurrent = listNotTestedNodes.front();
			nodeCurrent->bVisited = true; // We only explore a node once

			//Generation of neighbours
			for (int dir = 0; dir < DIR_END; dir++)
			{
				sNode* nodeGenerated = new sNode;
				nodeGenerated->fGlobalGoal = INFINITY;
				nodeGenerated->fLocalGoal = INFINITY;
				nodeGenerated->parent = nullptr;
				memcpy(nodeGenerated->state, nodeCurrent->state, sizeof(int[TAQUIN_SIZE * TAQUIN_SIZE]));
				nodeGenerated->nIndice_x0 = nodeCurrent->nIndice_x0;
				nodeGenerated->nIndice_y0 = nodeCurrent->nIndice_y0;

				if (Move(nodeGenerated, dir))
				{
					sNode* ptr = IsIn(nodeGenerated, nodes);
					if (ptr != nullptr)	//nodeGenerated a déjà été généré
					{
						delete nodeGenerated;
						nodeGenerated = ptr;
					}
					else                //nodeGenerated n'a jamais été généré
						nodes.push_back(nodeGenerated);
					nodeCurrent->vecNeighbours.push_back(nodeGenerated);
				}
				else
					delete nodeGenerated;
			}
			
			// Check each of this node's neighbours...
			for (auto nodeNeighbour : nodeCurrent->vecNeighbours)
			{
				if (!nodeNeighbour->bVisited)
					listNotTestedNodes.push_back(nodeNeighbour);
				// Calculate the neighbours potential lowest parent distance
				float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + heuristic(nodeCurrent, nodeNeighbour);
				if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
				{
					nodeNeighbour->parent = nodeCurrent;
					nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;
					nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
				}
			}
		}
	}

	virtual bool OnUserUpdate(float fElpasedTime) override
	{
		sNode* parent = nullptr;
		
		if (GetKey(SPACE).bPressed)
		{
			CreateStartNode();
			nodeEnd->bVisited = false;
			nodeEnd->fGlobalGoal = INFINITY;
			nodeEnd->fLocalGoal = INFINITY;
			nodeEnd->parent = nullptr;
			nodeEnd->vecNeighbours.clear();
			nodes.clear();
			nodes.push_back(nodeEnd);
			Solve_Astar();
			//Construction du chemin
			parent = nodeEnd;
			chemin.nbStep = 0;
			while (parent != nullptr)
			{
				chemin.path.push_back(parent);
				chemin.nbStep++;
				printState(parent);
				parent = parent->parent;
			}
			currentNodePath = chemin.path.size() - 1;
		}

		FillRect(0, 0, ScreenWidth(), ScreenHeight(),DARK_CYAN);
		//Draw StartNode and EndNode
		DrawNode(nodeStart, ScreenWidth() / 2 - (int)((TAQUIN_SIZE + 1.5f) * nCaseSize)- 20, ScreenHeight() / 2 - TAQUIN_SIZE * nCaseSize / 2);
		DrawNode(nodeEnd, ScreenWidth() / 2 + TAQUIN_SIZE * nCaseSize / 2 + 20, ScreenHeight() / 2 - TAQUIN_SIZE * nCaseSize/2);
		//Draw Path
		if (currentNodePath >= 0)
		{
			DrawNode(chemin.path[currentNodePath], ScreenWidth() / 2 - (int)(TAQUIN_SIZE * nCaseSize / 2.0f), ScreenHeight() / 2 - TAQUIN_SIZE * nCaseSize / 2);
			currentNodePath--;
			Sleep(800);
		}
		else
			chemin.path.clear();

		//Draw Start/Path/Goal
		DrawString(ScreenWidth() / 2 - (int)((TAQUIN_SIZE + 1.5f) * nCaseSize) + (int)(0.75f * nCaseSize) - 8, ScreenHeight() / 2 - TAQUIN_SIZE * nCaseSize / 2 - 10,"Start", BLACK);
		DrawString(ScreenWidth() / 2 + (int)(TAQUIN_SIZE * nCaseSize / 2) + (int)(0.9f * nCaseSize) + 30, ScreenHeight() / 2 - TAQUIN_SIZE * nCaseSize / 2 - 10, "Goal", BLACK);
		DrawString(ScreenWidth() / 2 - (int)(TAQUIN_SIZE * nCaseSize / 2.0f) + (int)(0.90f * nCaseSize) + 10, ScreenHeight() / 2 - TAQUIN_SIZE * nCaseSize / 2 - 10, "Path", BLACK);
		
		//Display number of steps
		DrawString(ScreenWidth() / 2 - 80, ScreenHeight() / 2 + TAQUIN_SIZE * nCaseSize / 2 + 10, "Number of steps : " + to_string(chemin.nbStep - 1 < 0 ? 0: chemin.nbStep - 1), BLACK);
		
		return true;
	}

	void printState(sNode* node)
	{
		if (node)
		{
			for (int j = 0; j < TAQUIN_SIZE; j++)
			{
				std::cout << "[ ";
				for (int i = 0; i < TAQUIN_SIZE; i++)
				{
					std::cout << node->state[j * TAQUIN_SIZE + i];
					if (i != TAQUIN_SIZE - 1)
						std::cout << " , ";
				}
				std::cout << " ]" << std::endl;
			}
			std::cout << std::endl;
		}
	}
};

int main()
{
	Taquin game;
	game.Construct(600, 370, 2, 2);
	game.Start();
}

