#include "cGame.h"
void draw_start_menu();
void draw_transition_screen(std::string connectMsg);

cGame::cGame(void) {}
cGame::~cGame(void){}

char ipAddress[100];
int ipSize = 0;
bool cGame::Init(int lvl)
{
	bool res = true;
	noclip = false;
	portal_activated = false;
	time = ang = 0.0f;
	noclipSpeedF = 1.0f; 
	level = lvl;
	//state = STATE_RUN;
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
		Player.SetPos(respawn_points[respawn_id].GetX(),respawn_points[respawn_id].GetY()+RADIUS,respawn_points[respawn_id].GetZ());
		state = STATE_RUN;
	}
	else if(state == STATE_ENDGAME)
	{
		res=false;
	}
	else {
		res = Process();
		if (res) Render();
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
		if ((key == 'Y' || key == 'y') && !press)
			// Start Server here. Remove below line
			state = STATE_MULTIPLAYER;
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

	//la cantidad desplazada en pixeles
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

//Process
bool cGame::Process()
{
	bool res=true;
	
	//Process Input
	if(keys[27])	res=false;

	if (state == STATE_MENU) {
		if (keys['s']) {
			state = STATE_RUN;
			Render();
			//	return;
		}
		else if (keys['m']) {
			state = STATE_MULTIPLAYER;
			Render();
		}
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

			//comprueba si el player muere
			if (Player.GetY() <= Lava.GetHeight() + RADIUS)
			{
				Player.SetY(Lava.GetHeight() + RADIUS);
				Player.SetVel(0.0f, 0.0f, 0.0f);
				pickedkey_id = -1;
				state = STATE_LIVELOSS;
				Sound.Play(SOUND_SWISH);
			}

			Coord P; P.x = Player.GetX(); P.y = Player.GetY(); P.z = Player.GetZ();
			float r = RADIUS;

			//comprueba si el player entra en algun Respawn Point
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

			//comprueba si el player recoge alguna llave
			if (pickedkey_id == -1)
			{
				for (unsigned int i = 0; i < target_keys.size(); i++)
				{
					if (!target_keys[i].IsDeployed())
					{
						Coord K; K.x = target_keys[i].GetX(); K.y = target_keys[i].GetY(); K.z = target_keys[i].GetZ();
						if (sqrt((P.x - K.x)*(P.x - K.x) + (P.y - K.y)*(P.y - K.y) + (P.z - K.z)*(P.z - K.z)) <= RADIUS * 2)
						{
							pickedkey_id = i;
							Sound.Play(SOUND_PICKUP);
						}
					}
				}
			}

			//comprueba si el player llega con una llave a su respectiva columna
			if (pickedkey_id != -1)
			{
				if (columns[pickedkey_id].InsideGatheringArea(P.x, P.y, P.z))
				{
					Sound.Play(SOUND_UNLOCK);
					Sound.Play(SOUND_ENERGYFLOW);
					target_keys[pickedkey_id].Deploy();
					pickedkey_id = -1;
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

			//comprueba si el player atraviesa el portal estando activado
			if (portal_activated)
			{
				if (Portal.InsidePortal(P.x, P.y, P.z, RADIUS))
				{
					if ((initial_z - Portal.GetZ() <= 0.0f && Player.GetZ() - Portal.GetZ() >= 0.0f) ||
						(initial_z - Portal.GetZ() >= 0.0f && Player.GetZ() - Portal.GetZ() <= 0.0f)) state = STATE_ENDGAME;
				}
			}
		}

		//limpio buffer de sonidos
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

	//actualizo angulos de rotacion por movimiento
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
		float rz,rx; //angulos de rotacion
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

			//Transformamos las fuerzas definidas en el sistema de coordenadas de OpenGL al sistema de coordenadas definido por cnormal(eje y) 
			G1.x = cos(-rz)*G.x - sin(-rz)*G.y;
			G1.y = cos(-rx)*sin(-rz)*G.x + cos(-rx)*cos(-rz)*G.y - sin(-rx)*G.z;
			G1.z = sin(-rx)*sin(-rz)*G.x + sin(-rx)*cos(-rz)*G.y + cos(-rx)*G.z;

			F1.x = cos(-rz)*F.x - sin(-rz)*F.y;
			F1.y = cos(-rx)*sin(-rz)*F.x + cos(-rx)*cos(-rz)*F.y - sin(-rx)*F.z;
			F1.z = sin(-rx)*sin(-rz)*F.x + sin(-rx)*cos(-rz)*F.y + cos(-rx)*F.z;
			
			//consideramos la fuerza normal para un unico triangulo
			float cN = 0.0f;
			if (G1.y < 0.0f) {cN -= G1.y; G1.y = 0.0f;}
			if (F1.y < 0.0f) {cN -= F1.y; F1.y = 0.0f;}
			N += cN; //actualizo la fuerza normal global

			//actualizo la fuerza de cnormal global
			cNormal.x += cnormals[i].x;
			cNormal.y += cnormals[i].y;
			cNormal.z += cnormals[i].z;

			//consideramos la posible friccion
			if(cN > 0.0f && abs(F1.x) + abs(F1.z) > 0.0f)
			{
				factor = sqrt( ((FRICTION*cN)*(FRICTION*cN)) / (F1.x*F1.x + F1.z*F1.z) );

				if(abs(F1.x) < abs(F1.x*factor)) F1.x = 0.0f;
				else F1.x -= F1.x*factor;

				if(abs(F1.z) < abs(F1.z*factor)) F1.z = 0.0f;
				else F1.z -= F1.z*factor;
			}
			
			//volvemos a Transformar las fuerzas del sistema de coordenadas de cnormal(eje y) al sistema de coordenadas de OpenGL
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

		//consideramos el rebote
		if(N > GRAVITY*4) factor = sqrt( (N*N) / (cNormal.x*cNormal.x + cNormal.y*cNormal.y + cNormal.z*cNormal.z) );
		else factor = 0.0f;

		nextVX += cNormal.x*factor*ELASTICITY;
		nextVY += cNormal.y*factor*ELASTICITY;
		nextVZ += cNormal.z*factor*ELASTICITY;
		
		float bounceForce = sqrt( (cNormal.x*factor*ELASTICITY)*(cNormal.x*factor*ELASTICITY) + (cNormal.y*factor*ELASTICITY)*(cNormal.y*factor*ELASTICITY) + (cNormal.z*factor*ELASTICITY)*(cNormal.z*factor*ELASTICITY) );
		if(bounceForce >= PLAYER_JUMP_SPEED) Sound.PlayBounce(1.0f);
		else if(bounceForce/PLAYER_JUMP_SPEED > 0.2f) Sound.PlayBounce(bounceForce/PLAYER_JUMP_SPEED);

		//actualizamos velocidad
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

	//updates
	if(state != STATE_LIVELOSS) Lava.Update();
	Camera.Update(&Terrain, &Lava, Player.GetX(), Player.GetY(), Player.GetZ());
	ang = fmod(ang+2,360);

	Coord playerPos; playerPos.x = Player.GetX(); playerPos.y = Player.GetY(); playerPos.z = Player.GetZ();

	//draw scene(terrain + lava + skybox)
	Scene.Draw(&Data,&Terrain,&Lava,&Shader,playerPos);
	
	//draw keys
	for(unsigned int i=0; i<target_keys.size(); i++)
	{
		//color dye
		if(i==0) glColor3f(1.0f,0.0f,0.0f); //rojo
		if(i==1) glColor3f(1.0f,1.0f,0.0f); //amarillo
		if(i==2) glColor3f(0.0f,1.0f,0.0f); //verde
		if(i==3) glColor3f(0.2f,0.2f,1.0f); //azul
		if(i==4) glColor3f(1.0f,0.0f,1.0f); //violeta

		if(i==pickedkey_id) target_keys[i].DrawPicked(Player.GetX(),Player.GetY(),Player.GetZ(),Camera.GetYaw(),&Model,&Data,&Shader);
		else if(target_keys[i].IsDeployed())
		{
			target_keys[i].DrawDeployed(columns[i].GetHoleX(),columns[i].GetHoleY(),columns[i].GetHoleZ(),columns[i].GetYaw(),&Model,&Data,&Shader);
			
			//ray color
			if(i==0) glColor4f(1.0f,0.0f,0.0f,0.4f); //rojo
			if(i==1) glColor4f(1.0f,1.0f,0.0f,0.4f); //amarillo
			if(i==2) glColor4f(0.0f,1.0f,0.0f,0.4f); //verde
			if(i==3) glColor4f(0.2f,0.2f,1.0f,0.4f); //azul
			if(i==4) glColor4f(1.0f,0.0f,1.0f,0.4f); //violeta

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
	
	//draw columns
	for(unsigned int i=0; i<columns.size(); i++) columns[i].Draw(&Shader,&Model,&Data,i);

	if(abs(Camera.GetZ()-Portal.GetZ()) < Camera.GetDistance())
	{
		//draw player
		Player.Draw(&Data,&Camera,&Lava,&Shader);

		//draw portal
		Portal.Draw(&Data,portal_activated,&Shader,&Model);
	}
	else
	{
		//draw portal
		Portal.Draw(&Data,portal_activated,&Shader,&Model);
	
		//draw player
		Player.Draw(&Data,&Camera,&Lava,&Shader);
	}

	//draw respawn points
	for(unsigned int i=0; i<respawn_points.size(); i++)
	{
		if(i==respawn_id) respawn_points[i].Draw(Data.GetID(IMG_CIRCLE_ON),true,&Shader);
		else respawn_points[i].Draw(Data.GetID(IMG_CIRCLE_OFF),false,&Shader);
	}
	std::string msg;

	switch (state) {
	case STATE_MENU: draw_start_menu();
		break;
	case STATE_MULTIPLAYER: msg = "Start a new Server? (Y/N)";
		draw_transition_screen(msg);
		break;
	case STATE_CLIENT: msg = "Enter the IP address: ";
		msg.append(ipAddress);
		draw_transition_screen(msg);
		break;
	case STATE_CONNECT: msg = "Connecting to server \n";
		msg.append(ipAddress);
		draw_transition_screen(msg);
		break;
	default: break;
	}

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
	draw_string("Ballenger Multiplayer");
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
	draw_string("Ballenger Multiplayer");
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