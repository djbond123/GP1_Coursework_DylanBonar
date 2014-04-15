/*
=================
main.cpp
Main entry point for the Card application
=================
*/

#include "GameConstants.h"
#include "GameResources.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"
#include "cBambo.h"
#include "cSprite.h"
#include "cExplosion.h"
#include "cXAudio.h"
#include "cD3DXFont.h"
#include <iostream>
#include <sstream>


using namespace std;

HINSTANCE hInst; // global handle to hold the application instance
HWND wndHandle; // global variable to hold the window handle

// Get a reference to the DirectX Manager
static cD3DManager* d3dMgr = cD3DManager::getInstance();

// Get a reference to the DirectX Sprite renderer Manager 
static cD3DXSpriteMgr* d3dxSRMgr = cD3DXSpriteMgr::getInstance();

D3DXVECTOR2 bamboTrans = D3DXVECTOR2(300,300);

vector<cBambo*> aBambo;
vector<cBambo*>::iterator iter;
vector<cBambo*>::iterator index;

vector<cBambo*> retrybuttons;
vector<cBambo*>::iterator retrybuttoniter;
vector<cBambo*>::iterator retrybuttonindex;

vector<cBambo*> introbuttons;
vector<cBambo*>::iterator introbuttoniter;
vector<cBambo*>::iterator introbuttonindex;


RECT clientBounds;

TCHAR szTempOutput[30];

bool gHit = false;
int gBamboLeft;
char gBamboEatenStr[50];

float gTime = 10;

cXAudio musicSound;

D3DXVECTOR3 expPos;
list<cExplosion*> gExplode;

cXAudio gExplodeSound;
int score;
cD3DXTexture* bamboTextures[4];
cD3DXTexture* buttonTextures[3];
cD3DXTexture* introbuttonTextures[2];
char* bamboTxtres[] = {"Images\\Bambo1.png","Images\\Bambo2.png","Images\\Bambo3.png","Images\\explosion.png"};
char* buttonTxtres[] = {"Images\\RetryTitle.png","Images\\YesButton.png","Images\\NoButton.png"};
char* introbuttonTxtres[]= {"Images\\StartButton.png","Images\\QuitButton.png"};
string gameScene = "GAME";
string endScene = "END";
string introScene = "INTRO";
string currentScene = introScene;


bool surfaceUpdate;
/*
==================================================================
* LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
* LPARAM lParam)
* The window procedure
==================================================================
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check any available messages from the queue
	switch (message)
	{
		case WM_LBUTTONDOWN:
			{
				POINT mouseXY;
				mouseXY.x = LOWORD(lParam);
				mouseXY.y = HIWORD(lParam);
				
				expPos = D3DXVECTOR3((float)mouseXY.x,(float)mouseXY.y, 0.0f);
				if(currentScene == gameScene) 
				{
					iter = aBambo.begin();
					while (iter != aBambo.end() && !gHit)
					{
						if ( (*iter)->insideRect((*iter)->getBoundingRect(),mouseXY))
						{
							OutputDebugString("Munch!\n");
							gHit = true;
							expPos = (*iter)->getSpritePos();
							gExplode.push_back(new cExplosion(expPos,bamboTextures[3]));
						
							gExplodeSound.playSound(L"Sounds\\munchingQuieter.wav",false);
							iter = aBambo.erase(iter);
							gBamboLeft--;
							score++;
							sprintf_s( gBamboEatenStr, 50, "Bambo Eaten : %d", score);
							gTime += 0.5f;
				
						}
					
						else
						{
							++iter;
						}
					}
				
					gHit = false;
					return 0;
				
			
				}
					/*
					=========================
					* Here is where the buttons will be controlled
					=========================
					*/
					// Check for retry buttons if it is that scene, check id of buttons to determine where to go
					if(currentScene == endScene)
					{
						retrybuttoniter = retrybuttons.begin();
						while (retrybuttoniter != retrybuttons.end() && !gHit)
						{
							if ( (*retrybuttoniter)->insideRect((*retrybuttoniter)->getBoundingRect(),mouseXY))
							{
								// If id = 1 then we accept to retry
								if((*retrybuttoniter)->GetId() == 1)
								{
							
									//Resets all stats back to default
									OutputDebugString("Click!\n");
									currentScene = gameScene;
									gTime = 10.0f;
									sprintf_s( gBamboEatenStr, 50, "BEGIN!: 0", gBamboLeft);
									score = 0;
									return 0;

								}
								// if id = 2 we decline and exit game
								else if((*retrybuttoniter)->GetId() == 2) 
								{
									PostQuitMessage(0);
									return 0;
								}
							
							}
							else
							{
								++retrybuttoniter;
							}
						}
					}

					if(currentScene == introScene)
					{
						introbuttoniter = introbuttons.begin();
						while (introbuttoniter != introbuttons.end() && !gHit)
						{
							if ( (*introbuttoniter)->insideRect((*introbuttoniter)->getBoundingRect(),mouseXY))
							{
								// If id = 1 then we accept to startzx
								if((*introbuttoniter)->GetId() == 0) 
								{
									//Resets all stats back to default
									OutputDebugString("Click!\n");
									currentScene = gameScene;
									surfaceUpdate = true;
									gTime = 10.0f;
									sprintf_s( gBamboEatenStr, 50, "BEGIN!: 0", gBamboLeft);
									score = 0;
									return 0;
								}
								// if id = 2 we decline and exit game
								else if((*introbuttoniter)->GetId() == 1) 
								{
									PostQuitMessage(0);
									return 0;
								}
							
							}
							else
							{
								++introbuttoniter;
							}
						}

				gHit = false;
				return 0;
			
					}
			}
			case WM_CLOSE:
			{
				// Exit the Game
				PostQuitMessage(0);
				 return 0;
			}

			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
		}
	
	// Always return the message to the default window
	// procedure for further processing
	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
