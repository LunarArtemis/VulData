void CrwMap::decodeArray(const CiffComponent& ciffComponent,
                             const CrwMapping*    pCrwMapping,
                                   Image&         image,
                                   ByteOrder      byteOrder)
    {
        if (ciffComponent.typeId() != unsignedShort) {
            return decodeBasic(ciffComponent, pCrwMapping, image, byteOrder);
        }

        long aperture = 0;
        long shutterSpeed = 0;

        IfdId ifdId = ifdIdNotSet;
        switch (pCrwMapping->tag_) {
        case 0x0001: ifdId = canonCsId; break;
        case 0x0004: ifdId = canonSiId; break;
        case 0x000f: ifdId = canonCfId; break;
        case 0x0012: ifdId = canonPiId; break;
        }
        assert(ifdId != ifdIdNotSet);

        std::string gName(Internal::groupName(ifdId));
        uint16_t c = 1;
        while (uint32_t(c)*2 < ciffComponent.size()) {
            uint16_t n = 1;
            ExifKey key(c, gName);
            UShortValue value;
            if (ifdId == canonCsId && c == 23 && ciffComponent.size() > 50) n = 3;
            value.read(ciffComponent.pData() + c*2, n*2, byteOrder);
            image.exifData().add(key, &value);
            if (ifdId == canonSiId && c == 21) aperture = value.toLong();
            if (ifdId == canonSiId && c == 22) shutterSpeed = value.toLong();
            c += n;
        }

        if (ifdId == canonSiId) {
            // Exif.Photo.FNumber
            float f = fnumber(canonEv(aperture));
            Rational r = floatToRationalCast(f);
            URational ur(r.first, r.second);
            URationalValue fn;
            fn.value_.push_back(ur);
            image.exifData().add(ExifKey("Exif.Photo.FNumber"), &fn);

            // Exif.Photo.ExposureTime
            ur = exposureTime(canonEv(shutterSpeed));
            URationalValue et;
            et.value_.push_back(ur);
            image.exifData().add(ExifKey("Exif.Photo.ExposureTime"), &et);
        }
    }