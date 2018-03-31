#undef UNICODE
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <signal.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
// #pragma comment (lib, "Mswsock.lib")
#define SIGINT 2
#define SIGKILL 9
#define SIGQUIT 3

#include "cGame.h"

void draw_start_menu();
void draw_chat_window(std::string chatMsg);
void draw_score(int score, int lives);
void draw_transition_screen(std::string connectMsg);
void draw_game_over();
void startServer();
void s_cl(char *a, int x);
void s_handle(int s);
int connectToServer();
DWORD WINAPI receive_cmd_server(LPVOID lpParam);
DWORD WINAPI receive_cmd_client(LPVOID lpParam);
cGame::cGame(void) {}
cGame::~cGame(void){}
SOCKET sock, client;
char ipAddress[100];
char chatBuffer[1024];
int ipSize = 0;
int chatSize = 0;
float mx, my, mz;
Coord playerPos;
Coord playerPos_recv;
int cGame::main_win = 0;
int cGame::sub_win = 0;

int sendKeyId = -1, recvKeyId = -1;
bool isSendChat = false, isRecvChat = false, isMultiplayer = false;
bool sendKeyDeployed = false, sendKeyLost = false, recvKeyDeployed = false, recvKeyLost = false;
std::string sendChatMsg;
std::string recvChatMsg;

bool cGame::Init(int lvl)
{
	bool res = true;
	noclip = false;
	portal_activated = false;
	time = ang = 0.0f;
	noclipSpeedF = 1.0f; 
	level = lvl;
	score = 0;
	lives = 3;
	state = STATE_MENU;
	respawn_id = 0;
	pickedkey_id = -1;

	//Graphics initialization
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,(float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,CAMERA_ZNEAR,CAMERA_ZFAR);
	glMatrixMode(GL_MODELVIEW);

	const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_position[] = { 50.0f, 50.0f, 50.0f, 0.0f };
	const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
	const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
	const GLfloat mat_specular[]   = { 0.3f, 0.3f, 0.3f, 1.0f };
	const GLfloat mat_shininess[]  = { 100.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Texture initialization
	Data.Load();

	//level initialization(terrain, lava and skybox)
	Scene.LoadLevel(level,&Terrain,&Lava,CAMERA_ZFAR);

	//Sound initialization
	Sound.Load();

	//Shader initialization
	Shader.Load();

	//Model initialization
	Model.Load();

	//target keys initialization
	cKey key;
	key.SetPos(883,Terrain.GetHeight(883,141),141);
	target_keys.push_back(key);
	key.SetPos(345,Terrain.GetHeight(345,229),229);
	target_keys.push_back(key);
	key.SetPos(268,Terrain.GetHeight(268,860),860);
	target_keys.push_back(key);
	key.SetPos(780,Terrain.GetHeight(780,858),858);
	target_keys.push_back(key);
	key.SetPos(265,Terrain.GetHeight(265,487),487);
	target_keys.push_back(key);

	// white keys init
	key.SetPos(580, Terrain.GetHeight(580, 564), 564);
	white_keys.push_back(key);
	key.SetPos(432, Terrain.GetHeight(432, 430), 430);
	white_keys.push_back(key);
	key.SetPos(373, Terrain.GetHeight(373, 640), 640);
	white_keys.push_back(key);
	key.SetPos(508, Terrain.GetHeight(508, 817), 817);
	white_keys.push_back(key);
	key.SetPos(708, Terrain.GetHeight(708, 866), 866);
	white_keys.push_back(key);
	key.SetPos(810, Terrain.GetHeight(810, 828), 828);
	white_keys.push_back(key);
	key.SetPos(843, Terrain.GetHeight(843, 429), 828);
	white_keys.push_back(key);
	key.SetPos(757, Terrain.GetHeight(757, 218), 218);
	white_keys.push_back(key);
	key.SetPos(661, Terrain.GetHeight(661, 137), 137);
	white_keys.push_back(key);
	key.SetPos(304, Terrain.GetHeight(304, 248), 248);
	white_keys.push_back(key);

	//columns initialization
	cColumn col;
	col.SetColumn(TERRAIN_SIZE/2+18,Terrain.GetHeight(TERRAIN_SIZE/2+18,TERRAIN_SIZE/2+8),TERRAIN_SIZE/2+8,   90);
	columns.push_back(col);
	col.SetColumn(TERRAIN_SIZE/2+14,Terrain.GetHeight(TERRAIN_SIZE/2+14,TERRAIN_SIZE/2-8),TERRAIN_SIZE/2-8,   90);
	columns.push_back(col);
	col.SetColumn(TERRAIN_SIZE/2,Terrain.GetHeight(TERRAIN_SIZE/2,TERRAIN_SIZE/2-16),TERRAIN_SIZE/2-16,      180);
	columns.push_back(col);
	col.SetColumn(TERRAIN_SIZE/2-14,Terrain.GetHeight(TERRAIN_SIZE/2-14,TERRAIN_SIZE/2-8),TERRAIN_SIZE/2-8,  -90);
	columns.push_back(col);
	col.SetColumn(TERRAIN_SIZE/2-18,Terrain.GetHeight(TERRAIN_SIZE/2-18,TERRAIN_SIZE/2+8),TERRAIN_SIZE/2+8,  -90);
	columns.push_back(col);

	//Player initialization
	Player.SetPos(TERRAIN_SIZE/2,Terrain.GetHeight(TERRAIN_SIZE/2,TERRAIN_SIZE/2)+RADIUS,TERRAIN_SIZE/2);
	if (isMultiplayer)
		Player2.SetPos(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2) + RADIUS, TERRAIN_SIZE / 2);

	//Portal initialization
	Portal.SetPos(TERRAIN_SIZE/2,Terrain.GetHeight(TERRAIN_SIZE/2,TERRAIN_SIZE/2+32),TERRAIN_SIZE/2+32);

	//respawn points initialization
	cRespawnPoint rp;
	rp.SetPos(TERRAIN_SIZE/2,Terrain.GetHeight(TERRAIN_SIZE/2,TERRAIN_SIZE/2),TERRAIN_SIZE/2);
	respawn_points.push_back(rp);
	rp.SetPos(256,Terrain.GetHeight(256,160),160);
	respawn_points.push_back(rp);
	rp.SetPos(840,Terrain.GetHeight(840,184),184);
	respawn_points.push_back(rp);
	rp.SetPos(552,Terrain.GetHeight(552,760),760);
	respawn_points.push_back(rp);
	rp.SetPos(791,Terrain.GetHeight(791,850),850);
	respawn_points.push_back(rp);
	rp.SetPos(152,Terrain.GetHeight(152,832),832);
	respawn_points.push_back(rp);
	rp.SetPos(448,Terrain.GetHeight(448,944),944);
	respawn_points.push_back(rp);
	rp.SetPos(816,Terrain.GetHeight(816,816),816);
	respawn_points.push_back(rp);

	Sound.Play(SOUND_AMBIENT);

	return res;
}