==================================================================
* bool initWindow( HINSTANCE hInstance )
* initWindow registers the window class for the application, creates the window
==================================================================
*/
bool initWindow( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	// Fill in the WNDCLASSEX structure. This describes how the window
	// will look to the system
	wcex.cbSize = sizeof(WNDCLASSEX); // the size of the structure
	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcex.hInstance = hInstance; // handle to the application instance
	wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MyWindowIcon)); // icon to associate with the application
	wcex.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_GUNSIGHT));// the default cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcex.lpszMenuName = NULL; // the resource name for the menu
	wcex.lpszClassName = "Bambo"; // the class name being created
	wcex.hIconSm = LoadIcon(hInstance,"Balloon.ico"); // the handle to the small icon

	RegisterClassEx(&wcex);
	// Create the window
	wndHandle = CreateWindow("Bambo",			// the window class to use
							 "Panda Pals",			// the title bar text
							WS_OVERLAPPEDWINDOW,	// the window style
							CW_USEDEFAULT, // the starting x coordinate
							CW_USEDEFAULT, // the starting y coordinate
							800, // the pixel width of the window
							600, // the pixel height of the window
							NULL, // the parent window; NULL for desktop
							NULL, // the menu for the application; NULL for none
							hInstance, // the handle to the application instance
							NULL); // no values passed to the window
	// Make sure that the window handle that is created is valid
	if (!wndHandle)
		return false;
	// Display the window on the screen
	ShowWindow(wndHandle, SW_SHOW);
	UpdateWindow(wndHandle);
	return true;
}

