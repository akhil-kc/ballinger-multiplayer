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
#include <fstream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
#include "cGame.h"
#define DEFAULT_BUFLEN 512
const int SERVER_PORT = 25001;
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27014"
int visits = 0; /*the number client connect*/

//Delete console
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

cGame Game;

void AppReshape(int w, int h)
{
	Game.Reshape(w,h);
}
void AppRender()
{
	Game.Render();
}
void AppKeyboard(unsigned char key, int x, int y)
{
	Game.ReadKeyboard(key,x,y,true);
}
void AppKeyboardUp(unsigned char key, int x, int y)
{
	Game.ReadKeyboard(key,x,y,false);
}
void AppSpecialKeys(int key, int x, int y)
{
	Game.ReadSpecialKeyboard(key,x,y,true);
}
void AppSpecialKeysUp(int key, int x, int y)
{
	Game.ReadSpecialKeyboard(key,x,y,false);
}
void AppMouse(int button, int state, int x, int y)
{
	Game.ReadMouse(button,state,x,y);
}
void AppMouseMotion(int x, int y)
{
	Game.ReadMouseMotion(x,y);
}
void AppIdle()
{
	if(!Game.Loop()) exit(0);
}

// our thread for recving commands
DWORD WINAPI receive_cmds(LPVOID lpParam)
{
	printf("thread created\r\n");

	// set our socket to the socket passed in as a parameter   
	SOCKET current_client = (SOCKET)lpParam;

	// buffer to hold our recived data
	char buf[100];
	// buffer to hold our sent data
	char sendData[100];
	// for error checking 
	int res;

	// our recv loop
	while (true)
	{
		res = recv(current_client, buf, sizeof(buf), 0); // recv cmds

		//Sleep(10);

		if (res == 0)
		{
			MessageBox(0, "error", "error", MB_OK);
			closesocket(current_client);
			ExitThread(0);
		}
		else {
			std::cout << "From Client: " << buf;
			std::cout << "\n";
			strcpy(buf, "");
		}

		/*if (strstr(buf, "hello"))
		{ // greet this user
			printf("\nrecived hello cmd");

			strcpy(sendData, "hello, greetz from KOrUPt\n");
			Sleep(10);
			send(current_client, sendData, sizeof(sendData), 0);
		}
		else if (strstr(buf, "bye"))
		{ // dissconnected this user
			printf("\nrecived bye cmd\n");

			strcpy(sendData, "cya\n");
			Sleep(10);
			send(current_client, sendData, sizeof(sendData), 0);

			// close the socket associted with this client and end this thread
			closesocket(current_client);
			ExitThread(0);
		}
		else*/
		
		sprintf(sendData, "Player's coordinates %f %f %f", Game.Player.GetX(), Game.Player.GetY(), Game.Player.GetZ());
			//strcpy(sendData, "Invalid cmd\n");
		Sleep(10);
		send(current_client, sendData, sizeof(sendData), 0);
			
		// clear buffers
		strcpy(sendData, "");
		strcpy(buf, "");
	}
}


void main(int argc, char** argv)
{
	int res_x,res_y,pos_x,pos_y;
	int x;
	bool client_flag = false;
	bool server_flag = false;
	std::cout << "In main function";

	/*********************server part**************************************/

	int i = 0;
	printf("Starting up multi-threaded TCP server by KOrUPt\r\n");

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

	// socket that we snedzrecv data on
	SOCKET client;

	sockaddr_in from;
	int fromlen = sizeof(from);

	// loop forever 
	
	// accept connections
	client = accept(sock, (struct sockaddr*)&from, &fromlen);
	printf("Client connected\r\n");

	
	//GLUT initialization
	glutInit(&argc, argv);

	//RGBA with double buffer
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);

	//Create centered window
	res_x = glutGet(GLUT_SCREEN_WIDTH);
	res_y = glutGet(GLUT_SCREEN_HEIGHT);
	pos_x = (res_x >> 1) - (SCREEN_WIDTH >> 1);
	pos_y = (res_y >> 1) - (SCREEN_HEIGHT >> 1);

	glutInitWindowPosition(pos_x, pos_y);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("The Ballenger Server");
	//glutFullScreen();

		/*glutGameModeString("800x600:32");
	glutEnterGameMode();*/

	//Make the default cursor disappear
	glutSetCursor(GLUT_CURSOR_NONE);

	//Register callback functions
	glutReshapeFunc(AppReshape);
	glutDisplayFunc(AppRender);
	glutKeyboardFunc(AppKeyboard);
	glutKeyboardUpFunc(AppKeyboardUp);
	glutSpecialFunc(AppSpecialKeys);
	glutSpecialUpFunc(AppSpecialKeysUp);
	glutMouseFunc(AppMouse);
	glutMotionFunc(AppMouseMotion);
	glutPassiveMotionFunc(AppMouseMotion);
	glutIdleFunc(AppIdle);

	//GLEW initialization
	GLint GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		printf("ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	//Game initializations
	Game.Init(1);

	// create our recv_cmds thread and parse client socket as a parameter
	CreateThread(NULL, 0, receive_cmds, (LPVOID)client, 0, &thread);

	//Application loop
	glutMainLoop();
	
	// shutdown winsock
	closesocket(sock);
	WSACleanup();
}
