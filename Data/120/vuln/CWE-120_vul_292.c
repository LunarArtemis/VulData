static int yc_poly_emulator(char* decryptor_offset, char* code, unsigned int ecx)
{

  /* 
     This is the instruction set of the poly code.
     Numbers stand for example only.

     2C 05            SUB AL,5
     2AC1             SUB AL,CL
     34 10            XOR AL,10
     32C1             XOR AL,CL
     FEC8             DEC AL
     04 10            ADD AL,10
     02C1             ADD AL,CL
     C0C0 06          ROL AL,6
     C0C8 05          ROR AL,5
     D2C8             ROR AL,CL
     D2C0             ROL AL,CL

  */
  unsigned char al;
  unsigned char cl = ecx & 0xff;
  unsigned int j,i;

  for(i=0;i<ecx;i++) /* Byte looper - Decrypts every byte and write it back */
    {
      al = code[i];

      for(j=0;j<0x30;j++)   /* Poly Decryptor "Emulator" */
	{
	  switch(decryptor_offset[j])
	    {

	    case '\xEB':	/* JMP short */
	      j++;
	      j = j + decryptor_offset[j];
	      break;

	    case '\xFE':	/* DEC  AL */
	      al--;
	      j++;
	      break;

	    case '\x2A':	/* SUB AL,CL */
	      al = al - cl;
	      j++;
	      break;

	    case '\x02':	/* ADD AL,CL */
	      al = al + cl;
	      j++;
	      break
		;
	    case '\x32':	/* XOR AL,CL */
	      al = al ^ cl;
	      j++;
	      break;
	      ;
	    case '\x04':	/* ADD AL,num */
	      j++;
	      al = al + decryptor_offset[j];
	      break;
	      ;
	    case '\x34':	/* XOR AL,num */
	      j++;
	      al = al ^ decryptor_offset[j];
	      break;

	    case '\x2C':	/* SUB AL,num */
	      j++;
	      al = al - decryptor_offset[j];
	      break;

			
	    case '\xC0':
	      j++;
	      if(decryptor_offset[j]=='\xC0') /* ROL AL,num */
		{
		  j++;
		  CLI_ROL(al,decryptor_offset[j]);
		}
	      else			/* ROR AL,num */
		{
		  j++;
		  CLI_ROR(al,decryptor_offset[j]);
		}
	      break;

	    case '\xD2':
	      j++;
	      if(decryptor_offset[j]=='\xC8') /* ROR AL,CL */
		{
		  j++;
		  CLI_ROR(al,cl);
		}
	      else			/* ROL AL,CL */
		{
		  j++;
		  CLI_ROL(al,cl);
		}
	      break;

	    case '\x90':
	    case '\xf8':
	    case '\xf9':
	      break;

	    default:
	      cli_dbgmsg("yC: Unhandled opcode %x\n", (unsigned char)decryptor_offset[j]);
	      return 1;
	    }
	}
      cl--;
      code[i] = al;
    }
  return 0;

}