static void TIDY_CALL defaultFree( TidyAllocator* ARG_UNUSED(allocator), void* mem )
{
    if ( mem )
    {
        if ( g_free )
            g_free( mem );
        else
            free( mem );
    }
}