bool cGame::Loop()
{
	bool res=true;
	int t1,t2;
	t1 = glutGet(GLUT_ELAPSED_TIME);
	
    if(state == STATE_LIVELOSS)
	{
		Render();
		glutSetWindow(cGame::sub_win);
		View4Display();
		glutSetWindow(cGame::main_win);
		Player.SetPos(respawn_points[respawn_id].GetX(),respawn_points[respawn_id].GetY()+RADIUS,respawn_points[respawn_id].GetZ());
		state = STATE_RUN;
	}
	else if(state == STATE_ENDGAME)
	{
		res=false;
	}
	else {
		res = Process();
		if (res) {
			Render();
			glutSetWindow(cGame::sub_win);
			View4Display();
			glutSetWindow(cGame::main_win);
		}
	}

	do { t2 = glutGet(GLUT_ELAPSED_TIME);
	} while(t2-t1 < 1000/FRAMERATE);
	return res;
}

void cGame::Finalize()
{
}

//Input
void cGame::ReadKeyboard(unsigned char key, int x, int y, bool press)
{
	if (state == STATE_MULTIPLAYER) {
		if ((key == 'Y' || key == 'y') && !press) {
			state = STATE_WAIT_CONNECT;
		}
		else if (key == 'N' || key == 'n')
			state = STATE_CLIENT;
	}

	if (state == STATE_CLIENT) {
		if (key == 13)
			state = STATE_CONNECT;
		if ((isdigit(key) || key == '.') && !press) {
			ipAddress[ipSize] = key;
			ipSize++;
		}
		return;
	}

	if (state == STATE_CHAT) {
		if ((isRecvChat == true) && (key == 13)) {
			isRecvChat = false;
			state = STATE_RUN;
			return;
		}

		if (key == 13) {
			state = STATE_RUN;
			isSendChat = true;
			sendChatMsg = std::string(chatBuffer);
			for (int i = 0 ; i< chatSize; i++)
				chatBuffer[i] = 0;
			chatSize = 0;
			keys['c'] = false;
			Render();
			return;
		}
		else if (press) {
			chatBuffer[chatSize] = key;
			chatSize++;
		}
		return;
	}

	if(key >= 'A' && key <= 'Z') key += 32;
	keys[key] = press;

}

void cGame::ReadSpecialKeyboard(unsigned char key, int x, int y, bool press)
{
	if(key == GLUT_KEY_F10 && press)
	{
		if(!noclip) Sound.Play(SOUND_SCREWGRAVITY);
		noclip = !noclip;
		Player.SetVel(0.0f,0.0f,0.0f);
	}
}

void cGame::ReadMouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN) {
        if(button == GLUT_LEFT_BUTTON) {
            mouse_left_down = true;
        }
        else if(button == GLUT_RIGHT_BUTTON) {
            mouse_right_down = true;
        }
    }
    else if(state == GLUT_UP) {
        if(button == GLUT_LEFT_BUTTON) {
            mouse_left_down = false;
        }
        else if(button == GLUT_RIGHT_BUTTON) {
            mouse_right_down = false;
        }
    }
}

void cGame::ReadMouseMotion(int x, int y)
{
	// This variable is hack to stop glutWarpPointer from triggering an event callback to Mouse(...)
    // This avoids it being called recursively and hanging up the event loop
    static bool just_warped = false;

    if(just_warped) {
        just_warped = false;
        return;
    }

	//the amount displaced in pixels
    int dx = x - SCREEN_WIDTH/2;
    int dy = y - SCREEN_HEIGHT/2;

    if(dx) {
        Camera.RotateYaw(CAMERA_SPEED*dx);
		if(Camera.GetState() != STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw());
    }

    if(dy) {
        Camera.RotatePitch(-CAMERA_SPEED*dy);
    }

    glutWarpPointer(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

    just_warped = true;
}

void ResetViewport()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.0, 2.0, -2.0, 2.0, 0.5, 5.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//View4Display
void cGame::View4Display() {
	glClearColor(0.0, 0.0, 0.0, 0.0);         // black background 
	glMatrixMode(GL_PROJECTION);              // setup viewing projection 
	glLoadIdentity();                           // start with identity matrix
	glLineWidth(2.5);
	glOrtho(0.0, 10.0, 0.0, 10.0, -1.0, 1.0);   // setup a 10x10x2 viewing world
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_LINE);

	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10.0, 0.0, 0.0);

	glEnd();
	glFlush();
	glutSwapBuffers();
}

