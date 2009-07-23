#include "z80.h"
#include "z80macros.h"
#if 0
Z80_WORD StepOutInstruction()
{
	/* Run to instruction after DJNZ? */


	return 0;
}
#endif

Z80_WORD StepIntoInstruction()
{
	/* get current instruction */
	Z80_BYTE Instruction = Z80_RD_MEM(Z80_GetReg(Z80_PC));

	switch (Instruction)
	{
		/* DJNZ */
		case 0x010:
		{
			if (((Z80_GetReg(Z80_BC)>>8)&0x0ff)==1)
			{
				return Z80_GetReg(Z80_PC) + 2;
			}
			else
			{
				/* calc addr and go to it */
				Z80_BYTE_OFFSET Offset;

				Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

				return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
			}
		}
		break;

		/* JR  */
		case 0x018:
		{
			/* calc addr and go to it */
			Z80_BYTE_OFFSET Offset;

			Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

			return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
		}
		break;

		/* JR NZ, */
		case 0x020:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)!=0)
			{
				/* calc addr and go to it */
				Z80_BYTE_OFFSET Offset;

				Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

				return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
			}
		}
		break;

		/* JR Z, */
		case 0x028:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)==0)
			{
				/* calc addr and go to it */
				Z80_BYTE_OFFSET Offset;

				Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

				return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
			}
		}
		break;


		/* JR NC */
		case 0x030:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)==0)
			{
				/* calc addr and go to it */
				Z80_BYTE_OFFSET Offset;

				Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

				return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
			}
		}
		break;

		/* JR C */
		case 0x038:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)!=0)
			{
				/* calc addr and go to it */
				Z80_BYTE_OFFSET Offset;

				Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

				return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
			}
		}
		break;

        /* HALT */
        case 0x076:
        {
            return Z80_GetReg(Z80_PC);
        }
        break;

        /* RET NZ */
		case 0x0c0:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

        /* RET Z */
		case 0x0c8:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET */
		case 0x0c9:
			return Z80_RD_WORD(Z80_GetReg(Z80_SP));


		/* RET NC */
		case 0x0d0:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET C */
		case 0x0d8:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET PO */
		case 0x0e0:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET PE */
		case 0x0e8:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET P */
		case 0x0f0:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET M */
		case 0x0f8:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;





		/* RST 0 */
		case 0x0c7:
			return 0x0;

		/* RST 8 */
		case 0x0cf:
			return 0x08;

		/* RST 10 */
		case 0x0d7:
			return 0x010;

		/* RST 18 */
		case 0x0df:
			return 0x018;

		/* RST 20 */
		case 0x0e7:
			return 0x020;

		/* RST 28 */
		case 0x0ef:
			return 0x028;

		/* RST 30 */
		case 0x0f7:
			return 0x030;

		/* RST 38 */
		case 0x0ff:
			return 0x038;


		/* JP (HL) */
		case 0x0e9:
			return Z80_GetReg(Z80_HL);

		case 0x0fd:
		case 0x0dd:
		{
			Z80_BYTE Instruction2 = Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);
			switch (Instruction2)
			{
				case 0x0e9:
				{
					if (Instruction==0x0dd)
						return Z80_GetReg(Z80_IX);

					return Z80_GetReg(Z80_IY);
				}
				break;

				default:
					break;
			}
		}
		break;




		case 0x0ed:
		{
			Z80_BYTE Instruction2 = Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);
			switch (Instruction2)
			{
                /* RETN, RETI*/
				case 0x045:
				case 0x04d:
				case 0x055:
				case 0x05d:
				case 0x065:
				case 0x06d:
				case 0x075:
				case 0x07d:
				{
					return Z80_RD_WORD(Z80_GetReg(Z80_SP));
				}
				break;

				/* ldir */
				/* cpir */
				/* inir */
				/* otir */
				/* lddr */
				/* cpdr */
				/* indr */
				/* otdr */
				case 0x0b0:
				case 0x0b1:
				case 0x0b2:
				case 0x0b3:
				case 0x0b8:
				case 0x0b9:
				case 0x0ba:
				case 0x0bb:
				{
				    if (Z80_GetReg(Z80_BC)!=1)
                    {
                        return Z80_GetReg(Z80_PC);
                    }

                    return Z80_GetReg(Z80_PC)+2;

				}
				break;

				default:
					break;
			}


		}
		break;

		/* CALL NZ,nnnn */
		case 0x0c4:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* CALL Z,nnnn */
		case 0x0cc:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* CALL nc,nnnn */
		case 0x0d4:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* CALL C,nnnn */
		case 0x0dc:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* CALL PO,nnnn */
		case 0x0e4:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;


		/* CALL PE,nnnn */
		case 0x0ec:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* CALL P,nnnn */
		case 0x0f4:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* CALL M,nnnn */
		case 0x0fc:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* CALL nnnn */
		case 0x0cd:
			return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);

		/* JP nnnn */
		case 0x0c3:
			return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);

        /* JP nz,nnnn */
		case 0x0ca:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

    /* JP nz,nnnn */
		case 0x0c2:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* JP nc,nnnn */
		case 0x0d2:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;


		/* JP c,nnnn */
		case 0x0da:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* JP PO,nnnn */
		case 0x0e2:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* JP PE,nnnn */
		case 0x0EA:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;


		/* JP P,nnnn */
		case 0x0f2:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* JP M,nnnn */
		case 0x0fA:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

     default:
            break;

	}

	{
		int nBytes = Debug_GetOpcodeCount(Z80_GetReg(Z80_PC));
		return Z80_GetReg(Z80_PC)+nBytes;
	}
}

