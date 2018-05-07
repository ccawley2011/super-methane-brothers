/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Nintendo Switch Port by carstene1ns 04/2018
 */

//------------------------------------------------------------------------------
// Methane Brothers Document (Source File)
//------------------------------------------------------------------------------

#include <cstdlib>
#include <cstdio>

#include <switch.h>

#include "doc.h"
#include "target.h"
#ifdef METHANE_MIKMOD
#include "../mikmod/audiodrv.h"
#endif

//------------------------------------------------------------------------------
// The Screen, fixed video mode 320 x 256
//------------------------------------------------------------------------------
static char TheScreen[SCR_WIDTH * SCR_HEIGHT];

#ifdef METHANE_MIKMOD

int done = 0;
Thread mikModThread = { 0 };

static void AudioChannelThread(void *args) {
    CMikModDrv *m_pMikModDrv = (CMikModDrv *)args;
    while (!done) {
        m_pMikModDrv->Update();
        svcSleepThread(20000000);
    }
    return;
}

#endif

//------------------------------------------------------------------------------
//! \brief Initialise Document
//------------------------------------------------------------------------------
CMethDoc::CMethDoc()
{
#ifdef METHANE_MIKMOD
    SMB_NEW(m_pMikModDrv, CMikModDrv);
#endif

    m_GameTarget.Init(this);
    m_GameTarget.InitGame(TheScreen);
}

//------------------------------------------------------------------------------
//! \brief Destroy Document
//------------------------------------------------------------------------------
CMethDoc::~CMethDoc()
{
#ifdef METHANE_MIKMOD
    if (m_pMikModDrv) {
        delete(m_pMikModDrv);
        m_pMikModDrv = 0;
    }
#endif
}

//------------------------------------------------------------------------------
//! \brief Initialise the game
//------------------------------------------------------------------------------
void CMethDoc::InitGame(void)
{
    m_GameTarget.InitGame(TheScreen);
    m_GameTarget.PrepareSoundDriver();
}

//------------------------------------------------------------------------------
//! \brief Initialise the sound driver
//------------------------------------------------------------------------------
void CMethDoc::InitSoundDriver(void)
{
#ifdef METHANE_MIKMOD
    m_pMikModDrv->InitDriver();

    // update audio in a separate thread
    if (threadCreate(&mikModThread, AudioChannelThread, m_pMikModDrv, 0x5000, 0x1C, -2) == 0)
        threadStart(&mikModThread);
#endif
}

//------------------------------------------------------------------------------
//! \brief Remove the sound driver
//------------------------------------------------------------------------------
void CMethDoc::RemoveSoundDriver(void)
{
#ifdef METHANE_MIKMOD
    /* allow audio thread to terminate cleanly */
    done = 1;
    if (mikModThread.handle) {
        threadWaitForExit(&mikModThread);
        threadClose(&mikModThread);
    }

    m_pMikModDrv->RemoveDriver();
#endif
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
void CMethDoc::RedrawMainView( int )
{
    // Function not used
}

//------------------------------------------------------------------------------
//! \brief Draw the screen
//!
//! 	\param screen_ptr = 32 bit per pixel screen
//!	\param paused_flag = 0 = Game not paused
//------------------------------------------------------------------------------

void CMethDoc::DrawScreen( void *screen_ptr, int paused_flag)
{
    int cnt;
    int *dptr;
    unsigned char *sptr;
    METHANE_RGB *pptr;
    METHANE_RGB *mptr;

    int *tptr_base;
    int *tptr;

    // Allocate a temporary palette to assist conversion
    tptr_base = (int *) malloc(sizeof(int) * PALETTE_SIZE);
    if (!tptr_base) return;

    // Convert the game palette to 32bit RGB
    pptr = m_GameTarget.m_rgbPalette;
    tptr = tptr_base;

    if (!paused_flag) {
        for (cnt = 0; cnt < PALETTE_SIZE; cnt++, pptr++) {
            mptr = pptr;
            *(tptr++) = (mptr->red) | (mptr->green << 8) |
                (mptr->blue << 16) | (0xFF << 24);
        }
    } else {
        // If paused - grey scale the palette
        for (cnt=0; cnt < PALETTE_SIZE; cnt++, pptr++) {
            mptr = pptr;
            unsigned char cval = (mptr->red + mptr->green + mptr->blue) / 3;
            *(tptr++) = (0xFF << 24) | (cval << 16) | (cval << 8) | (cval);
        }
    }

    // Write to the screen
    dptr = (int *) screen_ptr;
    sptr = (unsigned char *) TheScreen;
    tptr = tptr_base;
    for (cnt = 0; cnt < (SCR_WIDTH * SCR_HEIGHT); cnt++)
        *(dptr++) = tptr[*(sptr++)];

    // Deallocate the temporary palette
    free(tptr_base);
}

//------------------------------------------------------------------------------
//! \brief The Game Main Loop 
//!
//! 	\param screen_ptr = 32 bit per pixel screen
//!	\param paused_flag = 0 = Game not paused
//------------------------------------------------------------------------------
void CMethDoc::MainLoop( void *screen_ptr, int paused_flag )
{
    if (!paused_flag)
        m_GameTarget.MainLoop();

    DrawScreen(screen_ptr, paused_flag);
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
#ifdef METHANE_MIKMOD
    m_pMikModDrv->PlaySample(id, pos, rate);
#endif
}

//------------------------------------------------------------------------------
//! \brief Stop the module (called from the game)
//------------------------------------------------------------------------------
void CMethDoc::StopModule(void)
{
#ifdef METHANE_MIKMOD
    m_pMikModDrv->StopModule();
#endif
}

//------------------------------------------------------------------------------
//! \brief Play a module (called from the game)
//!
//! 	\param id = SMOD_xxx id
//------------------------------------------------------------------------------
void CMethDoc::PlayModule(int id)
{
#ifdef METHANE_MIKMOD
    m_pMikModDrv->PlayModule(id);
#endif
}

//------------------------------------------------------------------------------
//! \brief Update the current module (ie restart the module if it has stopped) (called from the game)
//!
//! 	\param id = SMOD_xxx id (The module that should be playing)
//------------------------------------------------------------------------------
void CMethDoc::UpdateModule(int id)
{
#ifdef METHANE_MIKMOD
    m_pMikModDrv->UpdateModule(id);
#endif
}

// Not implemented yet, needs function to insert name using controller first
//------------------------------------------------------------------------------
//! \brief Load the high scores
//------------------------------------------------------------------------------
void CMethDoc::LoadScores(void)
{

}

//------------------------------------------------------------------------------
//! \brief Save the high scores
//------------------------------------------------------------------------------
void CMethDoc::SaveScores(void)
{

}

//------------------------------------------------------------------------------
//! \brief Change the music and sound volume
//! \param s = sound volume (0 to 10)
//! \param m = music volume (0 to 10)
//------------------------------------------------------------------------------
void CMethDoc::ChangeVolume(int s, int m)
{
#ifdef METHANE_MIKMOD
    m_pMikModDrv->ChangeVolume(s, m);
#endif
}