//Process
bool cGame::Process()
{
	bool res = true;
	//Process Input
	if (keys[27])	res = false;

	if (recvKeyDeployed && (recvKeyId > 0 )) {
		target_keys[recvKeyId].Deploy();
		recvKeyDeployed = false;
		recvKeyId = -1;
	}
	if (recvKeyLost) {
		recvKeyId = -1;
	}

	if (state == STATE_MENU) {
		if (keys['s']) {
			state = STATE_RUN;
			Render();
			glutSetWindow(cGame::sub_win);
			View4Display();
			glutSetWindow(cGame::main_win);
			//	return;
		}
		else if (keys['m']) {
			isMultiplayer = true;
			state = STATE_MULTIPLAYER;
			Render();
			glutSetWindow(cGame::sub_win);
			View4Display();
			glutSetWindow(cGame::main_win);
		}
	}
	else if (state == STATE_RUN && keys['c']) {
			state = STATE_CHAT;
			Render();
	}
	else {

		float vx, vy, vz;
		Camera.GetDirectionVector(vx, vy, vz);
		float factor = sqrt(1.0f / (vx*vx + vz * vz));

		if (keys['1']) Camera.SetState(STATE_FPS);
		Player.SetFade(!keys['2']);
		if (keys['3']) Camera.SetState(STATE_TPS);


		if (noclip)
		{
			if (keys[P_UP])
			{
				Player.SetX(Player.GetX() + noclipSpeedF * vx);
				Player.SetY(Player.GetY() + noclipSpeedF * vy);
				Player.SetZ(Player.GetZ() + noclipSpeedF * vz);
				if (Camera.GetState() == STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw());
			}
			else if (keys[P_DOWN])
			{
				Player.SetX(Player.GetX() - noclipSpeedF * vx);
				Player.SetY(Player.GetY() - noclipSpeedF * vy);
				Player.SetZ(Player.GetZ() - noclipSpeedF * vz);
				if (Camera.GetState() == STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw() - PI);
			}
			if (keys[P_LEFT])
			{
				Player.SetX(Player.GetX() + noclipSpeedF * vz*factor);
				Player.SetZ(Player.GetZ() - noclipSpeedF * vx*factor);
				if (Camera.GetState() == STATE_TPS_FREE)
				{
					if (keys['w']) Camera.SetLastYaw(Camera.GetYaw() - PI / 4);
					else if (keys['s']) Camera.SetLastYaw(Camera.GetYaw() - (PI * 3) / 4);
					else Camera.SetLastYaw(Camera.GetYaw() - PI / 2);
				}
			}
			else if (keys[P_RIGHT])
			{
				Player.SetX(Player.GetX() - noclipSpeedF * vz*factor);
				Player.SetZ(Player.GetZ() + noclipSpeedF * vx*factor);
				if (Camera.GetState() == STATE_TPS_FREE)
				{
					if (keys['w']) Camera.SetLastYaw(Camera.GetYaw() + PI / 4);
					else if (keys['s']) Camera.SetLastYaw(Camera.GetYaw() + (PI * 3) / 4);
					else Camera.SetLastYaw(Camera.GetYaw() + PI / 2);
				}
			}
			if (mouse_left_down) Player.SetY(Player.GetY() + noclipSpeedF);
			else if (mouse_right_down) Player.SetY(Player.GetY() - noclipSpeedF);
			if (keys[P_PLUS])
			{
				noclipSpeedF += 0.01f;
				if (noclipSpeedF > 2.0f) noclipSpeedF = 2.0f;
			}
			else if (keys[P_MINUS])
			{
				noclipSpeedF -= 0.01f;
				if (noclipSpeedF < 0.05f) noclipSpeedF = 0.05f;
			}
		}
		else
		{
			if (keys[P_UP])
			{
				float nextVX = Player.GetVX() + PLAYER_SPEED * vx*factor;
				float nextVZ = Player.GetVZ() + PLAYER_SPEED * vz*factor;
				float limitation_factor;
				if (sqrt(nextVX*nextVX + nextVZ * nextVZ) <= MAX_MOVEMENT) limitation_factor = 1.0f;
				else limitation_factor = sqrt((MAX_MOVEMENT*MAX_MOVEMENT) / (nextVX*nextVX + nextVZ * nextVZ));
				Player.SetVX(nextVX*limitation_factor);
				Player.SetVZ(nextVZ*limitation_factor);

				if (Camera.GetState() == STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw());
			}
			else if (keys[P_DOWN])
			{
				float nextVX = Player.GetVX() - PLAYER_SPEED * vx*factor;
				float nextVZ = Player.GetVZ() - PLAYER_SPEED * vz*factor;
				float limitation_factor;
				if (sqrt(nextVX*nextVX + nextVZ * nextVZ) <= MAX_MOVEMENT) limitation_factor = 1.0f;
				else limitation_factor = sqrt((MAX_MOVEMENT*MAX_MOVEMENT) / (nextVX*nextVX + nextVZ * nextVZ));
				Player.SetVX(nextVX*limitation_factor);
				Player.SetVZ(nextVZ*limitation_factor);

				if (Camera.GetState() == STATE_TPS_FREE) Camera.SetLastYaw(Camera.GetYaw() - PI);
			}
			if (keys[P_LEFT])
			{
				float nextVX = Player.GetVX() + PLAYER_SPEED * vz*factor;
				float nextVZ = Player.GetVZ() - PLAYER_SPEED * vx*factor;
				float limitation_factor;
				if (sqrt(nextVX*nextVX + nextVZ * nextVZ) <= MAX_MOVEMENT) limitation_factor = 1.0f;
				else limitation_factor = sqrt((MAX_MOVEMENT*MAX_MOVEMENT) / (nextVX*nextVX + nextVZ * nextVZ));
				Player.SetVX(nextVX*limitation_factor);
				Player.SetVZ(nextVZ*limitation_factor);

				if (Camera.GetState() == STATE_TPS_FREE)
				{
					if (keys['w']) Camera.SetLastYaw(Camera.GetYaw() - PI / 4);
					else if (keys['s']) Camera.SetLastYaw(Camera.GetYaw() - (PI * 3) / 4);
					else Camera.SetLastYaw(Camera.GetYaw() - PI / 2);
				}
			}
			else if (keys[P_RIGHT])
			{
				float nextVX = Player.GetVX() - PLAYER_SPEED * vz*factor;
				float nextVZ = Player.GetVZ() + PLAYER_SPEED * vx*factor;
				float limitation_factor;
				if (sqrt(nextVX*nextVX + nextVZ * nextVZ) <= MAX_MOVEMENT) limitation_factor = 1.0f;
				else limitation_factor = sqrt((MAX_MOVEMENT*MAX_MOVEMENT) / (nextVX*nextVX + nextVZ * nextVZ));
				Player.SetVX(nextVX*limitation_factor);
				Player.SetVZ(nextVZ*limitation_factor);

				if (Camera.GetState() == STATE_TPS_FREE)
				{
					if (keys['w']) Camera.SetLastYaw(Camera.GetYaw() + PI / 4);
					else if (keys['s']) Camera.SetLastYaw(Camera.GetYaw() + (PI * 3) / 4);
					else Camera.SetLastYaw(Camera.GetYaw() + PI / 2);
				}
			}
			if (keys[P_JUMP])
			{
				if (Player.GetY() - RADIUS < Terrain.GetHeight(Player.GetX(), Player.GetZ()) + 0.01f)
				{
					Player.SetVY(PLAYER_JUMP_SPEED);
					Sound.PlayBounce(1.0f);
				}
			}

			float initial_z = Player.GetZ();
			Physics(Player);

			//check if player dies
			if (Player.GetY() <= Lava.GetHeight() + RADIUS)
			{
				Player.SetY(Lava.GetHeight() + RADIUS);
				Player.SetVel(0.0f, 0.0f, 0.0f);
				pickedkey_id = -1;
				sendKeyLost = true;
				sendKeyId = -1;
				if (lives != 0)
					lives--;
				state = STATE_LIVELOSS;
				Sound.Play(SOUND_SWISH);
				if (lives <= 0)
					state = STATE_GAME_OVER;
			}

			Coord P; P.x = Player.GetX(); P.y = Player.GetY(); P.z = Player.GetZ();
			float r = RADIUS;

			//Check if the player enters any Respawn Point
			float cr = CIRCLE_RADIUS, ah = AURA_HEIGHT;
			for (unsigned int i = 0; i < respawn_points.size(); i++)
			{
				Coord RP; RP.x = respawn_points[i].GetX(); RP.y = respawn_points[i].GetY(); RP.z = respawn_points[i].GetZ();
				if (sqrt((P.x - RP.x)*(P.x - RP.x) + (P.y - RP.y)*(P.y - RP.y) + (P.z - RP.z)*(P.z - RP.z)) <= RADIUS + CIRCLE_RADIUS)
				{
					if (respawn_id != i) Sound.Play(SOUND_SWISH);
					respawn_id = i;
				}
			}

			//check if the player picks up a key
			if (pickedkey_id == -1)
			{
				for (unsigned int i = 0; i < target_keys.size(); i++)
				{
					if (!target_keys[i].IsDeployed())
					{
						Coord K; K.x = target_keys[i].GetX(); K.y = target_keys[i].GetY(); K.z = target_keys[i].GetZ();
						if (sqrt((P.x - K.x)*(P.x - K.x) + (P.y - K.y)*(P.y - K.y) + (P.z - K.z)*(P.z - K.z)) <= RADIUS * 2)
						{
							sendKeyId = i;
							pickedkey_id = i;
							Sound.Play(SOUND_PICKUP);
						}
					}
				}
			}

			int whiteKeyId = -1;
			for (unsigned int i = 0; i < white_keys.size(); i++)
			{
					Coord K; K.x = white_keys[i].GetX(); K.y = white_keys[i].GetY(); K.z = white_keys[i].GetZ();
					if (sqrt((P.x - K.x)*(P.x - K.x) + (P.y - K.y)*(P.y - K.y) + (P.z - K.z)*(P.z - K.z)) <= RADIUS * 2)
					{
						score += 20;
						whiteKeyId = i;
						Sound.Play(SOUND_PICKUP);
					}
			}

			if (whiteKeyId != -1) {
				white_keys.erase(white_keys.begin() + whiteKeyId);
			}

			//check if the player arrives with a key to their respective column
			if (pickedkey_id != -1)
			{
				if (columns[pickedkey_id].InsideGatheringArea(P.x, P.y, P.z))
				{
					Sound.Play(SOUND_UNLOCK);
					Sound.Play(SOUND_ENERGYFLOW);
					score += 100;
					sendKeyDeployed = true;
					target_keys[pickedkey_id].Deploy();
					pickedkey_id = sendKeyId = -1;
					if (respawn_id)
					{
						Sound.Play(SOUND_SWISH);
						respawn_id = 0;
					}
					bool all_keys_deployed = true;
					for (unsigned int i = 0; all_keys_deployed && i < target_keys.size(); i++) all_keys_deployed = target_keys[i].IsDeployed();
					portal_activated = all_keys_deployed;
					if (portal_activated) Sound.Play(SOUND_WARP);
				}
			}

			//check if the player goes through the portal while activated
			if (portal_activated)
			{
				if (Portal.InsidePortal(P.x, P.y, P.z, RADIUS))
				{
					if ((initial_z - Portal.GetZ() <= 0.0f && Player.GetZ() - Portal.GetZ() >= 0.0f) ||
						(initial_z - Portal.GetZ() >= 0.0f && Player.GetZ() - Portal.GetZ() <= 0.0f)) state = STATE_ENDGAME;
				}
			}
		}

		//clean sound buffer
		Sound.Update();
	}
	return res;
}

