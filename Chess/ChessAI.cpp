#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB 
#define OLC_PGE_APPLICATION
#include "./include/olcPixelGameEngine.h"
#include "./include/Chess.h"
#include "./include/AI.h"
using namespace olc;
using namespace std;
#define LVL_MAX 7
struct Rect
{
	int x;
	int y;
	int w;
	int h;
};

bool PointInRect(const v2d_generic<int>& p, const struct Rect& r)
{
	return ((p.x >= r.x) && (p.x < (r.x + r.w)) &&
		(p.y >= r.y) && (p.y < (r.y + r.h))) ? true : false;
}

class AIGame : public PixelGameEngine
{
	bool AI_PLAY = AI_SIDE == BLANC ? 1 : 0;
	bool end = false;
	bool start = false;
	Sprite* Pions[2][6];
	Chess chess;
	CASE Csouris;
	v2d_generic<int> pos_souris_prec;
	AI ai;
	Action prec_action = { -1,-1,-1,-1 };

	int nsquare_size = 80;
	int x0;
	int y0;

	struct Rect Plateau;

	struct Rect Bouton_couleur;
	struct Rect Bouton_start;
	struct Rect LVL_UP;
	struct Rect LVL_DOWN;
	v2d_generic<int> mousePos;

public:
	AIGame()
	{
		sAppName = "AI Chess";
	}
	~AIGame() {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 6; j++)
				delete Pions[i][j];
	}
	virtual bool OnUserCreate() override
	{
		//On charge les sprites
		Sprite* BB = new Sprite("./SpritesChess/blackBishop.png");
		Pions[NOIR][FOU] = BB;
		Sprite* BKK = new Sprite("./SpritesChess/blackKing.png");
		Pions[NOIR][ROI] = BKK;
		Sprite* BKI = new Sprite("./SpritesChess/blackKnight.png");
		Pions[NOIR][CAVALIER] = BKI;
		Sprite* BP = new Sprite("./SpritesChess/blackPawn.png");
		Pions[NOIR][PION] = BP;
		Sprite* BQ = new Sprite("./SpritesChess/blackQueen.png");
		Pions[NOIR][DAME] = BQ;
		Sprite* BR = new Sprite("./SpritesChess/blackRook.png");
		Pions[NOIR][TOUR] = BR;
		Sprite* WB = new Sprite("./SpritesChess/whiteBishop.png");
		Pions[BLANC][FOU] = WB;
		Sprite* WKK = new Sprite("./SpritesChess/whiteKing.png");
		Pions[BLANC][ROI] = WKK;
		Sprite* WKI = new Sprite("./SpritesChess/whiteKnight.png");
		Pions[BLANC][CAVALIER] = WKI;
		Sprite* WPB = new Sprite("./SpritesChess/whitePawn.png");
		Pions[BLANC][PION] = WPB;
		Sprite* WQ = new Sprite("./SpritesChess/whiteQueen.png");
		Pions[BLANC][DAME] = WQ;
		Sprite* WR = new Sprite("./SpritesChess/whiteRook.png");
		Pions[BLANC][TOUR] = WR;
		SetPixelMode(Pixel::MASK);
		
		//Init la souris
		Csouris.couleur = NOIR;
		Csouris.type = VIDE;
		pos_souris_prec.x = -1;
		pos_souris_prec.y = -1;
		//Bouton/Souris
		nsquare_size = 80;
		x0 = ScreenWidth() / 2 - (int)(nsquare_size * (CHESS_SIZE / 2.0f));
		y0 = ScreenHeight() / 2 - (int)(nsquare_size * (CHESS_SIZE / 2.0f));

		Plateau = { x0,  y0, CHESS_SIZE * nsquare_size, CHESS_SIZE * nsquare_size };

		Bouton_couleur = { (int)(x0 +  8.75 * nsquare_size),(int)(y0 + 6.5 * nsquare_size), nsquare_size, nsquare_size };
		Bouton_start = { x0 + - 2 * nsquare_size, (int)(y0 + 3.75 * nsquare_size), nsquare_size, (int)(nsquare_size / 2) };
		LVL_UP = { x0 + 8 * nsquare_size + 85, (int)(y0 + 3.5 * nsquare_size - 24), 30, 24 };
		LVL_DOWN = { x0 + 8 * nsquare_size + 85, (int)(y0 + 4.5 * nsquare_size) , 30, 24 };
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedtime) override
	{
		
		if (AI_PLAY && !end && start)
		{
			prec_action = ai.AI_Play(chess);
			chess.play(prec_action);
			nb_coups++;
			AI_PLAY = false;
		}		
		mousePos.x = GetMouseX(); mousePos.y = GetMouseY();
		//On test les clics de souris
		if (GetMouse(0).bPressed)
		{
			if (PointInRect(mousePos, Plateau)) {
				int x = AI_SIDE == BLANC ? (mousePos.x - x0) / nsquare_size : CHESS_SIZE - 1 - (mousePos.x - x0) / nsquare_size;
				int y = AI_SIDE == BLANC ? (mousePos.y - y0) / nsquare_size : CHESS_SIZE - 1 - (mousePos.y - y0) / nsquare_size;
				if (chess.getCase(x, y).couleur != AI_SIDE && start) {
					pos_souris_prec.x = x;
					pos_souris_prec.y = y;
					Csouris = chess.getCase(x, y);
				}
			}
			else if (PointInRect(mousePos, Bouton_couleur))
				AI_SIDE = !AI_SIDE;
			else if (PointInRect(mousePos, Bouton_start))
			{
				start = true;
				AI_PLAY = AI_SIDE == BLANC ? 1 : 0;
			}
			else if (PointInRect(mousePos, LVL_UP)) {
				if (COUCHE_MAX < LVL_MAX)
					COUCHE_MAX++;
			}
			else if (PointInRect(mousePos, LVL_DOWN)) {
				if (COUCHE_MAX > 1)
					COUCHE_MAX--;
			}
			
		}
		else if (GetMouse(0).bReleased)//On relache le clic on remet le pion choisi pour l'instant
		{
			int x = AI_SIDE == BLANC ? (mousePos.x - x0) / nsquare_size : CHESS_SIZE - 1 - (mousePos.x - x0) / nsquare_size;
			int y = AI_SIDE == BLANC ? (mousePos.y - y0) / nsquare_size : CHESS_SIZE - 1 - (mousePos.y - y0) / nsquare_size;
			if (chess.play(pos_souris_prec.x, pos_souris_prec.y, x, y)) {
				prec_action = { pos_souris_prec.x, pos_souris_prec.y, x, y };
				AI_PLAY = !AI_PLAY;
				nb_coups++;
			}
			
			pos_souris_prec.x = -1;
			pos_souris_prec.y = -1;
			
			Csouris.type = VIDE;
		}
		else if (GetMouse(1).bPressed)//Clic gauche => reset
		{
			chess = Chess();
			AI_PLAY = AI_SIDE == BLANC ? 1 : 0;
			start = end = false;
			nb_coups = 0;
			prec_action = { -1,-1,-1,-1 };
		}

		//Première couche en blanc
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), WHITE);
		//Bouton choix de la couleur
		FillRect(Bouton_couleur.x, Bouton_couleur.y, Bouton_couleur.w, Bouton_couleur.h, AI_SIDE ? BLACK : WHITE);
		DrawRect(Bouton_couleur.x, Bouton_couleur.y, Bouton_couleur.w, Bouton_couleur.h, BLACK);
		DrawString(Bouton_couleur.x + Bouton_couleur.w / 6, Bouton_couleur.y + Bouton_couleur.h / 2.25, "Couleur", AI_SIDE ? WHITE : BLACK);
		//Bouton start
		FillRect(Bouton_start.x, Bouton_start.y, Bouton_start.w, Bouton_start.h, start ? DARK_GREEN : WHITE);
		DrawRect(Bouton_start.x, Bouton_start.y, Bouton_start.w, Bouton_start.h, BLACK);
		DrawString(Bouton_start.x + Bouton_start.w / 4, Bouton_start.y + Bouton_start.h/ 2.5, "Start", BLACK);
		//Bouton lvl IA
		DrawLine(LVL_UP.x             , LVL_UP.y + LVL_UP.h, LVL_UP.x + LVL_UP.w / 2, LVL_UP.y           , BLACK);
		DrawLine(LVL_UP.x + LVL_UP.w/2, LVL_UP.y           , LVL_UP.x + LVL_UP.w    , LVL_UP.y + LVL_UP.h, BLACK);
		DrawLine(LVL_UP.x + LVL_UP.w  , LVL_UP.y + LVL_UP.h, LVL_UP.x               , LVL_UP.y + LVL_UP.h, BLACK);

		DrawString(LVL_DOWN.x + 5, LVL_UP.y + LVL_UP.h + (LVL_DOWN.y - (LVL_UP.y + LVL_UP.h))/2 - 10, to_string(COUCHE_MAX), BLACK, 3);

		DrawLine(LVL_DOWN.x                 , LVL_DOWN.y             , LVL_DOWN.x + LVL_DOWN.w / 2, LVL_DOWN.y + LVL_DOWN.h, BLACK);
		DrawLine(LVL_DOWN.x + LVL_DOWN.w / 2, LVL_DOWN.y + LVL_DOWN.h, LVL_DOWN.x + LVL_DOWN.w    , LVL_DOWN.y             , BLACK);
		DrawLine(LVL_DOWN.x + LVL_DOWN.w    , LVL_DOWN.y             , LVL_DOWN.x                 , LVL_DOWN.y             , BLACK);
		//Draw squares
		for (int x = 0; x < CHESS_SIZE; x += 2)
			for (int y = 1 ; y < CHESS_SIZE; y += 2)
				FillRect(x0 + x * nsquare_size, y0 + y * nsquare_size, nsquare_size, nsquare_size, DARK_GREY);
		for (int x = 1; x < CHESS_SIZE; x += 2)
			for (int y = 0; y < CHESS_SIZE; y += 2)
				FillRect(x0 + x * nsquare_size, y0 + y * nsquare_size, nsquare_size, nsquare_size, DARK_GREY);
		//Draw coup précédent
		if (prec_action.c1 >= 0 && prec_action.l1 >= 0 && prec_action.c2 >= 0 && prec_action.l2 >= 0)
		{
			FillRect(x0 + (AI_SIDE == BLANC ? prec_action.c1 : CHESS_SIZE - 1 - prec_action.c1) * nsquare_size, y0 + (AI_SIDE == BLANC ? prec_action.l1 : CHESS_SIZE - 1 - prec_action.l1) * nsquare_size, nsquare_size, nsquare_size, BLUE);
			FillRect(x0 + (AI_SIDE == BLANC ? prec_action.c2 : CHESS_SIZE - 1 - prec_action.c2) * nsquare_size, y0 + (AI_SIDE == BLANC ? prec_action.l2 : CHESS_SIZE - 1 - prec_action.l2) * nsquare_size, nsquare_size, nsquare_size, BLUE);
		}
		//Draw lines
		for (int x = 0; x <= CHESS_SIZE; x++)
			DrawLine(x0 + x * nsquare_size, y0, x0 + x * nsquare_size, y0 + CHESS_SIZE * nsquare_size, BLACK);
		for (int y = 0; y <= CHESS_SIZE; y++)
			DrawLine(x0, y0 + y * nsquare_size, x0 + CHESS_SIZE * nsquare_size, y0 + y * nsquare_size, BLACK);

		//Draw Sprites
		for (int x = 0; x < CHESS_SIZE; x++)
			for (int y = 0; y < CHESS_SIZE; y++)
				if (chess.getCase(x, y).type != VIDE && (x != pos_souris_prec.x || (y != pos_souris_prec.y)))
						DrawSprite(x0 + (AI_SIDE == BLANC ? x : CHESS_SIZE - 1 - x) * nsquare_size, y0 + (AI_SIDE == BLANC ? y : CHESS_SIZE - 1 - y) * nsquare_size, Pions[chess.getCase(x, y).couleur][chess.getCase(x, y).type]);
		//Draw sprite de la souris
		if(Csouris.type != VIDE)
			DrawSprite(mousePos.x - Pions[Csouris.couleur][Csouris.type]->width/2, mousePos.y - Pions[Csouris.couleur][Csouris.type]->height / 2, Pions[Csouris.couleur][Csouris.type]);

		//Test victoire ou defaite
		end = true;
		if (chess.getNb_Piece(BLANC, ROI) < 1)
			DrawString(x0 + (int)((CHESS_SIZE / 2.0f) * nsquare_size - 15), y0 - 10, "BLACK WIN", BLACK);
		else if (chess.getNb_Piece(NOIR, ROI) < 1)
			DrawString(x0 + (int)((CHESS_SIZE / 2.0f) * nsquare_size - 15), y0 - 10, "WHITE WIN", BLACK);
		else
			end = false;
		
		return true;
	}

};

int main()
{
	AIGame game;
	game.Construct(1080, 720, 1, 1);
	game.Start();
	return 0;
}
