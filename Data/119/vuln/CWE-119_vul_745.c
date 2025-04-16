static void* TIDY_CALL defaultRealloc( TidyAllocator* allocator, void* mem, size_t newsize )
{
    void *p;
    if ( mem == NULL )
        return defaultAlloc( allocator, newsize );

    p = ( g_realloc ? g_realloc(mem, newsize) : realloc(mem, newsize) );
    if (!p)
        defaultPanic( allocator, "Out of memory!");
    return p;
}