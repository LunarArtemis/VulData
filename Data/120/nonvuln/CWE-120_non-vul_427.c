int main(int argc, char **argv)
{
     extern MacTS_main(int argc, char **argv);
     char**    argv;
     int argc = ccommand(&argv);
     MacTS_main(argc,argv);
     return 0;
}