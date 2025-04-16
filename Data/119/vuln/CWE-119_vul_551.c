bool HandleCoincidence(SkOpContourHead* contourList, SkOpCoincidence* coincidence,
        SkChunkAlloc* allocator) {
    SkOpGlobalState* globalState = contourList->globalState();
    // combine t values when multiple intersections occur on some segments but not others
    DEBUG_COINCIDENCE_HEALTH(contourList, "start");
    if (!moveMultiples(contourList)) {
        return false;
    }
    DEBUG_COINCIDENCE_HEALTH(contourList, "moveMultiples");
    findCollapsed(contourList);
    DEBUG_COINCIDENCE_HEALTH(contourList, "findCollapsed");
    // move t values and points together to eliminate small/tiny gaps
    moveNearby(contourList);
    DEBUG_COINCIDENCE_HEALTH(contourList, "moveNearby");
    align(contourList);  // give all span members common values
    DEBUG_COINCIDENCE_HEALTH(contourList, "align");
    coincidence->fixAligned();  // aligning may have marked a coincidence pt-t deleted
    DEBUG_COINCIDENCE_HEALTH(contourList, "fixAligned");
#if DEBUG_VALIDATE
    globalState->setPhase(SkOpGlobalState::kIntersecting);
#endif
    // look for intersections on line segments formed by moving end points
    addAlignIntersections(contourList, allocator);
    DEBUG_COINCIDENCE_HEALTH(contourList, "addAlignIntersections");
    if (coincidence->addMissing(allocator)) {
        DEBUG_COINCIDENCE_HEALTH(contourList, "addMissing");
        moveNearby(contourList);
        DEBUG_COINCIDENCE_HEALTH(contourList, "moveNearby2");
        align(contourList);  // give all span members common values
        DEBUG_COINCIDENCE_HEALTH(contourList, "align2");
        coincidence->fixAligned();  // aligning may have marked a coincidence pt-t deleted
        DEBUG_COINCIDENCE_HEALTH(contourList, "fixAligned2");
    }
#if DEBUG_VALIDATE
    globalState->setPhase(SkOpGlobalState::kWalking);
#endif
    // check to see if, loosely, coincident ranges may be expanded
    if (coincidence->expand()) {
        DEBUG_COINCIDENCE_HEALTH(contourList, "expand1");
        if (!coincidence->addExpanded(allocator  PATH_OPS_DEBUG_VALIDATE_PARAMS(globalState))) {
            return false;
        }
    }
    DEBUG_COINCIDENCE_HEALTH(contourList, "expand2");
    // the expanded ranges may not align -- add the missing spans
    coincidence->mark();  // mark spans of coincident segments as coincident
    DEBUG_COINCIDENCE_HEALTH(contourList, "mark1");
    // look for coincidence missed earlier
    if (missingCoincidence(contourList, coincidence, allocator)) {
        DEBUG_COINCIDENCE_HEALTH(contourList, "missingCoincidence1");
        (void) coincidence->expand();
        DEBUG_COINCIDENCE_HEALTH(contourList, "expand3");
        if (!coincidence->addExpanded(allocator  PATH_OPS_DEBUG_VALIDATE_PARAMS(globalState))) {
            return false;
        }
        DEBUG_COINCIDENCE_HEALTH(contourList, "addExpanded2");
        coincidence->mark();
    }
    DEBUG_COINCIDENCE_HEALTH(contourList, "missingCoincidence2");
    SkOpCoincidence overlaps;
    do {
        SkOpCoincidence* pairs = overlaps.isEmpty() ? coincidence : &overlaps;
        if (!pairs->apply()) {  // adjust the winding value to account for coincident edges
            return false;
        }
        DEBUG_COINCIDENCE_HEALTH(contourList, "pairs->apply");
        // For each coincident pair that overlaps another, when the receivers (the 1st of the pair)
        // are different, construct a new pair to resolve their mutual span
        pairs->findOverlaps(&overlaps, allocator);
        DEBUG_COINCIDENCE_HEALTH(contourList, "pairs->findOverlaps");
    } while (!overlaps.isEmpty());
    calcAngles(contourList, allocator);
    sortAngles(contourList);
    if (globalState->angleCoincidence()) {
        (void) missingCoincidence(contourList, coincidence, allocator);
        if (!coincidence->apply()) {
            return false;
        }
    }
#if DEBUG_ACTIVE_SPANS
    coincidence->debugShowCoincidence();
    DebugShowActiveSpans(contourList);
#endif
    return true;
}