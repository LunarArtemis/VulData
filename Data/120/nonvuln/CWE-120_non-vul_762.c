OIIO_PLUGIN_EXPORTS_END



bool
ICOInput::open(const std::string& name, ImageSpec& newspec)
{
    m_filename = name;

    m_file = Filesystem::fopen(name, "rb");
    if (!m_file) {
        errorf("Could not open file \"%s\"", name);
        return false;
    }

    if (!fread(&m_ico, 1, sizeof(m_ico)))
        return false;

    if (bigendian()) {
        // ICOs are little endian
        //swap_endian (&m_ico.reserved); // no use flipping, it's 0 anyway
        swap_endian(&m_ico.type);
        swap_endian(&m_ico.count);
    }
    if (m_ico.reserved != 0 || m_ico.type != 1) {
        errorf("File failed ICO header check");
        return false;
    }

    // default to subimage #0, according to convention
    bool ok = seek_subimage(0, 0);
    if (ok)
        newspec = spec();
    else
        close();
    return ok;
}