/*
==================================================================
// This is winmain, the main entry point for Windows applications
==================================================================
*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	// Initialize the window
	if ( !initWindow( hInstance ) )
		return false;
	// called after creating the window
	if ( !d3dMgr->initD3DManager(wndHandle) )
		return false;
	if ( !d3dxSRMgr->initD3DXSpriteMgr(d3dMgr->getTheD3DDevice()))
		return false;

	// Grab the frequency of the high def timer
	__int64 freq = 0;				// measured in counts per second;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	float sPC = 1.0f / (float)freq;			// number of seconds per count

	__int64 currentTime = 0;				// current time measured in counts per second;
	__int64 previousTime = 0;				// previous time measured in counts per second;

	float numFrames   = 0.0f;				// Used to hold the number of frames
	float timeElapsed = 0.0f;				// cumulative elapsed time

	GetClientRect(wndHandle,&clientBounds);

	float fpsRate = 1.0f/25.0f;

	D3DXVECTOR3 abamboPos;
	D3DXVECTOR3 aButtonPos;

	musicSound.playSound(L"Sounds\\backsound.wav",true);

	sprintf_s( gBamboEatenStr, 50, "BEGIN!: %d", gBamboLeft);


	//====================================================================
	// Load four textures for the bambo; yellow, green, red & explosion
	//====================================================================
	for (int txture = 0; txture < 4; txture++)
	{
		bamboTextures[txture] = new cD3DXTexture(d3dMgr->getTheD3DDevice(), bamboTxtres[txture]);
	}

	for (int txture = 0; txture <3; txture++)
	{
		buttonTextures[txture] = new cD3DXTexture(d3dMgr->getTheD3DDevice(), buttonTxtres[txture]);
	}

	for (int txture = 0; txture <3; txture++)
	{
		introbuttonTextures[txture] = new cD3DXTexture(d3dMgr->getTheD3DDevice(), introbuttonTxtres[txture]);
	}

	// Initial starting position for bambo
	D3DXVECTOR3 bamboPos;
	D3DXVECTOR3 buttonPos;

	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );

	/* generate random number of bambo */
	int numBambo = 1; //(rand() % 10 + 1);
	gBamboLeft = numBambo;

	float spritePositions[10] = 
		{50.0f,  100.0f, 150.0f, 200.0f,
		250.0f, 300.0f, 350.0f, 400.0f,
		450.0f, 500.0f};
	
	//These don't need to wrapped in loops, this method is called on the load up 
	// so wrapping them in loops won't allow them to get a chance to generate
		for(int loop = 0; loop < numBambo; loop++)
		{
			bamboPos = D3DXVECTOR3((spritePositions[(rand() % 10 +1)]) ,(spritePositions[(rand() % 10 + 1)]),0);
			aBambo.push_back(new cBambo());
			aBambo[loop]->setSpritePos(bamboPos);
			aBambo[loop]->setTranslation(D3DXVECTOR2(0.0f,0.0f));
			aBambo[loop]->setTexture(bamboTextures[(loop % 3)]);
		}
	
		for(int loop = 0; loop < 3; loop++)
		{
			buttonPos = D3DXVECTOR3( 150.0f + (200*loop) , 300.0f,0);
			retrybuttons.push_back(new cBambo());
			retrybuttons[loop]->setSpritePos(buttonPos);
			retrybuttons[loop]->setTranslation(D3DXVECTOR2(0.0f,0.0f));
			retrybuttons[loop]->setTexture(buttonTextures [loop]);
			retrybuttons[loop]->SetId(loop);
		}
	
		for(int loop = 0; loop < 2; loop++)
		{
			buttonPos = D3DXVECTOR3( 100.0f + (400*loop) , 400.0f ,0);
			introbuttons.push_back(new cBambo());
			introbuttons[loop]->setSpritePos(buttonPos);
			introbuttons[loop]->setTranslation(D3DXVECTOR2(0.0f,0.0f));
			introbuttons[loop]->setTexture(introbuttonTextures [loop]);
			introbuttons[loop]->SetId(loop);
		}

	LPDIRECT3DSURFACE9 aSurface;				// the Direct3D surface
	LPDIRECT3DSURFACE9 gameSurface;
	LPDIRECT3DSURFACE9 endSurface;
	LPDIRECT3DSURFACE9 introSurface;
	LPDIRECT3DSURFACE9 theBackbuffer = NULL;  // This will hold the back buffer
	
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	// Create the background surface
	aSurface = d3dMgr->getD3DSurfaceFromFile("Images\\Background.png");
	gameSurface = d3dMgr->getD3DSurfaceFromFile("Images\\Background.png");
	endSurface = d3dMgr->getD3DSurfaceFromFile("Images\\End BackGround.png");
	introSurface = d3dMgr->getD3DSurfaceFromFile("Images\\introBackground.png");
	
	// load custom font
	cD3DXFont* bamboFont = new cD3DXFont(d3dMgr->getTheD3DDevice(),hInstance, "Cactus Sandwich");

	RECT textPos;
	SetRect(&textPos, 50, 10, 550, 100);
	
	RECT TimerPos;
	SetRect(&TimerPos, 50, 50, 550, 100);

	QueryPerformanceCounter((LARGE_INTEGER*)&previousTime);
	
	while( msg.message!=WM_QUIT )
	{
		// Check the message queue
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			if (currentScene == gameScene)
			{
				if ( gBamboLeft <= 0)
				{
					for(int loop = 0; loop < numBambo; loop++)
					{
						bamboPos = D3DXVECTOR3((spritePositions[(rand() % 10)]) ,(spritePositions[(rand() % 10)]),0);
						aBambo.push_back(new cBambo());
						aBambo[loop]->setSpritePos(bamboPos);
						aBambo[loop]->setTranslation(D3DXVECTOR2(0.0f,0.0f));
						aBambo[loop]->setTexture(bamboTextures[(loop % 3)]);
						gBamboLeft++;
					}
				}
			
			}
		}
			// Game code goes here
			if (surfaceUpdate == true)
			{
				if (currentScene == introScene)
				{
					aSurface = introSurface;
				}
				
				if (currentScene == gameScene)
				{
					gTime -= 0.05f;
					aSurface = gameSurface;
				}

				if (currentScene == endScene)
				{
					aSurface = endSurface;
				}
				surfaceUpdate = false;
			}

			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			float dt = (currentTime - previousTime)*sPC;

			// Accumulate how much time has passed.
			timeElapsed += dt;

				
			/*
			==============================================================
			| Update the postion of the balloons and check for collisions
			==============================================================
			*/
			if(timeElapsed > fpsRate)
			{
				for(iter = aBambo.begin(); iter != aBambo.end(); ++iter)
				{
					(*iter)->update(timeElapsed);			// update balloon
					abamboPos = (*iter)->getSpritePos();  // get the position of the current balloon
				}
				for(retrybuttoniter = retrybuttons.begin(); retrybuttoniter != retrybuttons.end(); ++retrybuttoniter)
				{
					(*retrybuttoniter)->update(timeElapsed);			// update the button
					aButtonPos = (*retrybuttoniter)->getSpritePos();  // get the position of the button
				}

				for(introbuttoniter = introbuttons.begin(); introbuttoniter != introbuttons.end(); ++introbuttoniter)
				{
					(*introbuttoniter)->update(timeElapsed);			// update the button
					aButtonPos = (*introbuttoniter)->getSpritePos();  // get the position of the button
				}

				
				if ( currentScene == gameScene)
				{
					d3dMgr->beginRender();
					theBackbuffer = d3dMgr->getTheBackBuffer();
					d3dMgr->updateTheSurface(gameSurface, theBackbuffer);
					d3dMgr->releaseTheBackbuffer(theBackbuffer);
					d3dxSRMgr->beginDraw();

					vector<cBambo*>::iterator iterB = aBambo.begin();
					for(iterB = aBambo.begin(); iterB != aBambo.end(); ++iterB)
					{
						d3dxSRMgr->setTheTransform((*iterB)->getSpriteTransformMatrix());  
						d3dxSRMgr->drawSprite((*iterB)->getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
					}
				}
				
				if (currentScene == endScene)
				{	
					d3dMgr->beginRender();
					theBackbuffer = d3dMgr->getTheBackBuffer();
					d3dMgr->updateTheSurface(gameSurface, theBackbuffer);
					d3dMgr->releaseTheBackbuffer(theBackbuffer);
					d3dxSRMgr->beginDraw();

					vector<cBambo*>::iterator retryiterB = retrybuttons.begin();
					for(retryiterB = retrybuttons.begin(); retryiterB != retrybuttons.end(); ++retryiterB)
					{
						d3dxSRMgr->setTheTransform((*retryiterB)->getSpriteTransformMatrix());  
						d3dxSRMgr->drawSprite((*retryiterB)->getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
					}
				}
				
				if (currentScene == introScene)
				{	
					d3dMgr->beginRender();
					theBackbuffer = d3dMgr->getTheBackBuffer();
					d3dMgr->updateTheSurface(introSurface, theBackbuffer);
					d3dMgr->releaseTheBackbuffer(theBackbuffer);
					d3dxSRMgr->beginDraw();
					
					vector<cBambo*>::iterator introiterB = introbuttons.begin();
					for(introiterB = introbuttons.begin(); introiterB != introbuttons.end(); ++introiterB)
					{
						d3dxSRMgr->setTheTransform((*introiterB)->getSpriteTransformMatrix());  
						d3dxSRMgr->drawSprite((*introiterB)->getTexture(),NULL,NULL,NULL,0xFFFFFFFF);
					}
				}

				list<cExplosion*>::iterator iter = gExplode.begin();
				while(iter != gExplode.end())
				{
					if((*iter)->isActive() == false)
					{
						iter = gExplode.erase(iter);
					}
					else
					{
						(*iter)->update(timeElapsed);
						d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());  
						d3dxSRMgr->drawSprite((*iter)->getTexture(),&((*iter)->getSourceRect()),NULL,NULL,0xFFFFFFFF);
						++iter;
					}

					
				}

				if (gTime <= 0.0f)
				{
						currentScene = endScene;
						surfaceUpdate = true;
				}

				d3dxSRMgr->endDraw();
				bamboFont->printText(gBamboEatenStr,textPos);
				
				stringstream ss(stringstream::in | stringstream::out);
				ss <<gTime;
				string gTimeString = ss.str();
				
				if (currentScene == gameScene)
				{
					bamboFont->printText(gTimeString.c_str(),TimerPos);
				}
				
				d3dMgr->endRender();
				if(currentScene == gameScene)
				{
					gTime -= 0.05f;
				}
				timeElapsed = 0.0f;
			}

			previousTime = currentTime;
		}
	
	d3dxSRMgr->cleanUp();
	d3dMgr->clean();
	return (int) msg.wParam;
}


