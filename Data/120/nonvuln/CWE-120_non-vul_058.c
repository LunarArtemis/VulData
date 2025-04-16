void CrwMap::decode0x080a(const CiffComponent& ciffComponent,
                              const CrwMapping*    /*pCrwMapping*/,
                                    Image&         image,
                                    ByteOrder      byteOrder)
    {
        if (ciffComponent.typeId() != asciiString) return;

        // Make
        ExifKey key1("Exif.Image.Make");
        Value::UniquePtr value1 = Value::create(ciffComponent.typeId());
        uint32_t i = 0;
        for (;    i < ciffComponent.size()
               && ciffComponent.pData()[i] != '\0'; ++i) {
            // empty
        }
        value1->read(ciffComponent.pData(), ++i, byteOrder);
        image.exifData().add(key1, value1.get());

        // Model
        ExifKey key2("Exif.Image.Model");
        Value::UniquePtr value2 = Value::create(ciffComponent.typeId());
        uint32_t j = i;
        for (;    i < ciffComponent.size()
               && ciffComponent.pData()[i] != '\0'; ++i) {
            // empty
        }
        value2->read(ciffComponent.pData() + j, i - j + 1, byteOrder);
        image.exifData().add(key2, value2.get());
    }