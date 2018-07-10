/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//------------------------------------------------------------------------------
// The QTM Sound Driver wrapper (Source File)
//------------------------------------------------------------------------------

#ifdef METHANE_RISCOS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sound.h"

#include "audiodrv.h"
#include "swicalls.h"

static int SampleChannel = 5;	// Used by CQTMDrv::PlaySample

//------------------------------------------------------------------------------
//! \brief Sound driver constructor
//------------------------------------------------------------------------------
CQTMDrv::CQTMDrv()
{
}

//------------------------------------------------------------------------------
//! \brief Destroy the sound driver
//------------------------------------------------------------------------------
CQTMDrv::~CQTMDrv()
{
	RemoveDriver();
}

//------------------------------------------------------------------------------
//! \brief Initialise the sound driver
//------------------------------------------------------------------------------
void CQTMDrv::InitDriver(void)
{
	_kernel_swi_regs regs;
	unsigned char *ptr;
	int len;
	int cnt;
	unsigned char buffer[256];
	unsigned char *dest_lptr;
	unsigned char *lptr;
	int amplitude;
	signed char sample;

	// Create the log conversion tables
	dest_lptr = buffer;
	for (cnt=-128; cnt<=127; cnt++)
	{
		amplitude = cnt;

		regs.r[0] = amplitude << 24;
		_kernel_swi(Sound_SoundLog, &regs, &regs);	// PRM 4-26
		amplitude = regs.r[0];

		*(dest_lptr++) = amplitude;
	}

	lptr = buffer;
	for (cnt=0; cnt<SND_COUNT; cnt++)	// for all the samples
	{
		ptr = MethaneSampleData[cnt].ptr;
		len = MethaneSampleData[cnt].length;
		while(len>0)
		{
			sample = (signed char) *ptr;	// Get the sample
			*(ptr) = buffer[128 + sample];	// Log the sample
			ptr++;
			len--;
		}
	}
}

//------------------------------------------------------------------------------
//! \brief Remove the sound driver
//------------------------------------------------------------------------------
void CQTMDrv::RemoveDriver(void)
{
	// This was never implemented in the first place
}

//------------------------------------------------------------------------------
//! \brief Play a sample (called from the game)
//!
//! 	\param id = SND_xxx id
//!	\param pos = Sample Position to use 0 to 255
//!	\param rate = The rate
//------------------------------------------------------------------------------
void CQTMDrv::PlaySample(int id, int pos, int rate)
{
	SAMPLE_RESOURCE_DATA *snd_ptr;
	int channel;
	int volume;

	id = id - SND_START_NUMBER;
	if ((id<0) || (id >= SND_COUNT)) return;
	snd_ptr = &MethaneSampleData[id];

	// Assume centre position is critical (to fix fixed later)
	if ((pos>=120) && (pos <=130))
	{
		channel = 8;
	}else
	{
		// Use the 3 spare channels
		SampleChannel++;
		if (SampleChannel==8) SampleChannel = 5;
		channel = SampleChannel;
	}
	smb_snd_stereo(channel, pos - 127);

	// The volume wants to be equal while panning left to right
	//volume = 256-8;
	//pos = pos - 128;	// Centre position
	//if (pos < 0) pos = -pos;	// Check sign
	//volume = volume - pos;
	volume = 256-64;

	volume = volume >> 2;	// Divide by 4 to scale 0 to 64
	volume = volume - 16;	// It's a bit load
	if (volume < 10) volume = 10;	// It's a bit quiet

	// Now for the amiga note conversion FUDGE !!! (That'll teach me for converting it to windows first)
	if (rate >= 10000)
	{
		rate = rate / 30;
	}else
	{
		rate = rate / 16;
	}
	if (rate < 113) rate = 113;	// QTM min note limit
	if (rate > 856) rate = 856;	// QTM max note limit

	smb_snd_play_sample(snd_ptr->ptr, snd_ptr->length, channel, rate, volume );
}

//------------------------------------------------------------------------------
//! \brief Stop the module (called from the game)
//------------------------------------------------------------------------------
void CQTMDrv::StopModule(void)
{
	smb_snd_stop_module();
}

//------------------------------------------------------------------------------
//! \brief Play a module (called from the game)
//!
//! 	\param id = SMOD_xxx id
//------------------------------------------------------------------------------
void CQTMDrv::PlayModule(int id)
{
	MODULE_RESOURCE_DATA *mptr;

	id = id - MODULE_START_NUMBER;
	if ((id<0) || (id >= MODULE_COUNT)) return;
	mptr = &MethaneModuleData[id];

	smb_snd_play_module(mptr->ptr);

}

//------------------------------------------------------------------------------
//! \brief Update the current module (ie restart the module if it has stopped) (called from the game)
//!
//! 	\param id = SMOD_xxx id (The module that should be playing)
//------------------------------------------------------------------------------
void CQTMDrv::UpdateModule(int id)
{
	// Not required on RISCOS
}

void CQTMDrv::ChangeVolume(int s, int m)
{
	// TODO: Write me...
}

#endif // (METHANE_RISCOS)
