static int ScaKwdTab(GmfMshSct *msh)
{
   int      KwdCod, c;
   int64_t  NexPos, EndPos, LstPos;
   char     str[ GmfStrSiz ];

   if(msh->typ & Asc)
   {
      // Scan each string in the file until the end
      while(fscanf(msh->hdl, "%s", str) != EOF)
      {
         // Fast test in order to reject quickly the numeric values
         if(isalpha(str[0]))
         {
            // Search which kwd code this string is associated with, then get its
            // header and save the curent position in file (just before the data)
            for(KwdCod=1; KwdCod<= GmfMaxKwd; KwdCod++)
               if(!strcmp(str, GmfKwdFmt[ KwdCod ][0]))
               {
                  ScaKwdHdr(msh, KwdCod);
                  break;
               }
         }
         else if(str[0] == '#')
            while((c = fgetc(msh->hdl)) != '\n' && c != EOF);
      }
   }
   else
   {
      // Get file size
      EndPos = GetFilSiz(msh);
      LstPos = -1;

      // Jump through kwd positions in the file
      do
      {
         // Get the kwd code and the next kwd position
         ScaWrd(msh, ( char *)&KwdCod);
         NexPos = GetPos(msh);

         // Make sure the flow does not move beyond the file size
         if(NexPos > EndPos)
            longjmp(msh->err, -24);

         // And check that it does not move back
         if(NexPos && (NexPos <= LstPos))
            longjmp(msh->err, -30);

         LstPos = NexPos;

         // Check if this kwd belongs to this mesh version
         if( (KwdCod >= 1) && (KwdCod <= GmfMaxKwd) )
            ScaKwdHdr(msh, KwdCod);

         // Go to the next kwd
         if(NexPos && !(SetFilPos(msh, NexPos)))
            longjmp(msh->err, -25);

      }while(NexPos && (KwdCod != GmfEnd));
   }

   return(1);
}