void cGame::Physics(cBicho &object)
{
	Coord initialPos; initialPos.x = object.GetX(); initialPos.y = object.GetY(); initialPos.z = object.GetZ();
	Coord center; center.x = object.GetX() + object.GetVX(); center.y = object.GetY() + object.GetVY(); center.z = object.GetZ() + object.GetVZ();
	std::vector<Vector> cnormals = Terrain.GetCollisionNormals(center,RADIUS);
	object.SetPos(center.x,center.y,center.z); //despues de GetCollisionNormals la posicion center sera una posicion valida sobre la superficie

	//update angles of rotation by movement
	if(object.GetZ() != initialPos.z || object.GetX() != initialPos.x)
	{
		float yaw,pitch;
		float dx = abs(abs(object.GetX()) - abs(initialPos.x)), dz = abs(abs(object.GetZ()) - abs(initialPos.z));
		if(object.GetZ() > initialPos.z && object.GetX() >= initialPos.x) yaw = atan( dx / dz ); //primer cuadrante
		if(object.GetZ() <= initialPos.z && object.GetX() > initialPos.x) yaw = PI/2 + atan( dz / dx ); //segundo cuadrante
		if(object.GetZ() < initialPos.z && object.GetX() <= initialPos.x) yaw = PI + atan( dx / dz );//tercer cuadrante
		if(object.GetZ() >= initialPos.z && object.GetX() < initialPos.x) yaw = PI*3/2 + atan( dz / dx );//cuarto cuadrante
		object.SetYaw(yaw*(180/PI));

		float perimeter = PI*2*RADIUS;
		float dy = abs(abs(object.GetY()) - abs(initialPos.y));
		float travel_dist = sqrt( dx*dx + dy*dy + dz*dz );
		if(cos(yaw) >= 0.0f) pitch = object.GetPitch() + (travel_dist/perimeter) * 360.0f;
		else pitch = object.GetPitch() - (travel_dist/perimeter) * 360.0f;
		if(pitch < 0.0f) pitch = 360.0f - abs(pitch);
		object.SetPitch(fmod(pitch,360.0f));
	}

	if(cnormals.empty()) object.SetVY(object.GetVY() - GRAVITY);
	else
	{
		Vector G,F,G1,F1,cNormal;
		float rz,rx; //rotation angles
		float factor,N = 0.0f;

		G.x = 0.0f; G.y = -GRAVITY; G.z = 0.0f;
		F.x = object.GetVX(); F.y = object.GetVY(); F.z = object.GetVZ();
		cNormal.x = 0.0f; cNormal.y = 0.0f; cNormal.z = 0.0f;

		for(unsigned int i=0; i<cnormals.size(); i++)
		{
			if(cnormals[i].x == 0.0f) rz = 0.0f;
			else if(cnormals[i].x >  0.0f) rz = -PI/2 + atan(cnormals[i].y/cnormals[i].x);
			else rz = PI/2 + atan(cnormals[i].y/cnormals[i].x);

			if(cnormals[i].z == 0.0f) rx = 0.0f;
			else if(cnormals[i].z >  0.0f) rx = PI/2 - atan(cnormals[i].y/cnormals[i].z);
			else rx = -PI/2 - atan(cnormals[i].y/cnormals[i].z);

			//transform the forces defined in the OpenGL coordinate system to the coordinate system defined by cnormal(y axis)
			G1.x = cos(-rz)*G.x - sin(-rz)*G.y;
			G1.y = cos(-rx)*sin(-rz)*G.x + cos(-rx)*cos(-rz)*G.y - sin(-rx)*G.z;
			G1.z = sin(-rx)*sin(-rz)*G.x + sin(-rx)*cos(-rz)*G.y + cos(-rx)*G.z;

			F1.x = cos(-rz)*F.x - sin(-rz)*F.y;
			F1.y = cos(-rx)*sin(-rz)*F.x + cos(-rx)*cos(-rz)*F.y - sin(-rx)*F.z;
			F1.z = sin(-rx)*sin(-rz)*F.x + sin(-rx)*cos(-rz)*F.y + cos(-rx)*F.z;
			
			//consider the normal force for a single triangle
			float cN = 0.0f;
			if (G1.y < 0.0f) {cN -= G1.y; G1.y = 0.0f;}
			if (F1.y < 0.0f) {cN -= F1.y; F1.y = 0.0f;}
			N += cN; //update global normal force

			//update global cnormal force
			cNormal.x += cnormals[i].x;
			cNormal.y += cnormals[i].y;
			cNormal.z += cnormals[i].z;

			//consider the possible friction
			if(cN > 0.0f && abs(F1.x) + abs(F1.z) > 0.0f)
			{
				factor = sqrt( ((FRICTION*cN)*(FRICTION*cN)) / (F1.x*F1.x + F1.z*F1.z) );

				if(abs(F1.x) < abs(F1.x*factor)) F1.x = 0.0f;
				else F1.x -= F1.x*factor;

				if(abs(F1.z) < abs(F1.z*factor)) F1.z = 0.0f;
				else F1.z -= F1.z*factor;
			}
			
			//we return to Transform the forces of the cnormal coordinate system(y axis) to the OpenGL coordinate system
			G.x = cos(rz)*G1.x - sin(rz)*cos(rx)*G1.y + sin(rz)*sin(rx)*G1.z;
			G.y = sin(rz)*G1.x + cos(rz)*cos(rx)*G1.y - cos(rz)*sin(rx)*G1.z;
			G.z = sin(rx)*G1.y + cos(rx)*G1.z;

			F.x = cos(rz)*F1.x - sin(rz)*cos(rx)*F1.y + sin(rz)*sin(rx)*F1.z;
			F.y = sin(rz)*F1.x + cos(rz)*cos(rx)*F1.y - cos(rz)*sin(rx)*F1.z;
			F.z = sin(rx)*F1.y + cos(rx)*F1.z;
		}

		float nextVX = F.x + G.x;
		float nextVY = F.y + G.y;
		float nextVZ = F.z + G.z;

		//limitaremos la velocidad para que la esfera no se salte triangulos
		float limitation_factor;
		if( sqrt(nextVX*nextVX + nextVY*nextVY + nextVZ*nextVZ) <= MAX_MOVEMENT ) limitation_factor = 1.0f;
		else limitation_factor = sqrt( (MAX_MOVEMENT*MAX_MOVEMENT)/(nextVX*nextVX + nextVY*nextVY + nextVZ*nextVZ) );
		
		nextVX *= limitation_factor;
		nextVY *= limitation_factor;
		nextVZ *= limitation_factor;

		//consider the rebound
		if(N > GRAVITY*4) factor = sqrt( (N*N) / (cNormal.x*cNormal.x + cNormal.y*cNormal.y + cNormal.z*cNormal.z) );
		else factor = 0.0f;

		nextVX += cNormal.x*factor*ELASTICITY;
		nextVY += cNormal.y*factor*ELASTICITY;
		nextVZ += cNormal.z*factor*ELASTICITY;
		
		float bounceForce = sqrt( (cNormal.x*factor*ELASTICITY)*(cNormal.x*factor*ELASTICITY) + (cNormal.y*factor*ELASTICITY)*(cNormal.y*factor*ELASTICITY) + (cNormal.z*factor*ELASTICITY)*(cNormal.z*factor*ELASTICITY) );
		if(bounceForce >= PLAYER_JUMP_SPEED) Sound.PlayBounce(1.0f);
		else if(bounceForce/PLAYER_JUMP_SPEED > 0.2f) Sound.PlayBounce(bounceForce/PLAYER_JUMP_SPEED);

		//update speed
		object.SetVel(nextVX, nextVY, nextVZ);
	}
}

