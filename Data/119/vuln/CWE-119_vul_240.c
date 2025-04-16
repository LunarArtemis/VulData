void SVGSVGElement::svgAttributeChanged(const QualifiedName& attrName)
{
    bool updateRelativeLengthsOrViewBox = false;
    bool widthChanged = attrName == SVGNames::widthAttr;
    if (widthChanged
        || attrName == SVGNames::heightAttr
        || attrName == SVGNames::xAttr
        || attrName == SVGNames::yAttr) {
        updateRelativeLengthsOrViewBox = true;
        updateRelativeLengthsInformation();

        // At the SVG/HTML boundary (aka RenderSVGRoot), the width attribute can
        // affect the replaced size so we need to mark it for updating.
        if (widthChanged) {
            RenderObject* renderObject = renderer();
            if (renderObject && renderObject->isSVGRoot())
                toRenderSVGRoot(renderObject)->setNeedsLayoutAndPrefWidthsRecalc();
        }
    }

    if (SVGFitToViewBox::isKnownAttribute(attrName)) {
        updateRelativeLengthsOrViewBox = true;
        if (RenderObject* object = renderer())
            object->setNeedsTransformUpdate();
    }

    SVGElementInstance::InvalidationGuard invalidationGuard(this);

    if (updateRelativeLengthsOrViewBox
        || SVGLangSpace::isKnownAttribute(attrName)
        || SVGExternalResourcesRequired::isKnownAttribute(attrName)
        || SVGZoomAndPan::isKnownAttribute(attrName)) {
        if (renderer())
            RenderSVGResource::markForLayoutAndParentResourceInvalidation(renderer());
        return;
    }

    SVGGraphicsElement::svgAttributeChanged(attrName);
}