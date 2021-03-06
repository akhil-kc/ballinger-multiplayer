#pragma once

//#include "cPlayer.h"
#include "cSound.h"
#include "cRespawnPoint.h"
#include "cColumn.h"
#include "cPortal.h"
#include "cKey.h"
#include "cDino.h"
#define SCREEN_WIDTH	1366
#define SCREEN_HEIGHT	745

#define FRAMERATE 60
#define GRAVITY 0.015f

enum{
	STATE_LIVELOSS,
	STATE_RUN,
	STATE_ENDGAME,
	STATE_MENU,
	STATE_MULTIPLAYER,
	STATE_CONNECT,
	STATE_WAIT_CONNECT,
	STATE_CLIENT,
	STATE_CHAT,
	STATE_GAME_OVER
};

#define TOTAL_LEVELS	  1

//controls
#define P_UP		'w'
#define P_DOWN		's'
#define P_LEFT		'a'
#define P_RIGHT		'd'
#define P_JUMP		' '
#define P_PLUS		'+'
#define P_MINUS		'-'

class cGame
{
public:
	cGame(void);
	virtual ~cGame(void);

	bool Init(int lvl);
	bool Loop();
	void Finalize();

	//Input
	void ReadKeyboard(unsigned char key, int x, int y, bool press);
	void ReadSpecialKeyboard(unsigned char key, int x, int y, bool press);
	void ReadMouse(int button, int state, int x, int y);
	void ReadMouseMotion(int x, int y);
	//Process
	bool Process();
	//Output
	void Reshape(int w, int h);
	void Render();
	void drawNavigation(float player1x, float player1z, float player2x, float player2z);
	//bool checkDeadReckoning(float x, float z);

public:
	unsigned char keys[256];
	bool mouse_left_down,mouse_right_down;
	int level,state,respawn_id,pickedkey_id;
	bool noclip,portal_activated;
	float time,ang, noclipSpeedF;
	int score, lives;
	std::vector<cRespawnPoint> respawn_points;
	std::vector<cKey> target_keys;
	std::vector<cKey> white_keys;
	std::vector<cColumn> columns;
	cPortal Portal;
	cScene Scene;
	cData Data;
	//Dino dino;
	Dino Player;
	Dino Player2;
	cCamera Camera;
	cSound Sound;
	cShader Shader;
	cModel Model;
	cTerrain Terrain;
	cLava Lava;
	static int main_win, sub_win;
	

	void Physics(cBicho &object);
	
};