//Output
void cGame::Reshape(int w, int h)
{
    glViewport (0, 0, (GLsizei)w, (GLsizei)h); //set the viewport to the current window specifications
    glMatrixMode (GL_PROJECTION); //set the matrix to projection

    glLoadIdentity ();
    gluPerspective (45.0, (GLfloat)w / (GLfloat)h, CAMERA_ZNEAR , CAMERA_ZFAR); //set the perspective (angle of sight, width, height, ,depth)
    glMatrixMode (GL_MODELVIEW); //set the matrix back to model
}

void cGame::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if ((state == STATE_RUN) && isMultiplayer)
		Player2.SetPos(playerPos_recv.x, playerPos_recv.y, playerPos_recv.z);
	
	//updates
	if(state != STATE_LIVELOSS) Lava.Update();
	Camera.Update(&Terrain, &Lava, Player.GetX(), Player.GetY(), Player.GetZ());
	ang = fmod(ang+2,360);

	 playerPos.x = Player.GetX(); playerPos.y = Player.GetY(); playerPos.z = Player.GetZ();

	Scene.Draw(&Data,&Terrain,&Lava,&Shader,playerPos);
	
	//draw keys
	for(unsigned int i=0; i<target_keys.size(); i++)
	{
		//color dye
		if(i==0) glColor3f(1.0f,0.0f,0.0f); //Red
		if(i==1) glColor3f(1.0f,1.0f,0.0f); //yellow
		if(i==2) glColor3f(0.0f,1.0f,0.0f); //green
		if(i==3) glColor3f(0.2f,0.2f,1.0f); //blue
		if(i==4) glColor3f(1.0f,0.0f,1.0f); //violet

		if(i==pickedkey_id) target_keys[i].DrawPicked(Player.GetX(),Player.GetY(),Player.GetZ(),Camera.GetYaw(),&Model,&Data,&Shader);
		else if (isMultiplayer && (i == recvKeyId))
			target_keys[i].DrawPicked(Player2.GetX(), Player2.GetY(), Player2.GetZ(), Camera.GetYaw(), &Model, &Data, &Shader);
		else if(target_keys[i].IsDeployed())
		{
			target_keys[i].DrawDeployed(columns[i].GetHoleX(),columns[i].GetHoleY(),columns[i].GetHoleZ(),columns[i].GetYaw(),&Model,&Data,&Shader);
			
			//ray color
			if(i==0) glColor4f(1.0f,0.0f,0.0f,0.4f); //red
			if(i==1) glColor4f(1.0f,1.0f,0.0f,0.4f); //yellow
			if(i==2) glColor4f(0.0f,1.0f,0.0f,0.4f); //green
			if(i==3) glColor4f(0.2f,0.2f,1.0f,0.4f); //blue
			if(i==4) glColor4f(1.0f,0.0f,1.0f,0.4f); //violet

			float r = ENERGY_BALL_RADIUS/2.0f; //energy ray radius
			int numrays = 6;
			glDisable(GL_LIGHTING);
			for(int j=0; j<numrays; j++)
			{
				float ang_rad = (ang+j*(360/numrays))*(PI/180);
				glEnable(GL_BLEND);
				glLineWidth(2.0);
				glBegin(GL_LINES);
					glVertex3f(columns[i].GetX()+cos(ang_rad)*r, columns[i].GetY()+COLUMN_HEIGHT+ENERGY_BALL_RADIUS+sin(ang_rad)*r, columns[i].GetZ());
					glVertex3f(Portal.GetReceptorX(i), Portal.GetReceptorY(i), Portal.GetZ());
				glEnd();
				glDisable(GL_BLEND);
			}
			glEnable(GL_LIGHTING);
		}
		else
		{
			float dist = sqrt( (Player.GetX()-target_keys[i].GetX())*(Player.GetX()-target_keys[i].GetX()) + (Player.GetZ()-target_keys[i].GetZ())*(Player.GetZ()-target_keys[i].GetZ()) );
			target_keys[i].DrawLevitating(&Shader,&Model,&Data, dist);
		}

		glColor4f(1,1,1,1);
	}

	for (unsigned int i = 0; i < white_keys.size(); i++) {
		glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
		float dist = sqrt((Player.GetX() - white_keys[i].GetX())*(Player.GetX() - white_keys[i].GetX()) + (Player.GetZ() - white_keys[i].GetZ())*(Player.GetZ() - white_keys[i].GetZ()));
		white_keys[i].DrawLevitating(&Shader, &Model, &Data, dist);
		glColor4f(1, 1, 1, 1);
	}

	//draw columns
	for(unsigned int i=0; i<columns.size(); i++) columns[i].Draw(&Shader,&Model,&Data,i);

	if(abs(Camera.GetZ()-Portal.GetZ()) < Camera.GetDistance())
	{
		//draw player
		Player.Draw(&Data,&Camera,&Lava,&Shader);

		//if(isMultiplayer)
			Player2.Draw(&Data, &Camera, &Lava, &Shader);

		//draw portal
		Portal.Draw(&Data,portal_activated,&Shader,&Model);
	}
	else
	{
		//draw portal
		Portal.Draw(&Data,portal_activated,&Shader,&Model);
	
		//draw player
		Player.Draw(&Data,&Camera,&Lava,&Shader);

		//if(isMultiplayer)
			Player2.Draw(&Data, &Camera, &Lava, &Shader);
	}

	//draw respawn points
	for(unsigned int i=0; i<respawn_points.size(); i++)
	{
		if(i==respawn_id) respawn_points[i].Draw(Data.GetID(IMG_CIRCLE_ON),true,&Shader);
		else respawn_points[i].Draw(Data.GetID(IMG_CIRCLE_OFF),false,&Shader);
	}

	std::string msg;
	if (isRecvChat) {
		state = STATE_CHAT;
		//chatBuffer = recvChatMsg.c_str();
	}
	switch (state) {
	case STATE_MENU: draw_start_menu();
		break;
	case STATE_MULTIPLAYER: msg = "Start a new Server? (Y/N)";
		draw_transition_screen(msg);
		break;
	case STATE_CONNECT: msg = "Connecting to server \n";
		msg.append(ipAddress);
		draw_transition_screen(msg);
		connectToServer();
		state = STATE_RUN;
		break;
	case STATE_CLIENT: msg = "Enter the IP address: ";
		msg.append(ipAddress);
		draw_transition_screen(msg);
		break;
	case STATE_WAIT_CONNECT: draw_transition_screen("Waiting for Client Connection");
		Sleep(3000);
		startServer();
		state = STATE_RUN;
		break;
	case STATE_CHAT: 
		if (isRecvChat)
			draw_chat_window(recvChatMsg);
		else
			draw_chat_window(chatBuffer);
		break;
	default: break;
	}

	if (state == STATE_GAME_OVER) {
		draw_game_over();
	}

	draw_score(score, lives);

	glutSwapBuffers();
}

