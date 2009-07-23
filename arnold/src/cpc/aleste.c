static int Aleste_ColourNumber;
static int Aleste_ColourPalette;
static int Aleste_Mode;
static int Aleste_Extport;

/* fabf */

void Aleste_Extport_Write(Z80_BYTE Data)
{
	


}


void Aleste_Multiport_Write(Z80_BYTE Data)
{
	switch (Data & 0x0c0)
	{
		case 0:
		{
			Aleste_ColourNumber = Data & 0x01f;
		}
		break;

		case 1:
		{
			switch (Aleste_Extport)
			{
				case 0:
				{

				}
				break;

			Aleste_ColourPalette = Data & 0x0
		}
		break;



}