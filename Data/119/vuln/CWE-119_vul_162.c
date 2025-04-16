static FT_Error
  _bdf_parse_start( char*          line,
                    unsigned long  linelen,
                    unsigned long  lineno,
                    void*          call_data,
                    void*          client_data )
  {
    unsigned long      slen;
    _bdf_line_func_t*  next;
    _bdf_parse_t*      p;
    bdf_font_t*        font;
    char               *s;

    FT_Memory          memory = NULL;
    FT_Error           error  = BDF_Err_Ok;

    FT_UNUSED( lineno );            /* only used in debug mode */


    next = (_bdf_line_func_t *)call_data;
    p    = (_bdf_parse_t *)    client_data;

    if ( p->font )
      memory = p->font->memory;

    /* Check for a comment.  This is done to handle those fonts that have */
    /* comments before the STARTFONT line for some reason.                */
    if ( ft_memcmp( line, "COMMENT", 7 ) == 0 )
    {
      if ( p->opts->keep_comments != 0 && p->font != 0 )
      {
        linelen -= 7;

        s = line + 7;
        if ( *s != 0 )
        {
          s++;
          linelen--;
        }

        error = _bdf_add_comment( p->font, s, linelen );
        if ( error )
          goto Exit;
        /* here font is not defined! */
      }

      goto Exit;
    }

    if ( !( p->flags & _BDF_START ) )
    {
      memory = p->memory;

      if ( ft_memcmp( line, "STARTFONT", 9 ) != 0 )
      {
        /* we don't emit an error message since this code gets */
        /* explicitly caught one level higher                  */
        error = BDF_Err_Missing_Startfont_Field;
        goto Exit;
      }

      p->flags = _BDF_START;
      font = p->font = 0;

      if ( FT_NEW( font ) )
        goto Exit;
      p->font = font;

      font->memory = p->memory;
      p->memory    = 0;

      { /* setup */
        size_t           i;
        bdf_property_t*  prop;


        error = hash_init( &(font->proptbl), memory );
        if ( error )
          goto Exit;
        for ( i = 0, prop = (bdf_property_t*)_bdf_properties;
              i < _num_bdf_properties; i++, prop++ )
        {
          error = hash_insert( prop->name, i,
                               &(font->proptbl), memory );
          if ( error )
            goto Exit;
        }
      }

      if ( FT_ALLOC( p->font->internal, sizeof ( hashtable ) ) )
        goto Exit;
      error = hash_init( (hashtable *)p->font->internal,memory );
      if ( error )
        goto Exit;
      p->font->spacing      = p->opts->font_spacing;
      p->font->default_char = -1;

      goto Exit;
    }

    /* Check for the start of the properties. */
    if ( ft_memcmp( line, "STARTPROPERTIES", 15 ) == 0 )
    {
      if ( !( p->flags & _BDF_FONT_BBX ) )
      {
        /* Missing the FONTBOUNDINGBOX field. */
        FT_ERROR(( "_bdf_parse_start: " ERRMSG1, lineno, "FONTBOUNDINGBOX" ));
        error = BDF_Err_Missing_Fontboundingbox_Field;
        goto Exit;
      }

      error = _bdf_list_split( &p->list, (char *)" +", line, linelen );
      if ( error )
        goto Exit;
      /* at this point, `p->font' can't be NULL */
      p->cnt = p->font->props_size = _bdf_atoul( p->list.field[1], 0, 10 );

      if ( FT_NEW_ARRAY( p->font->props, p->cnt ) )
        goto Exit;

      p->flags |= _BDF_PROPS;
      *next     = _bdf_parse_properties;

      goto Exit;
    }

    /* Check for the FONTBOUNDINGBOX field. */
    if ( ft_memcmp( line, "FONTBOUNDINGBOX", 15 ) == 0 )
    {
      if ( !( p->flags & _BDF_SIZE ) )
      {
        /* Missing the SIZE field. */
        FT_ERROR(( "_bdf_parse_start: " ERRMSG1, lineno, "SIZE" ));
        error = BDF_Err_Missing_Size_Field;
        goto Exit;
      }

      error = _bdf_list_split( &p->list, (char *)" +", line, linelen );
      if ( error )
        goto Exit;

      p->font->bbx.width  = _bdf_atos( p->list.field[1], 0, 10 );
      p->font->bbx.height = _bdf_atos( p->list.field[2], 0, 10 );

      p->font->bbx.x_offset = _bdf_atos( p->list.field[3], 0, 10 );
      p->font->bbx.y_offset = _bdf_atos( p->list.field[4], 0, 10 );

      p->font->bbx.ascent  = (short)( p->font->bbx.height +
                                      p->font->bbx.y_offset );

      p->font->bbx.descent = (short)( -p->font->bbx.y_offset );

      p->flags |= _BDF_FONT_BBX;

      goto Exit;
    }

    /* The next thing to check for is the FONT field. */
    if ( ft_memcmp( line, "FONT", 4 ) == 0 )
    {
      error = _bdf_list_split( &p->list, (char *)" +", line, linelen );
      if ( error )
        goto Exit;
      _bdf_list_shift( &p->list, 1 );

      s = _bdf_list_join( &p->list, ' ', &slen );

      if ( !s )
      {
        FT_ERROR(( "_bdf_parse_start: " ERRMSG8, lineno, "FONT" ));
        error = BDF_Err_Invalid_File_Format;
        goto Exit;
      }

      /* Allowing multiple `FONT' lines (which is invalid) doesn't hurt... */
      FT_FREE( p->font->name );

      if ( FT_NEW_ARRAY( p->font->name, slen + 1 ) )
        goto Exit;
      FT_MEM_COPY( p->font->name, s, slen + 1 );

      /* If the font name is an XLFD name, set the spacing to the one in  */
      /* the font name.  If there is no spacing fall back on the default. */
      error = _bdf_set_default_spacing( p->font, p->opts, lineno );
      if ( error )
        goto Exit;

      p->flags |= _BDF_FONT_NAME;

      goto Exit;
    }

    /* Check for the SIZE field. */
    if ( ft_memcmp( line, "SIZE", 4 ) == 0 )
    {
      if ( !( p->flags & _BDF_FONT_NAME ) )
      {
        /* Missing the FONT field. */
        FT_ERROR(( "_bdf_parse_start: " ERRMSG1, lineno, "FONT" ));
        error = BDF_Err_Missing_Font_Field;
        goto Exit;
      }

      error = _bdf_list_split( &p->list, (char *)" +", line, linelen );
      if ( error )
        goto Exit;

      p->font->point_size   = _bdf_atoul( p->list.field[1], 0, 10 );
      p->font->resolution_x = _bdf_atoul( p->list.field[2], 0, 10 );
      p->font->resolution_y = _bdf_atoul( p->list.field[3], 0, 10 );

      /* Check for the bits per pixel field. */
      if ( p->list.used == 5 )
      {
        unsigned short bitcount, i, shift;


        p->font->bpp = (unsigned short)_bdf_atos( p->list.field[4], 0, 10 );

        /* Only values 1, 2, 4, 8 are allowed. */
        shift = p->font->bpp;
        bitcount = 0;
        for ( i = 0; shift > 0; i++ )
        {
          if ( shift & 1 )
            bitcount = i;
          shift >>= 1;
        }

        shift = (short)( ( bitcount > 3 ) ? 8 : ( 1 << bitcount ) );

        if ( p->font->bpp > shift || p->font->bpp != shift )
        {
          /* select next higher value */
          p->font->bpp = (unsigned short)( shift << 1 );
          FT_TRACE2(( "_bdf_parse_start: " ACMSG11, p->font->bpp ));
        }
      }
      else
        p->font->bpp = 1;

      p->flags |= _BDF_SIZE;

      goto Exit;
    }

    /* Check for the CHARS field -- font properties are optional */
    if ( ft_memcmp( line, "CHARS", 5 ) == 0 )
    {
      char  nbuf[128];


      if ( !( p->flags & _BDF_FONT_BBX ) )
      {
        /* Missing the FONTBOUNDINGBOX field. */
        FT_ERROR(( "_bdf_parse_start: " ERRMSG1, lineno, "FONTBOUNDINGBOX" ));
        error = BDF_Err_Missing_Fontboundingbox_Field;
        goto Exit;
      }

      /* Add the two standard X11 properties which are required */
      /* for compiling fonts.                                   */
      p->font->font_ascent = p->font->bbx.ascent;
      ft_sprintf( nbuf, "%hd", p->font->bbx.ascent );
      error = _bdf_add_property( p->font, (char *)"FONT_ASCENT",
                                 nbuf, lineno );
      if ( error )
        goto Exit;
      FT_TRACE2(( "_bdf_parse_properties: " ACMSG1, p->font->bbx.ascent ));

      p->font->font_descent = p->font->bbx.descent;
      ft_sprintf( nbuf, "%hd", p->font->bbx.descent );
      error = _bdf_add_property( p->font, (char *)"FONT_DESCENT",
                                 nbuf, lineno );
      if ( error )
        goto Exit;
      FT_TRACE2(( "_bdf_parse_properties: " ACMSG2, p->font->bbx.descent ));

      p->font->modified = 1;

      *next = _bdf_parse_glyphs;

      /* A special return value. */
      error = -1;
      goto Exit;
    }

    FT_ERROR(( "_bdf_parse_start: " ERRMSG9, lineno ));
    error = BDF_Err_Invalid_File_Format;

  Exit:
    return error;
  }