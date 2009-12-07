#ifdef SOUND_DSOUND

#define DIRECTSOUND_VERSION 0x0700
#include <dsound.h>
#include <stdio.h>

#include "main.h"
#include "util.h"
#include "file.h"
#include "sound.h"


// globals
LPDIRECTSOUND lpDS = NULL;
BOOL Sound3D = FALSE;

//
// Generic Functions
//

BOOL sound_init( void )
{
	int iErr;
	lpDS = NULL;
	if FAILED( CoInitialize(NULL) )
		return FALSE;

	// Attempt to initialize with DirectSound.
	// First look for DSOUND.DLL using CoCreateInstance.
	iErr = CoCreateInstance(&CLSID_DirectSound, NULL, CLSCTX_INPROC_SERVER,
								 &IID_IDirectSound, (void **) &lpDS);
	
		
	if ((iErr >= DS_OK)	&& (lpDS)) // Found DSOUND.DLL
		iErr = IDirectSound_Initialize(lpDS, NULL);	// Try to init Direct Sound.

	if (iErr < DS_OK)
		return FALSE; // Failed to get DirectSound, so no sound-system available.

	// build sound_caps structure
	sound_minimum_volume = (DSBVOLUME_MIN / 3);
	
	// Set control-level of DirectSound. (To normal, default.)
	if(IDirectSound_SetCooperativeLevel(lpDS, GetActiveWindow(), DSSCL_EXCLUSIVE) >= DS_OK)
		return TRUE;

	// If here, failed to initialize sound system in some way
	IDirectSound_Release(lpDS);
	lpDS = NULL;
	return(FALSE);
}

//
// Listener
//

//	 In dsound only valid when using 3d sound
//   which this game does not currently use
//   this is left here for openAL later

BOOL sound_listener_position( float x, float y, float z )
{
	return TRUE;
}

BOOL sound_listener_velocity( float x, float y, float z )
{
	return TRUE;
}

BOOL sound_listener_orientation( 
	float fx, float fy, float fz, // forward vector
	float ux, float uy, float uz  // up vector
)
{
	return TRUE;
}

//
// Buffers
//

void sound_destroy( void )
{
	if ( !lpDS )
		return;
	IDirectSound_Release(lpDS);
}

void sound_buffer_play( void * buffer )
{
	IDirectSoundBuffer_Play(
		(IDirectSoundBuffer*)buffer, 0, 0, 0 
	);
}

void sound_buffer_play_looping( void * buffer )
{
	IDirectSoundBuffer_Play(
		(IDirectSoundBuffer*)buffer, 0, 0,
		DSBPLAY_LOOPING 
	);
}

void sound_buffer_stop( void * buffer )
{
	IDirectSoundBuffer_Stop( 
		(IDirectSoundBuffer*) buffer 
	);
}

DWORD sound_buffer_size( void * buffer )
{
	DSBCAPS dsbcaps; 
	dsbcaps.dwSize = sizeof( DSBCAPS );
	IDirectSoundBuffer_GetCaps(
		(IDirectSoundBuffer*)buffer,
		&dsbcaps 
	);
	return dsbcaps.dwBufferBytes;
}

void sound_buffer_release( void * ptr )
{
	IDirectSoundBuffer* buffer = ptr;
	if (buffer != NULL)
		return;
	buffer->lpVtbl->Release(buffer);
	buffer = NULL;
}

void sound_buffer_3d_release( void * buffer )
{
	IDirectSound3DBuffer_Release(
		(IDirectSound3DBuffer*) buffer 
	);
}

BOOL sound_buffer_is_playing( void * buffer )
{
	DWORD dwStatus;
	IDirectSoundBuffer_GetStatus(
		(IDirectSoundBuffer*)buffer, 
		&dwStatus 
	);
	return (dwStatus & DSBSTATUS_PLAYING);
}

