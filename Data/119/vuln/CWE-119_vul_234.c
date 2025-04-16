void RenderSVGResourceContainer::layout()
{
    // Invalidate all resources if our layout changed.
    if (everHadLayout() && selfNeedsLayout())
        RenderSVGRoot::addResourceForClientInvalidation(this);

    RenderSVGHiddenContainer::layout();
}