static int MP4_ReadBox_String( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_string_t );

    p_box->data.p_string->psz_text = malloc( p_box->i_size + 1 - 8 ); /* +\0, -name, -size */
    if( p_box->data.p_string->psz_text == NULL )
        MP4_READBOX_EXIT( 0 );

    memcpy( p_box->data.p_string->psz_text, p_peek, p_box->i_size - 8 );
    p_box->data.p_string->psz_text[p_box->i_size - 8] = '\0';

#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "read box: \"%4.4s\" text=`%s'", (char *) & p_box->i_type,
                 p_box->data.p_string->psz_text );
#endif
    MP4_READBOX_EXIT( 1 );
}