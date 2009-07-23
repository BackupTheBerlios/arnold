#ifndef __JUKEBOX_HEADER_INCLUDED__
#define __JUKEBOX_HEADER_INCLUDED__

void Jukebox_Reset();
void Jukebox_Finish();
void Jukebox_Init();
void Jukebox_Write(Z80_WORD Addr, Z80_BYTE Data);
void Jukebox_CartridgeInsert(int nCartridge, char *pFilename);
void Jukebox_SetCartridgeEnable(int nCartridge, BOOL fEnable);
BOOL Jukebox_IsCartridgeEnabled(int nCartridge);
void Jukebox_InsertSystemCartridge(const unsigned char *pCartridgeData, const unsigned long CartridgeLength);
void Jukebox_Enable(BOOL bEnable);
BOOL Jukebox_IsEnabled();


#endif