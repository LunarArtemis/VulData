void CiffComponent::add(UniquePtr component)
    {
        doAddComponent(std::move(component));
    }