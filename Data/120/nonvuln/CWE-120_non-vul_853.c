void
free_separation_names(gs_memory_t * mem,
                gs_separations * pseparation)
{
    int i;

    /* Discard the sub levels. */
    for (i = 0; i < pseparation->num_separations; i++) {
        gs_free_object(mem->stable_memory, pseparation->names[i].data,
                                "free_separation_names");
        pseparation->names[i].data = NULL;
        pseparation->names[i].size = 0;
    }
    pseparation->num_separations = 0;
    return;
}