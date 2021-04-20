// JeuDeLaVie.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#define OLC_PGE_APPLICATION
#define TIME_SAMPLE 20 //Time between 2 images
#include "../olcPixelGameEngine.h"
#include <unistd.h>
using namespace olc;

class JDLV : public PixelGameEngine
{
public:
	JDLV()
	{
		sAppName = "Jeu de la Vie";
	}

	

	virtual bool OnUserCreate() override
	{
		nInput = new int[ScreenHeight() * ScreenWidth()];
		nOutput = new int[ScreenHeight() * ScreenWidth()];

		for (int x = 0; x < ScreenWidth(); x++)
		{
			for (int y = 0; y < ScreenHeight(); y++)
			{
				nInput[y * ScreenWidth() + x] = 0;
				nOutput[y * ScreenWidth() + x] = 0;
			}
		}
		return true;
	}

	int NbVoisinsVivants(int x, int y)
	{
		int cmpt = 0;
		if (nInput[y * ScreenWidth() + x + 1] == 1)
			cmpt++;
		if (nInput[(y + 1) * ScreenWidth() + x + 1] == 1)
			cmpt++;
		if (nInput[(y - 1) * ScreenWidth() + x + 1] == 1)
			cmpt++;
		if (nInput[(y + 1) * ScreenWidth() + x] == 1)
			cmpt++;
		if (nInput[(y - 1) * ScreenWidth() + x] == 1)
			cmpt++;
		if (nInput[(y + 1) * ScreenWidth() + x - 1] == 1)
			cmpt++;
		if (nInput[(y - 1) * ScreenWidth() + x - 1] == 1)
			cmpt++;
		if (nInput[y * ScreenWidth() + x - 1] == 1)
			cmpt++;
		return cmpt;
	}

	virtual bool OnUserUpdate(float fElapsedTime) override
	{
		usleep(TIME_SAMPLE*1000);
		int nMouseX = GetMouseX();
		int nMouseY = GetMouseY();
		//Affichage
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				FillRect(x, y, 1, 1, nInput[y * ScreenWidth() + x] == 1 ? WHITE : BLACK);
		
		//Appuie sur bouton
		if(GetKey(SHIFT).bHeld)
		{
			if (GetMouse(0).bHeld)
				nInput[nMouseY * ScreenWidth() + nMouseX] = !nInput[nMouseY * ScreenWidth() + nMouseX];
		}			
		//Mis à jour de l'output
		else
		{
			for (int x = 1; x < ScreenWidth() - 1; x++)
			{
				for (int y = 1; y < ScreenHeight() - 1; y++)
				{
					int nNbVoisinsVivants = NbVoisinsVivants(x, y);
					if (nInput[y * ScreenWidth() + x] == 0)	//Régle 1
						nOutput[y * ScreenWidth() + x] = nNbVoisinsVivants == 3;
					if (nInput[y * ScreenWidth() + x] == 1)	//Régle 2
						nOutput[y * ScreenWidth() + x] = nNbVoisinsVivants == 3 || nNbVoisinsVivants == 2;
				}
			}
			//Copie
			memcpy(nInput, nOutput, sizeof(int) * ScreenHeight() * ScreenWidth());
		}


		return true;
	}
private:
	int* nInput = nullptr;
	int* nOutput = nullptr;
};

int main()
{
	JDLV game;
	game.Construct(160, 100, 8, 8);
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
