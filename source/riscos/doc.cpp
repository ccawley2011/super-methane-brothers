/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//------------------------------------------------------------------------------
// The RISCOS main document file
//------------------------------------------------------------------------------

#include "swicalls.h"
#include "doc.h"

//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------
extern void main_code(void);

//------------------------------------------------------------------------------
// The Game Instance
//------------------------------------------------------------------------------
CMethDoc Game;
LAYER *TheScreen = 0;

//------------------------------------------------------------------------------
// The HighScore table filename
//------------------------------------------------------------------------------
static char HighScoreFileName[] = "<SMBGame$Dir>.HiScores";
#define HighScoreLoadBufferSize (MAX_HISCORES * 64)

//------------------------------------------------------------------------------
// The Main Function
//------------------------------------------------------------------------------
int main( void )
{
	SMB_Screen_Depth = AV_256_COLOUR;
	SMB_Screen_Width = 640;
	SMB_Screen_Height = 480;
	SMB_Screen_HZ = -1;
	smb_view_screen();				// Set up the image library screen

	smb_keyrep_off();
	main_code();					// The main routine
	smb_keyrep_on();
	return( 0 ) ;
}

//------------------------------------------------------------------------------
//! \brief The program main code
//------------------------------------------------------------------------------
void main_code(void)
{
	LAYER *lptr;
	int key;
	JOYSTICK *jptr1;
	JOYSTICK *jptr2;
	unsigned int time_start;
	unsigned int time_now;
	int time_diff;

	lptr = smb_vgfx_layer_create_sprite( SCR_WIDTH, SCR_HEIGHT, AV_256_COLOUR, 0);

	printf("The GNU General Public License V2 applies to this game.\n\n");
	printf("See: http://methane.sourceforge.net\n");
	if (QTM_Available)
	{
		printf("(QTM MODULE FOUND - Audio Enabled)\n");
	}else
	{
		printf("(QTM MODULE NOT FOUND - MUSIC/SOUND EFFECTS DISABLED)\n");
	}

	printf("Instructions:\n\n");
	printf("Press Fire to start. Use keyboard to enter player names.\n");
	printf("Press Return to start game. Tap Fire to fire gas from the gun.\n");
	printf("Hold Fire to suck a trapped baddie into the gun.\n");
	printf("Release Fire to throw the trapped baddie from the gun.\n");
	printf("Throw baddies at the wall to destroy them.\n\n");
	printf("Keys:\n");
	printf("Player One - Use Cursor keys and control (CTRL) to fire.\n");
	printf("Player Two - Use A,W,S,D and shift to fire.\n\n");
	printf("# = Quit (and save high scores)\n");
	printf("TAB = Change player graphic during game\n");
	//printf("F1 = Next Level (DISABLED)\n");

	smb_keybank_flush();
	TheScreen = lptr;
	Game.InitSoundDriver();
	Game.InitGame();
	Game.LoadScores();
	Game.StartGame();
	jptr1 = &Game.m_GameTarget.m_Joy1;
	jptr2 = &Game.m_GameTarget.m_Joy2;
	time_start = smb_gettime(0);
	while(1)
	{
		smb_keybank_scan();

		if (smb_getkeyready())
		{
			key = smb_getkey();
			if (key == '#') break;
			if (key == '\t')
			{
				Game.m_GameTarget.m_Game.TogglePuffBlow();
			}
			jptr1->key = key;
			jptr2->key = key;
		}
		jptr1->right = SMB_Keybank_Buffer[SMB_KB_RIGHT];
		jptr1->left = SMB_Keybank_Buffer[SMB_KB_LEFT];
		jptr1->up = SMB_Keybank_Buffer[SMB_KB_UP];
		jptr1->down = SMB_Keybank_Buffer[SMB_KB_DOWN];
		jptr1->fire = SMB_Keybank_Buffer[SMB_KB_FIRE];

		jptr2->right = SMB_Keybank_Buffer[SMB_KB_D];
		jptr2->left = SMB_Keybank_Buffer[SMB_KB_A];
		jptr2->up = SMB_Keybank_Buffer[SMB_KB_W];
		jptr2->down = SMB_Keybank_Buffer[SMB_KB_S];
		jptr2->fire = SMB_Keybank_Buffer[SMB_KB_SHIFT];

		// (CHEAT MODE DISABLED) --> jptr1->next_level = SMB_Keybank_Buffer[SMB_KB_CHEAT];
		Game.MainLoop(0);
		do
		{
			time_now = smb_gettime(0);
			time_diff = time_now - time_start;
		}while (time_diff < 5);
		time_start += time_diff;
	}
	Game.SaveScores();

	smb_vgfx_layer_free(lptr);

}

//------------------------------------------------------------------------------
//! \brief Initialise Document
//------------------------------------------------------------------------------
CMethDoc::CMethDoc()
{
	SMB_NEW(m_pQTMDrv, CQTMDrv);
	m_GameTarget.Init(this);
}

//------------------------------------------------------------------------------
//! \brief Destroy Document
//------------------------------------------------------------------------------
CMethDoc::~CMethDoc()
{
	if (m_pQTMDrv)
	{
		delete(m_pQTMDrv);
		m_pQTMDrv = 0;
	}
}

//------------------------------------------------------------------------------
//! \brief Initialise the game
//------------------------------------------------------------------------------
void CMethDoc::InitGame(void)
{
	m_GameTarget.InitGame((char *)TheScreen->bitmap_ptr);
	m_GameTarget.PrepareSoundDriver();
}

