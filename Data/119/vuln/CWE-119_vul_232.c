void RenderSVGResourceMarker::layout()
{
    // Invalidate all resources if our layout changed.
    if (everHadLayout() && selfNeedsLayout())
        RenderSVGRoot::addResourceForClientInvalidation(this);

    // RenderSVGHiddenContainer overwrites layout(). We need the
    // layouting of RenderSVGContainer for calculating  local
    // transformations and repaint.
    RenderSVGContainer::layout();
}