void draw_string(std::string str)
{
	for (unsigned int i = 0; i<str.length(); i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *(str.begin() + i));
	}
}

void draw_start_menu()
{
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-50, 50, -50, 50);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);

	glColor3f(0.4, 0, 0.8);
	glBegin(GL_QUADS);
	glVertex2f(20.0f, 20.0f);
	glVertex2f(20.0f, -20.0f);
	glVertex2f(-20.0f, -20.0f);
	glVertex2f(-20.0f, 20.0f);
	glEnd();

	glLineWidth(3);
	glColor3f(0.3, 0.7, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(20.0f, 20.0f);
	glVertex2f(20.0f, -20.0f);
	glVertex2f(-20.0f, -20.0f);
	glVertex2f(-20.0f, 20.0f);
	glEnd();

	glLineWidth(4);
	glPushMatrix();
	glTranslatef(-25, 25, 0);
	glScalef(0.04, 0.04, 0);
	glColor3f(0.98, 0.98, 0.5);
	draw_string("Dino Run Multiplayer");
	glPopMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(5.0f, 15.0f);
	glVertex2f(5.0f, 5.0f);
	glVertex2f(-5.0f, 5.0f);
	glVertex2f(-5.0f, 15.0f);
	glEnd();

	glLineWidth(3);
	glColor3f(0.3, 0.7, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(5.0f, 15.0f);
	glVertex2f(5.0f, 5.0f);
	glVertex2f(-5.0f, 5.0f);
	glVertex2f(-5.0f, 15.0f);
	glEnd();

	glLineWidth(4);
	glPushMatrix();
	glTranslatef(-4, 10, 0);
	glScalef(0.03, 0.03, 0);
	glColor3f(1, 1, 1);
	draw_string("Start");
	glPopMatrix();

	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(10.0f, -10.0f);
	glVertex2f(10.0f, 0.0f);
	glVertex2f(-10.0f, 0.0f);
	glVertex2f(-10.0f, -10.0f);
	glEnd();

	glLineWidth(3);
	glColor3f(0.3, 0.7, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(10.0f, -10.0f);
	glVertex2f(10.0f, 0.0f);
	glVertex2f(-10.0f, 0.0f);
	glVertex2f(-10.0f, -10.0f);
	glEnd();

	glLineWidth(4);
	glPushMatrix();
	glTranslatef(-8, -4, 0);
	glScalef(0.03, 0.03, 0);
	glColor3f(1, 1, 1);
	draw_string("Multiplayer");
	glPopMatrix();


	glEnable(GL_LIGHTING);
	//to enable 3D
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void draw_transition_screen(std::string connectMsg) {
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-50, 50, -50, 50);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);

	glColor3f(0.4, 0, 0.8);
	glBegin(GL_QUADS);
	glVertex2f(40.0f, 20.0f);
	glVertex2f(40.0f, -20.0f);
	glVertex2f(-40.0f, -20.0f);
	glVertex2f(-40.0f, 20.0f);
	glEnd();

	glLineWidth(3);
	glColor3f(0.3, 0.7, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(40.0f, 20.0f);
	glVertex2f(40.0f, -20.0f);
	glVertex2f(-40.0f, -20.0f);
	glVertex2f(-40.0f, 20.0f);
	glEnd();

	glLineWidth(4);
	glPushMatrix();
	glTranslatef(-25, 25, 0);
	glScalef(0.04, 0.04, 0);
	glColor3f(0.98, 0.98, 0.5);
	draw_string("Dino Run Multiplayer");
	glPopMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glLineWidth(4);
	glPushMatrix();
	glTranslatef(-20, 10, 0);
	glScalef(0.025, 0.03, 0);
	glColor3f(1, 1, 1);
	draw_string(connectMsg);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	//to enable 3D
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}

void draw_chat_window(std::string chatMsg)
{
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-50, 50, -50, 50);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);

	glColor3f(0.4, 0, 0.8);
	glBegin(GL_QUADS);
	glVertex2f(40.0f, 20.0f);
	glVertex2f(40.0f, -20.0f);
	glVertex2f(-40.0f, -20.0f);
	glVertex2f(-40.0f, 20.0f);
	glEnd();

	glLineWidth(3);
	glColor3f(0.3, 0.7, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(40.0f, 20.0f);
	glVertex2f(40.0f, -20.0f);
	glVertex2f(-40.0f, -20.0f);
	glVertex2f(-40.0f, 20.0f);
	glEnd();

	glLineWidth(4);
	glPushMatrix();
	glTranslatef(-25, 25, 0);
	glScalef(0.03, 0.03, 0);
	glColor3f(1, 0, 0.5);

	if (!isRecvChat)
		draw_string("Type Message and press Enter");

	glPopMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glLineWidth(4);

	glPushMatrix();
	glTranslatef(-37, 0, 0);
	glScalef(0.025, 0.03, 0);
	glColor3f(1, 1, 1);
	draw_string(chatMsg);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	//to enable 3D
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void draw_score(int score, int lives)
{
	std::string scoreMsg = "Score: ";
	std::string livesMsg = "Lives: ";
	scoreMsg += std::to_string(score);
	livesMsg += std::to_string(lives);

	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -1.0, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);

	glColor3f(0.1, 0, 0.8);
	glBegin(GL_QUADS);
	glVertex2f(0.0, 0.0);
	glVertex2f(120.0, 0.0);
	glVertex2f(120.0, 80.0);
	glVertex2f(0.0, 80.0);
	glEnd();

	glLineWidth(3);
	glColor3f(0.3, 0.7, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.0, 0.0);
	glVertex2f(120.0, 0.0);
	glVertex2f(120.0, 80.0);
	glVertex2f(0.0, 80.0);
	glEnd();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glLineWidth(4);

	glPushMatrix();
	glTranslatef(0, 37, 0);
	glScalef(0.15, -0.3, 0);
	glColor3f(1, 0, 0);
	draw_string(scoreMsg);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 67, 0);
	glScalef(0.15, -0.3, 0);
	glColor3f(1, 0, 0);
	draw_string(livesMsg);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	//to enable 3D
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void draw_game_over()
{
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-50, 50, -50, 50);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);

	glColor3f(0.4, 0, 0.8);
	glBegin(GL_QUADS);
	glVertex2f(60.0f, 10.0f);
	glVertex2f(60.0f, -10.0f);
	glVertex2f(-60.0f, -10.0f);
	glVertex2f(-60.0f, 10.0f);
	glEnd();

	glLineWidth(3);
	glColor3f(0.3, 0.7, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(60.0f, 10.0f);
	glVertex2f(60.0f, -10.0f);
	glVertex2f(-60.0f, -10.0f);
	glVertex2f(-60.0f, 10.0f);
	glEnd();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glLineWidth(4);
	glPushMatrix();
	glTranslatef(-37, 0, 0);
	glScalef(0.025, 0.03, 0);
	glColor3f(1, 1, 1);
	draw_string("Game Over");
	glPopMatrix();

	glEnable(GL_LIGHTING);
	//to enable 3D
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
int connectToServer()
{
	sockaddr_in ser;
	sockaddr addr;
	WSADATA data;
	DWORD poll;
	int res;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(123);                    //Set the port
	ser.sin_addr.s_addr = inet_addr(ipAddress);      //Set the address we want to connect to

	memcpy(&addr, &ser, sizeof(SOCKADDR_IN));

	res = WSAStartup(MAKEWORD(1, 1), &data);      //Start Winsock
	std::cout << "\n\nWSAStartup"
		<< "\nVersion: " << data.wVersion
		<< "\nDescription: " << data.szDescription
		<< "\nStatus: " << data.szSystemStatus;
	std::cout << "\n";
	if (res != 0)
		s_cl("WSAStarup failed", WSAGetLastError());

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);       //Create the socket
	if (sock == INVALID_SOCKET)
		s_cl("Invalid Socket ", WSAGetLastError());
	else if (sock == SOCKET_ERROR)
		s_cl("Socket Error)", WSAGetLastError());
	else
		std::cout << "Socket Established\n";

	res = connect(sock, &addr, sizeof(addr));               //Connect to the server
	if (res != 0)
	{
		s_cl("SERVER UNAVAILABLE", res);
		exit(0);
	}
	else
	{
		std::cout << "\nConnected to Server: ";
		memcpy(&ser, &addr, sizeof(SOCKADDR));
	}

	CreateThread(NULL, 0, receive_cmd_client, (LPVOID)sock, 0, &poll);
}