//------------------------------------------------------------------------------
//! \brief Initialise the sound driver
//------------------------------------------------------------------------------
void CMethDoc::InitSoundDriver(void)
{
	m_pQTMDrv->InitDriver();
}

//------------------------------------------------------------------------------
//! \brief Remove the sound driver
//------------------------------------------------------------------------------
void CMethDoc::RemoveSoundDriver(void)
{
	m_pQTMDrv->RemoveDriver();
}

//------------------------------------------------------------------------------
//! \brief Start the game
//------------------------------------------------------------------------------
void CMethDoc::StartGame(void)
{
	m_GameTarget.StartGame();
}

//------------------------------------------------------------------------------
//! \brief Redraw the game main view
//!
//! 	\param pal_change_flag : 0 = Palette not changed
//------------------------------------------------------------------------------
void CMethDoc::RedrawMainView( int pal_change_flag )
{
	// Function not used
}

//------------------------------------------------------------------------------
//! \brief Draw the screen
//!
//! 	\param screen_ptr = UNUSED
//------------------------------------------------------------------------------
void CMethDoc::DrawScreen( void *screen_ptr )
{
	METHANE_RGB *pptr;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	int cnt;

	smb_sync();	// Lock to VBL
	smb_vgfx_sprite_draw( TheScreen, 128, 64, 0 );

	// Set the game palette
	pptr = m_GameTarget.m_rgbPalette;
	for (cnt=0; cnt < PALETTE_SIZE; cnt++, pptr++)
	{
		red = pptr->red;
		green = pptr->green;
		blue = pptr->blue; 
		smb_setpalette( cnt, 16, red, green, blue );
	}
}

//------------------------------------------------------------------------------
//! \brief The Game Main Loop 
//!
//! 	\param screen_ptr = UNUSED
//------------------------------------------------------------------------------
void CMethDoc::MainLoop( void *screen_ptr )
{
	m_GameTarget.MainLoop();
	DrawScreen( screen_ptr );

}

//------------------------------------------------------------------------------
//! \brief Play a sample (called from the game)
//!
//! 	\param id = SND_xxx id
//!	\param pos = Sample Position to use 0 to 255
//!	\param rate = The rate
//------------------------------------------------------------------------------
void CMethDoc::PlaySample(int id, int pos, int rate)
{
	m_pQTMDrv->PlaySample(id, pos, rate);
}

//------------------------------------------------------------------------------
//! \brief Stop the module (called from the game)
//------------------------------------------------------------------------------
void CMethDoc::StopModule(void)
{
	m_pQTMDrv->StopModule();
}

//------------------------------------------------------------------------------
//! \brief Play a module (called from the game)
//!
//! 	\param id = SMOD_xxx id
//------------------------------------------------------------------------------
void CMethDoc::PlayModule(int id)
{
	m_pQTMDrv->PlayModule(id);
}

//------------------------------------------------------------------------------
//! \brief Update the current module (ie restart the module if it has stopped) (called from the game)
//!
//! 	\param id = SMOD_xxx id (The module that should be playing)
//------------------------------------------------------------------------------
void CMethDoc::UpdateModule(int id)
{
	m_pQTMDrv->UpdateModule(id);
}

//------------------------------------------------------------------------------
//! \brief Load the high scores
//------------------------------------------------------------------------------
void CMethDoc::LoadScores(void)
{
	FILE *fptr;
	char *mptr;
	char *tptr;
	char let;
	int cnt;

	fptr = fopen(HighScoreFileName, "r");
	if (!fptr) return;	// No scores available

	// Allocate file memory, which is cleared to zero
	mptr = (char *) calloc(1, HighScoreLoadBufferSize);
	if (!mptr)		// No memory
	{
		fclose(fptr);
		return;
	}
	fread( mptr, 1, HighScoreLoadBufferSize-2, fptr);	// Get the file

	// (Note: mptr is zero terminated)
	tptr = mptr;
	for (cnt=0; cnt<MAX_HISCORES; cnt++)	// For each highscore
	{

		if (!tptr[0]) break;

		m_GameTarget.m_Game.InsertHiScore( atoi(&tptr[4]), tptr );

		do	// Find next name
		{
			let = *(tptr++);
		}while (!( (let=='$') || (!let) ));
		if (!let) break;	// Unexpected EOF
	}

	free(mptr);

	fclose(fptr);

}

//------------------------------------------------------------------------------
//! \brief Save the high scores
//------------------------------------------------------------------------------
void CMethDoc::SaveScores(void)
{
	FILE *fptr;
	int cnt;
	HISCORES *hs;

	fptr = fopen(HighScoreFileName, "w");
	if (!fptr) return;	// Cannot write scores
	for (cnt=0, hs=m_GameTarget.m_Game.m_HiScores; cnt<MAX_HISCORES; cnt++, hs++)
	{
		fprintf(fptr, "%c%c%c%c%d$", hs->name[0], hs->name[1], hs->name[2], hs->name[3], hs->score);
	}
	fclose(fptr);

}

//------------------------------------------------------------------------------
//! \brief Change the music and sound volume
//! \param s = sound volume (0 to 10)
//! \param m = music volume (0 to 10)
//------------------------------------------------------------------------------
void CMethDoc::ChangeVolume(int s, int m)
{
	m_pQTMDrv->ChangeVolume(s, m);
}
