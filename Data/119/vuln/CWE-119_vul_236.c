void RenderSVGRoot::layout()
{
    ASSERT(needsLayout());

    m_resourcesNeedingToInvalidateClients.clear();

    // Arbitrary affine transforms are incompatible with LayoutState.
    LayoutStateDisabler layoutStateDisabler(view());

    bool needsLayout = selfNeedsLayout();
    LayoutRepainter repainter(*this, checkForRepaintDuringLayout() && needsLayout);

    LayoutSize oldSize = size();
    updateLogicalWidth();
    updateLogicalHeight();
    buildLocalToBorderBoxTransform();

    SVGSVGElement* svg = toSVGSVGElement(node());
    ASSERT(svg);
    m_isLayoutSizeChanged = needsLayout || (svg->hasRelativeLengths() && oldSize != size());
    SVGRenderSupport::layoutChildren(this, needsLayout || SVGRenderSupport::filtersForceContainerLayout(this));

    if (!m_resourcesNeedingToInvalidateClients.isEmpty()) {
        // Invalidate resource clients, which may mark some nodes for layout.
        HashSet<RenderSVGResourceContainer*>::iterator end = m_resourcesNeedingToInvalidateClients.end();
        for (HashSet<RenderSVGResourceContainer*>::iterator it = m_resourcesNeedingToInvalidateClients.begin(); it != end; ++it)
            (*it)->removeAllClientsFromCache();

        m_isLayoutSizeChanged = false;
        SVGRenderSupport::layoutChildren(this, false);
    }

    // At this point LayoutRepainter already grabbed the old bounds,
    // recalculate them now so repaintAfterLayout() uses the new bounds.
    if (m_needsBoundariesOrTransformUpdate) {
        updateCachedBoundaries();
        m_needsBoundariesOrTransformUpdate = false;
    }

    updateLayerTransform();

    repainter.repaintAfterLayout();

    clearNeedsLayout();
}