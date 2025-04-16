void CiffDirectory::doAddComponent(UniquePtr component)
    {
        components_.push_back(component.release());
    }