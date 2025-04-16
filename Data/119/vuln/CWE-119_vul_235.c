RenderSVGResourceContainer::RenderSVGResourceContainer(SVGElement* node)
    : RenderSVGHiddenContainer(node)
    , m_id(node->getIdAttribute())
    , m_registered(false)
    , m_isInvalidating(false)
{
}