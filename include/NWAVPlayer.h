/*============================================================\
|  This file was made by TheGameratorT.                       |
|                                                             |
|  This code is meant to work with the following template:    |
|  https://github.com/Overblade/NSMB-ASMReference             |
|                                                             |
|  You may modify this file and use it for whatever you want  |
|  just be sure to credit me (TheGameratorT).                 |
|                                                             |
|  Hope you like it just as much as I had fun coding this!    |
|                                                             |
|  ---------------------------------------------------------  |
|                                                             |
|  NWAV player core declarations.                             |
|  This is the header that allows the engine code to be       |
|  implemented into the game and be accessed externally.      |
\============================================================*/

#ifndef _NWAVPLAYER_H
#define _NWAVPLAYER_H

#define NWAV 0x5641574E
#define FX32_CAST(x) ((fx32)x)
#define FX32_SHIFT 12

//#include "nitro_if.h"
#include "../include/types.h"
#include "../include/config.h"
#include "../include/debug.h"
#include "../include/sound.h"




//The function type of the function that will handle the events.
typedef void(*NWAVPlayer_EventHandler)(int); 

/// <summary>Initializes the player system. (Hook after SND_Init)</summary>
void NWAVPlayer_init(void);

/// <summary>Updates the game fading. (Hook after SND_Main)</summary>
BOOL NWAVPlayer_updateFade(void);

/// <summary>Plays a music.</summary>
/// <param name="fileID">The file ID of the music file to play.</param>
void NWAVPlayer_play(int fileID);

/// <summary>Stops the music playing.</summary>
/// <param name="frames">Number of frames where the volume shift occurs.</param>
void NWAVPlayer_stop(int frames);

/// <summary>Gets the music volume.</summary>
/// <returns>The music volume.</returns>
int  NWAVPlayer_getVolume(void);

/// <summary>Sets the music volume.</summary>
/// <param name="volume">The target volume. Value range = [0, 127]</param>
/// <param name="frames">Number of frames where the volume shift occurs.</param>
void NWAVPlayer_setVolume(int volume, int frames);

/// <summary>Gets the music speed.</summary>
/// <returns>The current music speed.</returns>
fx32 NWAVPlayer_getSpeed(void);

/// <summary>Sets the music speed.</summary>
/// <param name="speed">The target speed for the music to be played at.</param>
void NWAVPlayer_setSpeed(fx32 speed);

/// <summary>Gets if the music is paused.</summary>
/// <returns>True if the music is paused. False otherwise.</returns>
BOOL NWAVPlayer_getPaused(void);

/// <summary>Sets if the music is paused.</summary>
/// <param name="paused">Sets the music as paused when true, unpauses when false.</param>
void NWAVPlayer_setPaused(BOOL paused);

/// <summary>Sets the event handler function.</summary>
/// <param name="func">The function pointer of the event handler.</param>
void NWAVPlayer_setEventHandler(NWAVPlayer_EventHandler func);



// Streamed Audio
void LONG_CALL NNS_SndInit_Original(void);
void LONG_CALL NNS_SndMain_Original(void);
void LONG_CALL PlayBGM_Original(u16 seqno);
//void LONG_CALL NNS_SndPlayerSetTempoRatio_Original(int handle, int tempo);
void LONG_CALL NNS_SndPlayerStopSeqByPlayerNo_Original(u8 playerID, int fadeFrame);
void LONG_CALL GF_SndHandleMoveVolume_Original(int param1, int volume, int frames);
void LONG_CALL NNS_SndPlayerPauseByPlayerNo_Original(u8 playerID, BOOL paused); // playerID can be either PLAYER_FIELD or PLAYER_BGM (1 or 7)

typedef u8 FSFile[72];
typedef u8 OSThread[200];
typedef u8 OSMessageQueue[32];

typedef void *OSMessage;
typedef void (*SNDAlarmHandler)(void*);


typedef enum
{
	SND_CHANNEL_DATASHIFT_NONE,
	SND_CHANNEL_DATASHIFT_1BIT,
	SND_CHANNEL_DATASHIFT_2BIT,
	SND_CHANNEL_DATASHIFT_4BIT
} SNDChannelDataShift;

typedef enum
{
	SND_WAVE_FORMAT_PCM8,
	SND_WAVE_FORMAT_PCM16,
	SND_WAVE_FORMAT_ADPCM,
	SND_WAVE_FORMAT_PSG,
	SND_WAVE_FORMAT_NOISE = SND_WAVE_FORMAT_PSG
} SNDWaveFormat;

typedef enum
{
	SND_CHANNEL_LOOP_MANUAL,
	SND_CHANNEL_LOOP_REPEAT,
	SND_CHANNEL_LOOP_1SHOT
} SNDChannelLoop;

typedef enum
{
	FS_SEEK_SET,
	FS_SEEK_CUR,
	FS_SEEK_END
} FSSeekFileMode;

#define FX32_CAST(x) ((fx32)x)
#define FX32_SHIFT 12

//void OS_Panic();
void LONG_CALL OS_WakeUpThreadDirect(OSThread *thread);
void LONG_CALL OS_CreateThread(OSThread *thread, void (*func)(void *), void *arg, void *stack, u32 stackSize, u32 prio);
BOOL LONG_CALL OS_ReceiveMessage(OSMessageQueue *mq, OSMessage *msg, s32 flags);
BOOL LONG_CALL OS_SendMessage(OSMessageQueue *mq, OSMessage msg, s32 flags);
void LONG_CALL OS_InitMessageQueue(OSMessageQueue *mq, OSMessage *msgArray, s32 msgCount);

void MI_CpuFill8(void *dest, u8 data, u32 size);
//static inline void MI_CpuClear8(void *dest, u32 size) {
//    MI_CpuFill8(dest, 0, size);
//}

void LONG_CALL SND_SetupChannelPcm(int chNo, SNDWaveFormat format, const void *dataAddr, SNDChannelLoop loop, int loopStart, int dataLen, int volume, SNDChannelDataShift shift, int timer, int pan);
void LONG_CALL SND_SetChannelVolume(u32 chBitMask, int volume, SNDChannelDataShift shift);
void LONG_CALL SND_LockChannel(u32 chBitMask, u32 flags);
void LONG_CALL SND_SetupAlarm(int alarmNo, u32 tick, u32 period, SNDAlarmHandler handler, void *arg);
void LONG_CALL SND_StopTimer(u32 chBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags);
void LONG_CALL SND_StartTimer(u32 chBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags);
void LONG_CALL NNS_SndSetMasterVolume(int volume);

BOOL LONG_CALL FS_SeekFile(FSFile *p_file, s32 offset, FSSeekFileMode origin);
s32  LONG_CALL FS_ReadFile(FSFile *p_file, void *dst, s32 len);
BOOL LONG_CALL FS_CloseFile(FSFile *p_file);
BOOL LONG_CALL FS_OpenFileFast(FSFile* p_file, void* archivePtr, int file_id);
void LONG_CALL FS_InitFile(FSFile *p_file);
void* LONG_CALL FS_FindArchive(const char* name, int len);

void DC_FlushRange(const void *vAddr, u32 size);
void DC_InvalidateRange(void *vAddr, u32 size);


//static inline fx32 FX_MulInline(fx32 v1, fx32 v2) {
//    return FX32_CAST(((s64)(v1)*v2 + 0x800LL) >> FX32_SHIFT);
//}

#endif //!_NWAVPLAYER_H