DWORD WINAPI receive_cmd_client(LPVOID lpParam) {
	char RecvdData[100] = "";
	int ret, res;
	char sendPosition[100];
	char sendKey[100];
	char sendChat[100];
	char keyDeployed[100];
	char keyLost[100];
	SOCKET current_server = (SOCKET)lpParam;
	std::cout << "Client Thread Created\n";

	while (true)
	{
		//Sleep(1000);
		ret = recv(current_server, RecvdData, sizeof(RecvdData), 0);
		if (ret > 0)
		{
			std::cout << "\n From Server\n" << RecvdData;

			if (strstr(RecvdData, "OP_POSITION")) {
				char * ret = strchr(RecvdData, ':');
				char * token = strtok(ret + 1, ",");
				float newPos[3];
				int i = 0;
				/* walk through other tokens */
				while (token != NULL) {
					newPos[i] = atof(token);
					token = strtok(NULL, ",");
					i++;
				}
				playerPos_recv.x = newPos[0];
				playerPos_recv.y = newPos[1];
				playerPos_recv.z = newPos[2];
			}
			if (strstr(RecvdData, "OP_KEY")) {
				char * ret = strchr(RecvdData, ':');
				printf("key is:%s\n", ret + 1);
				recvKeyId = atoi(ret + 1);
			}
			if (strstr(RecvdData, "OP_CHAT")) {
				char * ret = strchr(RecvdData, ':');
				printf("chat is:%s\n", ret + 1);
				recvChatMsg = std::string(ret);
				isRecvChat = true;
			}
			if (strstr(RecvdData, "OP_KEYDEPLOY")) {
				recvKeyDeployed = true;
			}
			if (strstr(RecvdData, "OP_KEYLOST")) {
				recvKeyLost = true;
				recvKeyId = -1;
			}

		}
		strcpy(RecvdData, "");
		strcpy(sendChat, "");
		strcpy(sendPosition, "");
		strcpy(sendKey, "");

		strcpy(sendPosition, "");
		strcpy(sendChat, "");
		strcpy(sendKey, "");

		sprintf(sendPosition, "OP_POSITION:%f,%f,%f", playerPos.x, playerPos.y, playerPos.z);
		Sleep(100);
		res = send(current_server, sendPosition, sizeof(sendPosition), 0);

		if (sendKeyDeployed) {
			sprintf(keyDeployed, "OP_KEYDEPLOY");
			sendKeyDeployed = false;
		}
		if (sendKeyLost) {
			sprintf(keyLost, "OP_KEYLOST");
			sendKeyLost = false;
		}

		if (sendKeyId != -1) {
			sprintf(sendKey, "OP_KEY:%d", sendKeyId);
			Sleep(100);
			res = send(current_server, sendKey, sizeof(sendKey), 0);
		}
		if (isSendChat) {
			sprintf(sendChat, "OP_CHAT:%s", sendChatMsg.c_str());
			std::cout << "\n Sending chat msg --" << sendChat;
			Sleep(100);
			res = send(current_server, sendChat, sizeof(sendChat), 0);
			isSendChat = false;
		}


		if (res == 0)
		{
			printf("\nSERVER terminated connection\n");
			closesocket(current_server);
			break;
		}
		else if (res == SOCKET_ERROR)
		{
			//-1 == send error
			printf("Socket error,,,,didnt send\n");
			//Sleep(40);
			s_handle(res);
			break;
		}


	}
	return 1;
}

