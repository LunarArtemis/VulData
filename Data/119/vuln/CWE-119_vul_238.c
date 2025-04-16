SVGElement::SVGElement(const QualifiedName& tagName, Document& document, ConstructionType constructionType)
    : Element(tagName, &document, constructionType)
{
    ScriptWrappable::init(this);
    registerAnimatedPropertiesForSVGElement();
    setHasCustomStyleCallbacks();
}