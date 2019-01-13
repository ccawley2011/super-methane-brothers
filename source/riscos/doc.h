/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//------------------------------------------------------------------------------
// The RISCOS main document header file
//------------------------------------------------------------------------------

#ifndef _doc_h
#define _doc_h 1

#include "audiodrv.h"
#include "game.h"
#include "target.h"

class CQTMDrv;
class CMethDoc
{

public:
	CMethDoc();
	~CMethDoc();

	void InitGame(void);
	void StartGame(void);
	void MainLoop(void *screen_ptr, int paused_flag);
	void RedrawMainView( int pal_change_flag );
	void PlayModule(int id);
	void StopModule(void);
	void PlaySample(int id, int pos, int rate);
	void RemoveSoundDriver(void);
	void InitSoundDriver(void);
	void UpdateModule(int id);
	void SaveScores(void);
	void LoadScores(void);
	void ChangeVolume(int s, int m);

private:
	CQTMDrv	*m_pQTMDrv;
	void DrawScreen( void *screen_ptr, int paused_flag );
private:
public:
	CGameTarget	m_GameTarget;

};

#endif