void s_handle(int s)
{
	if (sock)
		closesocket(sock);
	if (client)
		closesocket(client);
	WSACleanup();
	//Sleep(1000);
	std::cout << "EXIT SIGNAL :" << s;
	//exit(0);
}

void s_cl(char *a, int x)
{
	std::cout << a;
	s_handle(x + 1000);
}

void startServer()
{
	int i = 0;
	printf("Starting up multi-threaded TCP server\r\n");

	// our masterSocket(socket that listens for connections)
	SOCKET sock;

	// for our thread
	DWORD thread;

	WSADATA wsaData;
	sockaddr_in server;

	// start winsock
	int ret = WSAStartup(0x101, &wsaData); // use highest version of winsock avalible

	if (ret != 0)
	{
		printf("server didnt startup\n");
		exit(0);
	}

	// fill in winsock struct ... 
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(123); // listen on telnet port 23

	// create our socket
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		printf("invalid socket\n");
		exit(0);
	}

	// bind our socket to a port(port 123) 
	if (bind(sock, (sockaddr*)&server, sizeof(server)) != 0)
	{
		printf("binding error\n");
		exit(0);
	}

	// listen for a connection  
	if (listen(sock, 5) != 0)
	{
		printf("listen error\n");
		exit(0);
	}
	printf("waiting for client to connect\n");
	// socket that we snedzrecv data on
	SOCKET client;

	sockaddr_in from;
	int fromlen = sizeof(from);

	// loop forever 

	// accept connections
	client = accept(sock, (struct sockaddr*)&from, &fromlen);
	printf("Client connected\r\n"); 

	CreateThread(NULL, 0, receive_cmd_server, (LPVOID)client, 0, &thread);

}

DWORD WINAPI receive_cmd_server(LPVOID lpParam)
{
	printf("Server thread created\r\n");

	// set our socket to the socket passed in as a parameter   
	SOCKET current_client = (SOCKET)lpParam;

	// buffer to hold our recived data
	char buf[100];
	// buffer to hold our sent data
	char sendPosition[100];
	char sendKey[100];
	char sendEvent[100];
	char sendChat[1024];
	char keyDeployed[100];
	char keyLost[100];
	// for error checking 
	int res;

	// our recv loop
	while (true)
	{
		//		Sleep(1000);
		res = recv(current_client, buf, sizeof(buf), 0); // recv cmds

		if (res == 0)
		{
			MessageBox(0, "error", "error", MB_OK);
			closesocket(current_client);
			ExitThread(0);
		}
		else {
			std::cout << "From Client: " << buf;
			std::cout << "\n";

			if (strstr(buf, "OP_POSITION")) {
				char * ret = strchr(buf, ':');
				char * token = strtok(ret + 1, ",");
				float newPos[3];
				int i = 0;
				/* walk through other tokens */
				while (token != NULL) {
					newPos[i] = atof(token);
					token = strtok(NULL, ",");
					i++;
				}
				playerPos_recv.x = newPos[0];
				playerPos_recv.y = newPos[1];
				playerPos_recv.z = newPos[2];
			}
			if (strstr(buf, "OP_KEY")) {
				char * ret = strchr(buf, ':');
				recvKeyId = atoi(ret + 1);
				printf("key is:%s\n", ret + 1);
			}
			if (strstr(buf, "OP_CHAT")) {
				char * ret = strchr(buf, ':');
				printf("chat is:%s\n", ret + 1);
				recvChatMsg = std::string(ret);
				std::cout << "\n Received chat :" << recvChatMsg;
				isRecvChat = true;
			}
			if (strstr(buf, "OP_KEYDEPLOY")) {
				recvKeyDeployed = true;
			}
			if (strstr(buf, "OP_KEYLOST")) {
				recvKeyLost = true;
				recvKeyId = -1;
			}

			strcpy(buf, "");
		}

		sprintf(sendPosition, "OP_POSITION:%f,%f,%f", playerPos.x, playerPos.y, playerPos.z);
		Sleep(100);
		send(current_client, sendPosition, sizeof(sendPosition), 0);

		if (sendKeyDeployed) {
			sprintf(keyDeployed, "OP_KEYDEPLOY");
			sendKeyDeployed = false;
		}
		if (sendKeyLost) {
			sprintf(keyLost, "OP_KEYLOST");
			sendKeyLost = false;
		}

		if (sendKeyId != -1) {
			sprintf(sendKey, "OP_KEY:%d", sendKeyId);
			Sleep(100);
			send(current_client, sendKey, sizeof(sendKey), 0);
		}
		if (isSendChat) {
			sprintf(sendChat, "OP_CHAT:%s", sendChatMsg.c_str());
			Sleep(100);
			res = send(current_client, sendChat, sizeof(sendChat), 0);
			isSendChat = false;
		}

		// clear buffers
		strcpy(sendPosition, "");
		strcpy(sendKey, "");
		strcpy(sendChat, "");
		strcpy(buf, "");
	}
}