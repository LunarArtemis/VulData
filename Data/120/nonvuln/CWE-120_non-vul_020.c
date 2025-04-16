static RETSIGTYPE
child_cleanup(int signo _U_)
{
  wait(NULL);
}