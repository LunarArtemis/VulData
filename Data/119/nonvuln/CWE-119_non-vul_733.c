static int ss_e_h(GWindow gw, GEvent *event) {
    int *done = GDrawGetUserData(gw);

    switch ( event->type ) {
      case et_char:
return( false );
      case et_close:
	*done = true;
      break;
    }
return( true );
}