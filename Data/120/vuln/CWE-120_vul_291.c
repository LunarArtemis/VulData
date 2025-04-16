int yc_decrypt(char *fbuf, unsigned int filesize, struct cli_exe_section *sections, unsigned int sectcount, uint32_t peoffset, int desc, uint32_t ecx,int16_t offset) {
  uint32_t ycsect = sections[sectcount].raw+offset;
  unsigned int i;
  struct pe_image_file_hdr *pe = (struct pe_image_file_hdr*) (fbuf + peoffset);
  char *sname = (char *)pe + EC16(pe->SizeOfOptionalHeader) + 0x18;

  /* 

  First layer (decryptor of the section decryptor) in last section 

  Start offset for analyze: Start of yC Section + 0x93
  End offset for analyze: Start of yC Section + 0xC3
  Length to decrypt - ECX = 0xB97

  */
  cli_dbgmsg("yC: offset: %x, length: %x\n", offset, ecx);
  cli_dbgmsg("yC: decrypting decryptor on sect %d\n", sectcount);
  if (yc_poly_emulator(fbuf + ycsect + 0x93, fbuf + ycsect + 0xc6, ecx))
    return 1;
  filesize-=sections[sectcount].ursz;

  /* 

  Second layer (decryptor of the sections) in last section 

  Start offset for analyze: Start of yC Section + 0x457
  End offset for analyze: Start of yC Section + 0x487
  Lenght to decrypt - ECX = Raw Size of Section

  */


  /* Loop through all sections and decrypt them... */
  for(i=0;i<sectcount;i++)
    {
      uint32_t name = (uint32_t) cli_readint32(sname+i*0x28);
      if (!sections[i].raw ||
	  !sections[i].rsz ||
	   name == 0x63727372 || /* rsrc */
	   name == 0x7273722E || /* .rsr */
	   name == 0x6F6C6572 || /* relo */
	   name == 0x6C65722E || /* .rel */
	   name == 0x6164652E || /* .eda */
	   name == 0x6164722E || /* .rda */
	   name == 0x6164692E || /* .ida */
	   name == 0x736C742E || /* .tls */
	   (name&0xffff) == 0x4379  /* yC */
	) continue;
      cli_dbgmsg("yC: decrypting sect%d\n",i);
      if (yc_poly_emulator(fbuf + ycsect + (offset == -0x18 ? 0x3ea : 0x457), fbuf + sections[i].raw, sections[i].ursz))
	  return 1;
    }

  /* Remove yC section */
  pe->NumberOfSections=EC16(sectcount);

  /* Remove IMPORT_DIRECTORY information */
  memset((char *)pe + sizeof(struct pe_image_file_hdr) + 0x68, 0, 8);

  /* OEP resolving */
  /* OEP = DWORD PTR [ Start of yC section+ A0F] */
  cli_writeint32((char *)pe + sizeof(struct pe_image_file_hdr) + 16, cli_readint32(fbuf + ycsect + 0xa0f));

  /* Fix SizeOfImage */
  cli_writeint32((char *)pe + sizeof(struct pe_image_file_hdr) + 0x38, cli_readint32((char *)pe + sizeof(struct pe_image_file_hdr) + 0x38) - sections[sectcount].vsz);

  if (cli_writen(desc, fbuf, filesize)==-1) {
    cli_dbgmsg("yC: Cannot write unpacked file\n");
    return 1;
  }
  return 0;
}