Z80_WORD StepOverInstruction()
{
	/* get current instruction */
	Z80_BYTE Instruction = Z80_RD_MEM(Z80_GetReg(Z80_PC));

	switch (Instruction)
	{
		/* JP (HL) */
		case 0x0e9:
			return Z80_GetReg(Z80_HL);

		case 0x0fd:
		case 0x0dd:
		{
			Z80_BYTE Instruction2 = Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);
			switch (Instruction2)
			{
				case 0x0e9:
				{
					if (Instruction==0x0dd)
						return Z80_GetReg(Z80_IX);

					return Z80_GetReg(Z80_IY);
				}
				break;

				default:
					break;
			}
		}
		break;


		/* JR  */
		case 0x018:
		{
			/* calc addr and go to it */
			Z80_BYTE_OFFSET Offset;

			Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

			return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
		}
		break;

		/* JR NZ, */
		case 0x020:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)!=0)
			{
				/* calc addr and go to it */
				Z80_BYTE_OFFSET Offset;

				Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

				return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
			}
		}
		break;

		/* JR Z, */
		case 0x028:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)==0)
			{
				/* calc addr and go to it */
				Z80_BYTE_OFFSET Offset;

				Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

				return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
			}
		}
		break;


		/* JR NC */
		case 0x030:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)==0)
			{
				/* calc addr and go to it */
				Z80_BYTE_OFFSET Offset;

				Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

				return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
			}
		}
		break;

		/* JR C */
		case 0x038:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)!=0)
			{
				/* calc addr and go to it */
				Z80_BYTE_OFFSET Offset;

				Offset = (Z80_BYTE_OFFSET)Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);

				return Z80_GetReg(Z80_PC) + (Z80_LONG)2 + Offset;
			}
		}
		break;

		/* RETN, RETI*/
		case 0x0ed:
		{
			Z80_BYTE Instruction2 = Z80_RD_MEM(Z80_GetReg(Z80_PC)+1);
			switch (Instruction2)
			{
				case 0x045:
				case 0x04d:
				case 0x055:
				case 0x05d:
				case 0x065:
				case 0x06d:
				case 0x075:
				case 0x07d:
				{
					return Z80_RD_WORD(Z80_GetReg(Z80_SP));
				}
				break;

				default:
					break;
			}


		}
		break;

        /* RET NZ */
		case 0x0c0:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

        /* RET Z */
		case 0x0c8:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET */
		case 0x0c9:
			return Z80_RD_WORD(Z80_GetReg(Z80_SP));


		/* RET NC */
		case 0x0d0:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET C */
		case 0x0d8:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET PO */
		case 0x0e0:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET PE */
		case 0x0e8:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET P */
		case 0x0f0:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* RET M */
		case 0x0f8:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_SP));
		}
		break;

		/* JP nnnn */
		case 0x0c3:
			return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);

    /* JP nz,nnnn */
		case 0x0c2:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

        /* JP z,nnnn */
		case 0x0ca:
		{
			if ((Z80_GetReg(Z80_F) & Z80_ZERO_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;


		/* JP nc,nnnn */
		case 0x0d2:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;


		/* JP c,nnnn */
		case 0x0da:
		{
			if ((Z80_GetReg(Z80_F) & Z80_CARRY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* JP PO,nnnn */
		case 0x0e2:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* JP PE,nnnn */
		case 0x0EA:
		{
			if ((Z80_GetReg(Z80_F) & Z80_PARITY_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;


		/* JP P,nnnn */
		case 0x0f2:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)==0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

		/* JP M,nnnn */
		case 0x0fA:
		{
			if ((Z80_GetReg(Z80_F) & Z80_SIGN_FLAG)!=0)
				return Z80_RD_WORD(Z80_GetReg(Z80_PC)+1);
		}
		break;

        default:
            break;


	}

	{
		int nBytes = Debug_GetOpcodeCount(Z80_GetReg(Z80_PC));
		return Z80_GetReg(Z80_PC)+nBytes;
	}
}

// get address after current opcode and run to that address?
