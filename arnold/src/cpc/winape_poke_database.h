#ifndef __WINAPE_POKE_DATABASE__
#define __WINAPE_POKE_DATABASE__

void WinapePokeDatabase_Init(const char *pWinapePokeDatabase);

void WinapePokeDatabase_ApplyPoke(int nGame, int nPoke, const char *pPoke);

void WinapePokeDatabase_Free(void);

#endif