void sound_buffer_set_freq( void* ptr, float freq )
{
	IDirectSoundBuffer * buffer = ptr;
	LPWAVEFORMATEX lpwaveinfo;
	DWORD dwSizeWritten, OrigFreq;

	// BUG:  Appears buffer pointer goes bad or is passed in as NULL
	if(!buffer)
	{
		DebugPrintf("BUG: sound_buffer_set_freq() buffer passed in was null\n");
		return;
	}

	if ( !freq || ( freq == 1.0F ) )
	{
		OrigFreq = DSBFREQUENCY_ORIGINAL; 
	}
	else
	{
		// get original frequency of buffer
		IDirectSoundBuffer_GetFormat( buffer, NULL, 0, &dwSizeWritten );
		lpwaveinfo = (LPWAVEFORMATEX)malloc( dwSizeWritten );
		IDirectSoundBuffer_GetFormat( buffer, lpwaveinfo, dwSizeWritten, 0 );
		OrigFreq = lpwaveinfo->nSamplesPerSec; 
		free(lpwaveinfo);
	
		// work out new frequency
		OrigFreq = (DWORD)( (float)OrigFreq * freq );

		if ( OrigFreq < DSBFREQUENCY_MIN )
			OrigFreq = DSBFREQUENCY_MIN;

		if ( OrigFreq > DSBFREQUENCY_MAX )
			OrigFreq = DSBFREQUENCY_MAX;
	}

	// set frequency
	if ( IDirectSoundBuffer_SetFrequency( buffer, OrigFreq ) != DS_OK )
		DebugPrintf("sound_buffer_set_freq: failed\n");
}

void sound_buffer_volume( void * buffer, long volume )
{
	IDirectSoundBuffer_SetVolume( (IDirectSoundBuffer*) buffer, volume );
}

void sound_buffer_pan( void * buffer, long pan )
{
	IDirectSoundBuffer_SetPan( (IDirectSoundBuffer*) buffer, pan );
}

DWORD sound_buffer_get_freq( void * buffer ) // samples per sec
{
	LPWAVEFORMATEX lpwaveinfo;
	DWORD dwSizeWritten, freq;
	IDirectSoundBuffer_GetFormat(
		(IDirectSoundBuffer*) buffer,
		NULL, 0, &dwSizeWritten 
	);
	lpwaveinfo = (LPWAVEFORMATEX)malloc( dwSizeWritten );
	IDirectSoundBuffer_GetFormat( 
		(IDirectSoundBuffer*) buffer,
		lpwaveinfo, dwSizeWritten, 0 
	);
	freq = lpwaveinfo->nSamplesPerSec; 
	free(lpwaveinfo);
	return freq;
}

DWORD sound_buffer_get_rate( void * buffer ) // avg bytes per second
{
	LPWAVEFORMATEX lpwaveinfo;
	DWORD dwSizeWritten, datarate;
	IDirectSoundBuffer_GetFormat( 
		(IDirectSoundBuffer*) buffer, 
		NULL, 0, &dwSizeWritten 
	);
	lpwaveinfo = (LPWAVEFORMATEX)malloc( dwSizeWritten );
	IDirectSoundBuffer_GetFormat( 
		(IDirectSoundBuffer*) buffer, 
		lpwaveinfo, dwSizeWritten, 0 
	);
	datarate = lpwaveinfo->nAvgBytesPerSec; 
	free(lpwaveinfo);
	return datarate;
}

// this gets the current play location
void sound_buffer_get_position( void * buffer, DWORD* time )
{
	IDirectSoundBuffer_GetCurrentPosition(
		(IDirectSoundBuffer*) buffer,
		time,
		NULL
	);
}

// this moves to a specific offset in the buffer
void sound_buffer_set_position( void * buffer, DWORD time )
{
	IDirectSoundBuffer_SetCurrentPosition(
		(IDirectSoundBuffer*) buffer,
		time
	);
}

