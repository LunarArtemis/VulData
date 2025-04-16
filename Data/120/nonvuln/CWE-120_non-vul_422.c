static int count_consecutive_cells(pointer x, int needed) {
 int n=1;
 while(cdr(x)==x+1) {
     x=cdr(x);
     n++;
     if(n>needed) return n;
 }
 return n;
}