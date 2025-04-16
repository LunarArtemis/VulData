static void unref_ft_library() {
    gFTMutex.assertHeld();
    SkASSERT(gFTCount > 0);

    --gFTCount;
    if (0 == gFTCount) {
        SkASSERT(nullptr != gFTLibrary);
        delete gFTLibrary;
        SkDEBUGCODE(gFTLibrary = nullptr;)
    }
}