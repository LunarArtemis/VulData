void SVGElement::updateRelativeLengthsInformation(bool clientHasRelativeLengths, SVGElement* clientElement)
{
    // If we're not yet in a document, this function will be called again from insertedInto(). Do nothing now.
    if (!inDocument())
        return;

    // An element wants to notify us that its own relative lengths state changed.
    // Register it in the relative length map, and register us in the parent relative length map.
    // Register the parent in the grandparents map, etc. Repeat procedure until the root of the SVG tree.
    for (ContainerNode* currentNode = this; currentNode && currentNode->isSVGElement(); currentNode = currentNode->parentNode()) {
        SVGElement* currentElement = toSVGElement(currentNode);

        bool hadRelativeLengths = currentElement->hasRelativeLengths();
        if (clientHasRelativeLengths)
            currentElement->m_elementsWithRelativeLengths.add(clientElement);
        else
            currentElement->m_elementsWithRelativeLengths.remove(clientElement);

        // If the relative length state hasn't changed, we can stop propagating the notfication.
        if (hadRelativeLengths == currentElement->hasRelativeLengths())
            break;

        clientElement = currentElement;
        clientHasRelativeLengths = clientElement->hasRelativeLengths();
    }
}