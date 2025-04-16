bool
ICOInput::close()
{
    if (m_png && m_info)
        PNG_pvt::destroy_read_struct(m_png, m_info);
    if (m_file) {
        fclose(m_file);
        m_file = NULL;
    }

    init();  // Reset to initial state
    return true;
}