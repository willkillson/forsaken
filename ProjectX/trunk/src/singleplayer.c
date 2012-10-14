
#include <stdio.h>

#include "main.h"
#include "net.h"
#include "render.h"
#include "title.h"

#include "new3d.h"
#include "quat.h"
#include "compobjects.h"
#include "bgobjects.h"
#include "object.h"
#include "networking.h"
#include "multiplayer.h"
#include "mxload.h"
#include "mxaload.h"
#include <stdio.h>
#include "local.h"
#include "text.h"
#include "ships.h"
#include "loadsave.h"
#include "pickups.h"
#include "primary.h"
#include "controls.h"
#include "util.h"


extern _Bool IsHost;   // is the user hosting/joining a game
extern BYTE Current_Camera_View; // which object is currently using the camera view....
extern	BYTE	GameStatus[];	// Game Status for every Ship...
extern u_int16_t RandomStartPosModify;
extern _Bool PlayDemo;
extern	_Bool	CaptureTheFlag;
extern	_Bool	CTF;
extern	int16_t	SelectedBike;
extern	char	biker_name[256];
extern	_Bool	CountDownOn;
extern	_Bool	TeamGame;
extern  int16_t   NewLevelNum;
extern  BYTE          MyGameStatus;
extern SLIDER WatchPlayerSelect;

_Bool LoadASinglePlayerGame( MENUITEM * Item )
{
	int i;
	PlayDemo = false;
	IsHost = true;
	RandomStartPosModify = 0;
	SetBikeMods( (u_int16_t) (SelectedBike+2) );
	SetupNetworkGame();
	for( i = 0 ; i < MAX_PLAYERS ; i++ )
	{
		GameStatus[i] = STATUS_Null;
	}
	WhoIAm = 0;								// I was the first to join...
	Current_Camera_View = 0;				// set camera to that view
	WatchPlayerSelect.value = 0;
	SwitchedToWatchMode = false;
	Ships[WhoIAm].enable = 1;
	memset(&Names, 0, sizeof(SHORTNAMETYPE) );
	set_my_player_name();
	Ships[ WhoIAm ].BikeNum = ( SelectedBike % MAXBIKETYPES );
	

	NewLevelNum = -1;
	PreInGameLoad( Item );

	if( NewLevelNum == -1 )
		return false;

	CountDownOn = false;

	MyGameStatus = STATUS_TitleLoadGameStartingSinglePlayer;

	return true;
}

_Bool StartASinglePlayerGame( MENUITEM * Item )
{
	int i;

	PlayDemo = false;
	IsHost = true;
	// reset all bollocks...
	TeamGame = false;
	CaptureTheFlag = false;
	CTF = false;

	RandomStartPosModify = 0;

	SetBikeMods( (u_int16_t) (SelectedBike+2) );

	SetupNetworkGame();
	
	for( i = 0 ; i < MAX_PLAYERS ; i++ )
		GameStatus[i] = STATUS_Null;
	
	WhoIAm = 0;								// I was the first to join...
	Current_Camera_View = 0;				// set camera to that view
	WatchPlayerSelect.value = 0;
	SwitchedToWatchMode = false;
	Ships[WhoIAm].enable = 1;
	
	memset(&Names, 0, sizeof(SHORTNAMETYPE) );
	set_my_player_name();
	Ships[ WhoIAm ].BikeNum = ( SelectedBike % MAXBIKETYPES );
	
	CountDownOn = false;

	MyGameStatus = STATUS_StartingSinglePlayer;

	return true;
}
