int writelogentry(PFTPCONTEXT context, const char *logtext1, const char *logtext2)
{
	char		cvbuf[32], _text[512];
	time_t		itm = time(NULL);
	struct tm	ltm;

	localtime_r(&itm, &ltm);

	_text[0] = 0;

	if ( ltm.tm_mday < 10 )
		strcat(_text, "0");
	ultostr(ltm.tm_mday, cvbuf);
	strcat(_text, cvbuf);
	strcat(_text, "-");

	if ( ltm.tm_mon+1 < 10 )
		strcat(_text, "0");
	ultostr(ltm.tm_mon+1, cvbuf);
	strcat(_text, cvbuf);
	strcat(_text, "-");

	ultostr(ltm.tm_year+1900, cvbuf);
	strcat(_text, cvbuf);
	strcat(_text, " ");

	if ( ltm.tm_hour < 10 )
		strcat(_text, "0");
	ultostr(ltm.tm_hour, cvbuf);
	strcat(_text, cvbuf);
	strcat(_text, ":");

	if ( ltm.tm_min < 10 )
		strcat(_text, "0");
	ultostr(ltm.tm_min, cvbuf);
	strcat(_text, cvbuf);
	strcat(_text, ":");

	if ( ltm.tm_sec < 10 )
		strcat(_text, "0");
	ultostr(ltm.tm_sec, cvbuf);
	strcat(_text, cvbuf);

	if (context) {
		strcat(_text, " S-id=");
		ultostr(context->SessionID, cvbuf);
		strcat(_text, cvbuf);
	}
	strcat(_text, ": ");

	if (logtext1)
		strcat(_text, logtext1);

	if (logtext2)
		strcat(_text, logtext2);

	strcat(_text, CRLF);

	return writeconsolestr(_text);
}