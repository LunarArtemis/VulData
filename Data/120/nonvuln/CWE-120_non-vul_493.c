static const char *procname(pointer x) {
 int n=procnum(x);
 const char *name=dispatch_table[n].name;
 if(name==0) {
     name="ILLEGAL!";
 }
 return name;
}