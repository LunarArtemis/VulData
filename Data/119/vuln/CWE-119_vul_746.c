static void* TIDY_CALL defaultAlloc( TidyAllocator* allocator, size_t size )
{
    void *p = ( g_malloc ? g_malloc(size) : malloc(size) );
    if ( !p )
        defaultPanic( allocator,"Out of memory!");
    return p;
}