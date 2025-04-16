void SVGRenderSupport::layoutChildren(RenderObject* start, bool selfNeedsLayout)
{
    bool layoutSizeChanged = layoutSizeOfNearestViewportChanged(start);
    bool transformChanged = transformToRootChanged(start);
    HashSet<RenderObject*> notlayoutedObjects;

    for (RenderObject* child = start->firstChild(); child; child = child->nextSibling()) {
        bool needsLayout = selfNeedsLayout;
        bool childEverHadLayout = child->everHadLayout();

        if (transformChanged) {
            // If the transform changed we need to update the text metrics (note: this also happens for layoutSizeChanged=true).
            if (child->isSVGText())
                toRenderSVGText(child)->setNeedsTextMetricsUpdate();
            needsLayout = true;
        }

        if (layoutSizeChanged) {
            // When selfNeedsLayout is false and the layout size changed, we have to check whether this child uses relative lengths
            if (SVGElement* element = child->node()->isSVGElement() ? toSVGElement(child->node()) : 0) {
                if (element->hasRelativeLengths()) {
                    // When the layout size changed and when using relative values tell the RenderSVGShape to update its shape object
                    if (child->isSVGShape())
                        toRenderSVGShape(child)->setNeedsShapeUpdate();
                    else if (child->isSVGText()) {
                        toRenderSVGText(child)->setNeedsTextMetricsUpdate();
                        toRenderSVGText(child)->setNeedsPositioningValuesUpdate();
                    }

                    needsLayout = true;
                }
            }
        }

        SubtreeLayoutScope layoutScope(child);
        if (needsLayout)
            layoutScope.setNeedsLayout(child);

        if (child->needsLayout()) {
            child->layout();
            // Renderers are responsible for repainting themselves when changing, except
            // for the initial paint to avoid potential double-painting caused by non-sensical "old" bounds.
            // We could handle this in the individual objects, but for now it's easier to have
            // parent containers call repaint().  (RenderBlock::layout* has similar logic.)
            if (!childEverHadLayout)
                child->repaint();
        } else if (layoutSizeChanged)
            notlayoutedObjects.add(child);
    }

    if (!layoutSizeChanged) {
        ASSERT(notlayoutedObjects.isEmpty());
        return;
    }

    // If the layout size changed, invalidate all resources of all children that didn't go through the layout() code path.
    HashSet<RenderObject*>::iterator end = notlayoutedObjects.end();
    for (HashSet<RenderObject*>::iterator it = notlayoutedObjects.begin(); it != end; ++it)
        invalidateResourcesOfChildren(*it);
}