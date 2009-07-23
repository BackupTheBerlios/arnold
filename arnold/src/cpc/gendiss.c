#include "cpc.h"

char Diss_GetHexDigitAsChar(unsigned char HexDigit)
{
    HexDigit &=0x0f;
    if ((HexDigit>=0) && (HexDigit<=9))
        return '0'+HexDigit;
    return 'A'+(HexDigit-10);
}


char *Diss_WriteHexByte(char *pString, unsigned char Value, BOOL bPrefix)
{
	unsigned char HexDigit;

	if (bPrefix)
	{
		/* write identifier to indicate hex value */
		*pString = '#';
		++pString;
	}

	/* write high digit */
	HexDigit = Value>>4;

	*pString = Diss_GetHexDigitAsChar(HexDigit);
	++pString;
	HexDigit = Value & 0x0f;

	*pString = Diss_GetHexDigitAsChar(HexDigit);
	++pString;
	return pString;
}


char *Diss_WriteHexWord(char *pString, unsigned short Value, BOOL bPrefix)
{
	unsigned char HexDigit;

	if (bPrefix)
	{
		/* write identifier to indicate hex value */
		*pString = '#';
		++pString;
	}

	HexDigit = Value>>12;

	*pString = Diss_GetHexDigitAsChar(HexDigit);
	++pString;

	HexDigit = Value>>8;

	*pString = Diss_GetHexDigitAsChar(HexDigit);
	++pString;

	HexDigit = Value>>4;

	*pString = Diss_GetHexDigitAsChar(HexDigit);
	++pString;

	HexDigit = Value;

	*pString = Diss_GetHexDigitAsChar(HexDigit);
	++pString;

    return pString;
}


static char *Diss_char(char *pString, const char ch)
{
        pString[0] = ch;
        ++pString;
        return pString;
}

char *Diss_space(char *pString)
{
	return Diss_char(pString,' ');
}

char *Diss_comma(char *pString)
{
	return Diss_char(pString,',');
}

char *Diss_colon(char *pString)
{
	return Diss_char(pString,':');
}

char *Diss_endstring(char *pString)
{
    pString[0] = '\0';
    ++pString;
    return pString;
}

char *Diss_strcat(char *pString, const char *pToken)
{
    int nTokenLength = strlen(pToken);
    strncpy(pString, pToken, nTokenLength);
    pString += nTokenLength;
    return pString;
}