// this sets the location in 3d space of the sound
void sound_buffer_set_3d_position( void * buffer, float x, float y, float z, float min, float max )
{			
	IDirectSound3DBuffer_SetPosition(
		(IDirectSound3DBuffer*) buffer,
		x, y, z, DS3D_IMMEDIATE
	);
	IDirectSound3DBuffer_SetMinDistance(
		(IDirectSound3DBuffer*) buffer,
		min, DS3D_IMMEDIATE
	); 
	IDirectSound3DBuffer_SetMaxDistance(
		(IDirectSound3DBuffer*) buffer,
		max, DS3D_IMMEDIATE
	); 
}

void* sound_buffer_load(char *name)
{
    IDirectSoundBuffer *sound_buffer = NULL;
    DSBUFFERDESC buffer_description = {0};
	WAVEFORMATEX buffer_format;
	SDL_AudioSpec wav_spec;
	Uint32 wav_length;
	Uint8 *wav_buffer;
	DWORD flags = DSBCAPS_STATIC | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE;

	if (!lpDS)
		return NULL;

	if( SDL_LoadWAV(name, &wav_spec, &wav_buffer, &wav_length) == NULL )
	{
		DebugPrintf("Could not open test.wav: %s\n", SDL_GetError());
		return NULL;
	}

	// http://msdn.microsoft.com/en-us/library/ms897764.aspx
    buffer_description.dwSize			= sizeof(buffer_description);
    buffer_description.dwFlags			= flags;
	buffer_description.dwBufferBytes	= (DWORD) wav_length;
	buffer_description.lpwfxFormat		= &buffer_format;

	// http://msdn.microsoft.com/en-us/library/dd757720%28VS.85%29.aspx
	buffer_format.wFormatTag		= (WORD)	WAVE_FORMAT_PCM;
	buffer_format.nChannels			= (WORD)	wav_spec.channels;
	buffer_format.wBitsPerSample	= (WORD)	((wav_spec.format == AUDIO_U8 || wav_spec.format == AUDIO_S8) ? 8 : 16);
	buffer_format.nSamplesPerSec	= (DWORD)	wav_spec.freq;
	buffer_format.nBlockAlign		= (WORD)	(buffer_format.nChannels * buffer_format.wBitsPerSample) / 8;
	buffer_format.nAvgBytesPerSec	= (DWORD)	(buffer_format.nSamplesPerSec * buffer_format.nBlockAlign);
	buffer_format.cbSize			= (WORD)	0;

	// http://msdn.microsoft.com/en-us/library/ms898123.aspx
	if( IDirectSound_CreateSoundBuffer( lpDS, &buffer_description, &sound_buffer, NULL ) == DS_OK )
    {
        LPVOID pMem1, pMem2;
        DWORD dwSize1, dwSize2;
        if (SUCCEEDED(IDirectSoundBuffer_Lock(sound_buffer, 0, wav_length, &pMem1, &dwSize1, &pMem2, &dwSize2, 0)))
        {
            CopyMemory(pMem1, wav_buffer, dwSize1);
            if ( 0 != dwSize2 )
                CopyMemory(pMem2, wav_buffer+dwSize1, dwSize2);
            IDirectSoundBuffer_Unlock(sound_buffer, pMem1, dwSize1, pMem2, dwSize2);
        }
		else
		{
            sound_buffer_release(sound_buffer);
            sound_buffer = NULL;
		}
    }
    else
    {
        sound_buffer = NULL;
    }

	SDL_FreeWAV(wav_buffer);

    return sound_buffer;
}

BOOL sound_buffer_duplicate( void * source, void ** destination )
{
	return IDirectSound_DuplicateSoundBuffer( 
		lpDS,
		(LPDIRECTSOUNDBUFFER) source, 
		(LPDIRECTSOUNDBUFFER*) destination
	) == DS_OK;
}

#endif // SOUND_DSOUND