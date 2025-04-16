CiffComponent* CiffDirectory::doFindComponent(uint16_t crwTagId,
                                                  uint16_t crwDir) const
    {
        CiffComponent* cc = nullptr;
        const Components::const_iterator b = components_.begin();
        const Components::const_iterator e = components_.end();
        for (Components::const_iterator i = b; i != e; ++i) {
            cc = (*i)->findComponent(crwTagId, crwDir);
            if (cc) return cc;
        }
        return 0;
    }