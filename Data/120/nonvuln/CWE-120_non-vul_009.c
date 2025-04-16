RETSIGTYPE requestinfo(int signo _U_)
{
	if (infodelay)
		++infoprint;
	else
		info(0);
}