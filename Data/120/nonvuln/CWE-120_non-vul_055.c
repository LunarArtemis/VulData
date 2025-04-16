CiffComponent* CiffDirectory::doAdd(CrwDirs& crwDirs, uint16_t crwTagId)
    {
        /*
          add()
            if stack not empty
              pop from stack
              find dir among components
              if not found, create it
              add()
            else
              find tag among components
              if not found, create it
              set value
        */
        const Components::iterator b = components_.begin();
        const Components::iterator e = components_.end();

        if (!crwDirs.empty()) {
            CrwSubDir csd = crwDirs.top();
            crwDirs.pop();
            // Find the directory
            for (Components::iterator i = b; i != e; ++i) {
                if ((*i)->tag() == csd.crwDir_) {
                    cc_ = *i;
                    break;
                }
            }
            if (cc_ == 0) {
                // Directory doesn't exist yet, add it
                m_ = UniquePtr(new CiffDirectory(csd.crwDir_, csd.parent_));
                cc_ = m_.get();
                add(std::move(m_));
            }
            // Recursive call to next lower level directory
            cc_ = cc_->add(crwDirs, crwTagId);
        }
        else {
            // Find the tag
            for (Components::iterator i = b; i != e; ++i) {
                if ((*i)->tagId() == crwTagId) {
                    cc_ = *i;
                    break;
                }
            }
            if (cc_ == 0) {
                // Tag doesn't exist yet, add it
                m_ = UniquePtr(new CiffEntry(crwTagId, tag()));
                cc_ = m_.get();
                add(std::move(m_));
            }
        }
        return cc_;
    }