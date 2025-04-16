void RenderTable::simplifiedNormalFlowLayout()
{
    for (RenderTableSection* section = topSection(); section; section = sectionBelow(section)) {
        section->layoutIfNeeded();
        section->computeOverflowFromCells();
    }
}