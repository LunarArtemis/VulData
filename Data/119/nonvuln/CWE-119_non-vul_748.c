static void AddSubPST(SplineChar *sc,struct lookup_subtable *sub,char *variant) {
    PST *pst;

    pst = chunkalloc(sizeof(PST));
    pst->type = pst_substitution;
    pst->subtable = sub;
    pst->u.alt.components = copy(variant);
    pst->next = sc->possub;
    sc->possub = pst;
}