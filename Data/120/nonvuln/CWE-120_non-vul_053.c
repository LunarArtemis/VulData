void CiffComponent::doPrint(std::ostream&      os,
                                ByteOrder          byteOrder,
                                const std::string& prefix) const
    {
        os << prefix
           << _("tag") << " = 0x" << std::setw(4) << std::setfill('0')
           << std::hex << std::right << tagId()
           << ", " << _("dir") << " = 0x" << std::setw(4) << std::setfill('0')
           << std::hex << std::right << dir()
           << ", " << _("type") << " = " << TypeInfo::typeName(typeId())
           << ", " << _("size") << " = " << std::dec << size_
           << ", " << _("offset") << " = " << offset_ << "\n";

        Value::UniquePtr value;
        if (typeId() != directory) {
            value = Value::create(typeId());
            value->read(pData_, size_, byteOrder);
            if (value->size() < 100) {
                os << prefix << *value << "\n";
            }
        }
    }