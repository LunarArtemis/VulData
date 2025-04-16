int list_length(scheme *sc, pointer a) {
    int i=0;
    pointer slow, fast;

    slow = fast = a;
    while (1)
    {
        if (fast == sc->NIL)
                return i;
        if (!is_pair(fast))
                return -2 - i;
        fast = cdr(fast);
        ++i;
        if (fast == sc->NIL)
                return i;
        if (!is_pair(fast))
                return -2 - i;
        ++i;
        fast = cdr(fast);

    /* Safe because we would have already returned if `fast'
       encountered a non-pair. */
        slow = cdr(slow);
        if (fast == slow)
        {
            /* the fast pointer has looped back around and caught up
               with the slow pointer, hence the structure is circular,
               not of finite length, and therefore not a list */
            return -1;
        }
    }
}