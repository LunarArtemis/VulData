FT_LOCAL_DEF( FT_Error )
  tt_face_load_font_dir( TT_Face    face,
                         FT_Stream  stream )
  {
    SFNT_HeaderRec  sfnt;
    FT_Error        error;
    FT_Memory       memory = stream->memory;
    TT_TableRec*    entry;
    FT_Int          nn;

    static const FT_Frame_Field  offset_table_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  SFNT_HeaderRec

      FT_FRAME_START( 8 ),
        FT_FRAME_USHORT( num_tables ),
        FT_FRAME_USHORT( search_range ),
        FT_FRAME_USHORT( entry_selector ),
        FT_FRAME_USHORT( range_shift ),
      FT_FRAME_END
    };


    FT_TRACE2(( "tt_face_load_font_dir: %08p\n", face ));

    /* read the offset table */

    sfnt.offset = FT_STREAM_POS();

    if ( FT_READ_ULONG( sfnt.format_tag )                    ||
         FT_STREAM_READ_FIELDS( offset_table_fields, &sfnt ) )
      goto Exit;

    /* many fonts don't have these fields set correctly */
#if 0
    if ( sfnt.search_range != 1 << ( sfnt.entry_selector + 4 )        ||
         sfnt.search_range + sfnt.range_shift != sfnt.num_tables << 4 )
      return FT_THROW( Unknown_File_Format );
#endif

    /* load the table directory */

    FT_TRACE2(( "-- Number of tables: %10u\n",    sfnt.num_tables ));
    FT_TRACE2(( "-- Format version:   0x%08lx\n", sfnt.format_tag ));

    if ( sfnt.format_tag != TTAG_OTTO )
    {
      /* check first */
      error = check_table_dir( &sfnt, stream );
      if ( error )
      {
        FT_TRACE2(( "tt_face_load_font_dir:"
                    " invalid table directory for TrueType\n" ));

        goto Exit;
      }
    }

    face->num_tables = sfnt.num_tables;
    face->format_tag = sfnt.format_tag;

    if ( FT_QNEW_ARRAY( face->dir_tables, face->num_tables ) )
      goto Exit;

    if ( FT_STREAM_SEEK( sfnt.offset + 12 )       ||
         FT_FRAME_ENTER( face->num_tables * 16L ) )
      goto Exit;

    entry = face->dir_tables;

    FT_TRACE2(( "\n"
                "  tag    offset    length   checksum\n"
                "  ----------------------------------\n" ));

    for ( nn = 0; nn < sfnt.num_tables; nn++ )
    {
      entry->Tag      = FT_GET_TAG4();
      entry->CheckSum = FT_GET_ULONG();
      entry->Offset   = FT_GET_ULONG();
      entry->Length   = FT_GET_ULONG();

      /* ignore invalid tables */
      if ( entry->Offset + entry->Length > stream->size )
        continue;
      else
      {
        FT_TRACE2(( "  %c%c%c%c  %08lx  %08lx  %08lx\n",
                    (FT_Char)( entry->Tag >> 24 ),
                    (FT_Char)( entry->Tag >> 16 ),
                    (FT_Char)( entry->Tag >> 8  ),
                    (FT_Char)( entry->Tag       ),
                    entry->Offset,
                    entry->Length,
                    entry->CheckSum ));
        entry++;
      }
    }

    FT_FRAME_EXIT();

    FT_TRACE2(( "table directory loaded\n\n" ));

  Exit:
    return error;
  }