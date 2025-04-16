void RenderView::layout()
{
    if (!configuration().paginated())
        setPageLogicalHeight(0);

    if (shouldUsePrintingLayout())
        m_minPreferredLogicalWidth = m_maxPreferredLogicalWidth = logicalWidth();

    SubtreeLayoutScope layoutScope(this);

    // Use calcWidth/Height to get the new width/height, since this will take the full page zoom factor into account.
    bool relayoutChildren = !shouldUsePrintingLayout() && (!m_frameView || width() != viewWidth() || height() != viewHeight());
    if (relayoutChildren) {
        layoutScope.setChildNeedsLayout(this);
        for (RenderObject* child = firstChild(); child; child = child->nextSibling()) {
            if ((child->isBox() && toRenderBox(child)->hasRelativeLogicalHeight())
                    || child->style()->logicalHeight().isPercent()
                    || child->style()->logicalMinHeight().isPercent()
                    || child->style()->logicalMaxHeight().isPercent()
                    || child->style()->logicalHeight().isViewportPercentage()
                    || child->style()->logicalMinHeight().isViewportPercentage()
                    || child->style()->logicalMaxHeight().isViewportPercentage()
                    || child->isSVGRoot())
                layoutScope.setChildNeedsLayout(child);
        }
    }

    ASSERT(!m_layoutState);
    if (!needsLayout())
        return;

    LayoutState state;
    bool isSeamlessAncestorInFlowThread = initializeLayoutState(state);

    m_pageLogicalHeightChanged = false;
    m_layoutState = &state;

    if (checkTwoPassLayoutForAutoHeightRegions())
        layoutContentInAutoLogicalHeightRegions(state);
    else
        layoutContent(state);

    if (m_frameView->partialLayout().isStopping()) {
        m_layoutState = 0;
        return;
    }

#ifndef NDEBUG
    checkLayoutState(state);
#endif
    m_layoutState = 0;
    clearNeedsLayout();

    if (isSeamlessAncestorInFlowThread)
        flowThreadController()->setCurrentRenderFlowThread(0);
}