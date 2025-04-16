int MacTS_main(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif
  scheme sc;
  FILE *fin;
  char *file_name=InitFile;
  int retcode;
  int isfile=1;

  if(argc==1) {
    printf(banner);
  }
  if(argc==2 && strcmp(argv[1],"-?")==0) {
    printf("Usage: tinyscheme -?\n");
    printf("or:    tinyscheme [<file1> <file2> ...]\n");
    printf("followed by\n");
    printf("          -1 <file> [<arg1> <arg2> ...]\n");
    printf("          -c <Scheme commands> [<arg1> <arg2> ...]\n");
    printf("assuming that the executable is named tinyscheme.\n");
    printf("Use - as filename for stdin.\n");
    return 1;
  }
  if(!scheme_init(&sc)) {
    fprintf(stderr,"Could not initialize!\n");
    return 2;
  }
  scheme_set_input_port_file(&sc, stdin);
  scheme_set_output_port_file(&sc, stdout);
#if USE_DL
  scheme_define(&sc,sc.global_env,mk_symbol(&sc,"load-extension"),mk_foreign_func(&sc, scm_load_ext));
#endif
  argv++;
  if(access(file_name,0)!=0) {
    char *p=getenv("TINYSCHEMEINIT");
    if(p!=0) {
      file_name=p;
    }
  }
  do {
    if(strcmp(file_name,"-")==0) {
      fin=stdin;
    } else if(strcmp(file_name,"-1")==0 || strcmp(file_name,"-c")==0) {
      pointer args=sc.NIL;
      isfile=file_name[1]=='1';
      file_name=*argv++;
      if(strcmp(file_name,"-")==0) {
        fin=stdin;
      } else if(isfile) {
        fin=fopen(file_name,"r");
      }
      for(;*argv;argv++) {
        pointer value=mk_string(&sc,*argv);
        args=cons(&sc,value,args);
      }
      args=reverse_in_place(&sc,sc.NIL,args);
      scheme_define(&sc,sc.global_env,mk_symbol(&sc,"*args*"),args);

    } else {
      fin=fopen(file_name,"r");
    }
    if(isfile && fin==0) {
      fprintf(stderr,"Could not open file %s\n",file_name);
    } else {
      if(isfile) {
        scheme_load_file(&sc,fin);
      } else {
        scheme_load_string(&sc,file_name);
      }
      if(!isfile || fin!=stdin) {
        if(sc.retcode!=0) {
          fprintf(stderr,"Errors encountered reading %s\n",file_name);
        }
        if(isfile) {
          fclose(fin);
        }
      }
    }
    file_name=*argv++;
  } while(file_name!=0);
  if(argc==1) {
    scheme_load_file(&sc,stdin);
  }
  retcode=sc.retcode;
  scheme_deinit(&sc);

  return retcode;
}