sal_uInt32 LwpObjectID::ReadIndexed(LwpObjectStream *pStrm)
{
    m_bIsCompressed = sal_False;
    if(LwpFileHeader::m_nFileRevision < 0x000B)
    {
        return Read(pStrm);
    }

    pStrm->QuickRead(&m_nIndex, sizeof(m_nIndex));
    if (m_nIndex)
    {
        m_bIsCompressed = sal_True;
        //m_nLow = index;		//note the m_nLow stores the index instead of the actual time id
        LwpGlobalMgr* pGlobal = LwpGlobalMgr::GetInstance();
        LwpObjectFactory* pFactory = pGlobal->GetLwpObjFactory();
        LwpIndexManager* pIdxMgr = pFactory->GetIndexManager();
        m_nLow =  pIdxMgr->GetObjTime( (sal_uInt16)m_nIndex);
    }
    else
    {
         pStrm->QuickRead(&m_nLow, sizeof(m_nLow));
    }
    pStrm->QuickRead(&m_nHigh, sizeof(m_nHigh));
    return DiskSizeIndexed();
}