static int GTBackPos(GTextField *gt,int pos, int ismeta) {
    int newpos;

    if ( ismeta )
	newpos = GTextFieldSelBackword(gt->text,pos);
    else
	newpos = pos-1;
    if ( newpos==-1 ) newpos = pos;
return( newpos );
}