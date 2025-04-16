static int syntaxnum(pointer p) {
     const char *s=strvalue(car(p));
     switch(strlength(car(p))) {
     case 2:
          if(s[0]=='i') return OP_IF0;        /* if */
          else return OP_OR0;                 /* or */
     case 3:
          if(s[0]=='a') return OP_AND0;      /* and */
          else return OP_LET0;               /* let */
     case 4:
          switch(s[3]) {
          case 'e': return OP_CASE0;         /* case */
          case 'd': return OP_COND0;         /* cond */
          case '*': return OP_LET0AST;       /* let* */
          default: return OP_SET0;           /* set! */
          }
     case 5:
          switch(s[2]) {
          case 'g': return OP_BEGIN;         /* begin */
          case 'l': return OP_DELAY;         /* delay */
          case 'c': return OP_MACRO0;        /* macro */
          default: return OP_QUOTE;          /* quote */
          }
     case 6:
          switch(s[2]) {
          case 'm': return OP_LAMBDA;        /* lambda */
          case 'f': return OP_DEF0;          /* define */
          default: return OP_LET0REC;        /* letrec */
          }
     default:
          return OP_C0STREAM;                /* cons-stream */
     }
}