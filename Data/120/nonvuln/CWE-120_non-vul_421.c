static int is_ascii_name(const char *name, int *pc) {
  int i;
  for(i=0; i<32; i++) {
     if(stricmp(name,charnames[i])==0) {
          *pc=i;
          return 1;
     }
  }
  if(stricmp(name,"del")==0) {
     *pc=127;
     return 1;
  }
  return 0;
}