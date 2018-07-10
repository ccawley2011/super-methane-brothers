/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//------------------------------------------------------------------------------
// The QTM Sound Driver wrapper (Header File)
//------------------------------------------------------------------------------

#ifdef METHANE_RISCOS

#ifndef _audiodrv_h
#define _audiodrv_h 1

#include "snddef.h"

class CQTMDrv
{
public:
	CQTMDrv();
	~CQTMDrv();
	void InitDriver(void);
	void RemoveDriver(void);
	void PlaySample(int id, int pos, int rate);
	void StopModule(void);
	void PlayModule(int id);
	void UpdateModule(int id);
	void ChangeVolume(int s, int m);

};

#endif
#endif // (METHANE_RISCOS)
