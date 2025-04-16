void SkOpCoincidence::mark() {
    SkCoincidentSpans* coin = fHead;
    if (!coin) {
        return;
    }
    do {
        SkOpSpanBase* end = coin->fCoinPtTEnd->span();
        SkOpSpanBase* oldEnd = end;
        SkOpSpan* start = coin->fCoinPtTStart->span()->starter(&end);
        SkOpSpanBase* oEnd = coin->fOppPtTEnd->span();
        SkOpSpanBase* oOldEnd = oEnd;
        SkOpSpanBase* oStart = coin->fOppPtTStart->span()->starter(&oEnd);
        bool flipped = (end == oldEnd) != (oEnd == oOldEnd);
        if (flipped) {
            SkTSwap(oStart, oEnd);
        }
        SkOpSpanBase* next = start;
        SkOpSpanBase* oNext = oStart;
        do {
            next = next->upCast()->next();
            oNext = flipped ? oNext->prev() : oNext->upCast()->next();
            if (next == end || oNext == oEnd) {
                break;
            }
            if (!next->containsCoinEnd(oNext)) {
                next->insertCoinEnd(oNext);
            }
            SkOpSpan* nextSpan = next->upCast();
            SkOpSpan* oNextSpan = oNext->upCast();
            if (!nextSpan->containsCoincidence(oNextSpan)) {
                nextSpan->insertCoincidence(oNextSpan);
            }
        } while (true);
    } while ((coin = coin->fNext));
}