sal_Bool ODbaseTable::fetchRow(OValueRefRow& _rRow,const OSQLColumns & _rCols, sal_Bool _bUseTableDefs,sal_Bool bRetrieveData)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::fetchRow" );
	// Einlesen der Daten
	sal_Bool bIsCurRecordDeleted = ((char)m_pBuffer[0] == '*') ? sal_True : sal_False;

	// only read the bookmark

	// Satz als geloescht markieren
	//	rRow.setState(bIsCurRecordDeleted ? ROW_DELETED : ROW_CLEAN );
	_rRow->setDeleted(bIsCurRecordDeleted);
	*(_rRow->get())[0] = m_nFilePos;

	if (!bRetrieveData)
		return sal_True;

	sal_Size nByteOffset = 1;
	// Felder:
	OSQLColumns::Vector::const_iterator aIter = _rCols.get().begin();
    OSQLColumns::Vector::const_iterator aEnd  = _rCols.get().end();
    const sal_Size nCount = _rRow->get().size();
	for (sal_Size i = 1; aIter != aEnd && nByteOffset <= m_nBufferSize && i < nCount;++aIter, i++)
	{
		// Laengen je nach Datentyp:
		sal_Int32 nLen = 0;
		sal_Int32 nType = 0;
		if(_bUseTableDefs)
		{
			nLen	= m_aPrecisions[i-1];
			nType	= m_aTypes[i-1];
		}
		else
		{
			(*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRECISION))	>>= nLen;
			(*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))		>>= nType;
		}
		switch(nType)
		{
            case DataType::INTEGER:		
            case DataType::DOUBLE:
            case DataType::TIMESTAMP:
			case DataType::DATE:		
            case DataType::BIT:			
			case DataType::LONGVARCHAR:	
            case DataType::LONGVARBINARY:   
                nLen = m_aRealFieldLengths[i-1]; 
                break;
			case DataType::DECIMAL:
				if(_bUseTableDefs)
					nLen = SvDbaseConverter::ConvertPrecisionToDbase(nLen,m_aScales[i-1]);
				else
					nLen = SvDbaseConverter::ConvertPrecisionToDbase(nLen,getINT32((*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE))));
				break;	// das Vorzeichen und das Komma
			
            case DataType::BINARY:
			case DataType::OTHER:
				nByteOffset += nLen;
				continue;
		}

		// Ist die Variable ueberhaupt gebunden?
		if ( !(_rRow->get())[i]->isBound() )
		{
			// Nein - naechstes Feld.
			nByteOffset += nLen;
			OSL_ENSURE( nByteOffset <= m_nBufferSize ,"ByteOffset > m_nBufferSize!");
			continue;
		} // if ( !(_rRow->get())[i]->isBound() )
        if ( ( nByteOffset + nLen) > m_nBufferSize )
            break; // length doesn't match buffer size.

		char *pData = (char *) (m_pBuffer + nByteOffset);

		//	(*_rRow)[i].setType(nType);

		if (nType == DataType::CHAR || nType == DataType::VARCHAR)
		{
			char cLast = pData[nLen];
			pData[nLen] = 0;
			String aStr(pData,(xub_StrLen)nLen,m_eEncoding);
			aStr.EraseTrailingChars();

			if ( aStr.Len() )
                *(_rRow->get())[i] = ::rtl::OUString(aStr);
			else// keine StringLaenge, dann NULL
                (_rRow->get())[i]->setNull();
				
			pData[nLen] = cLast;
		} // if (nType == DataType::CHAR || nType == DataType::VARCHAR)
        else if ( DataType::TIMESTAMP == nType )
        {
            sal_Int32 nDate = 0,nTime = 0;
			memcpy(&nDate, pData, 4);
            memcpy(&nTime, pData+ 4, 4);
            if ( !nDate && !nTime )
            {
                (_rRow->get())[i]->setNull();
            }
            else
            {
                ::com::sun::star::util::DateTime aDateTime;
                lcl_CalDate(nDate,nTime,aDateTime);
                *(_rRow->get())[i] = aDateTime;
            }
        }
        else if ( DataType::INTEGER == nType )
        {
            sal_Int32 nValue = 0;
			memcpy(&nValue, pData, nLen);
            *(_rRow->get())[i] = nValue;
        }
        else if ( DataType::DOUBLE == nType )
        {
            double d = 0.0;
            if (getBOOL((*aIter)->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISCURRENCY)))) // Currency wird gesondert behandelt
            {
                sal_Int64 nValue = 0;
			    memcpy(&nValue, pData, nLen);
            
                if ( m_aScales[i-1] )
                    d = (double)(nValue / pow(10.0,(int)m_aScales[i-1]));
                else
                    d = (double)(nValue);
            }
            else
            {
                memcpy(&d, pData, nLen);
            }
            
            *(_rRow->get())[i] = d;
        }
		else
		{
			// Falls Nul-Zeichen im String enthalten sind, in Blanks umwandeln!
			for (sal_Int32 k = 0; k < nLen; k++)
			{
				if (pData[k] == '\0')
					pData[k] = ' ';
			}

			String aStr(pData, (xub_StrLen)nLen,m_eEncoding);		// Spaces am Anfang und am Ende entfernen:
			aStr.EraseLeadingChars();
			aStr.EraseTrailingChars();

			if (!aStr.Len())
			{
				nByteOffset += nLen;
				(_rRow->get())[i]->setNull();	// keine Werte -> fertig
				continue;
			}

			switch (nType)
			{
				case DataType::DATE:
				{
					if (aStr.Len() != nLen)
					{
						(_rRow->get())[i]->setNull();
						break;
					}
					const sal_uInt16  nYear   = (sal_uInt16)aStr.Copy( 0, 4 ).ToInt32();
					const sal_uInt16  nMonth  = (sal_uInt16)aStr.Copy( 4, 2 ).ToInt32();
					const sal_uInt16  nDay    = (sal_uInt16)aStr.Copy( 6, 2 ).ToInt32();

					const ::com::sun::star::util::Date aDate(nDay,nMonth,nYear);
					*(_rRow->get())[i] = aDate;
				}
				break;
				case DataType::DECIMAL:
					*(_rRow->get())[i] = ORowSetValue(aStr);
					//	pVal->setDouble(SdbTools::ToDouble(aStr));
				break;
				case DataType::BIT:
				{
					sal_Bool b;
					switch (* ((const char *)pData))
					{
						case 'T':
						case 'Y':
						case 'J':	b = sal_True; break;
						default: 	b = sal_False; break;
					}
					*(_rRow->get())[i] = b;
					//	pVal->setDouble(b);
				}
				break;
                case DataType::LONGVARBINARY:
                case DataType::BINARY:
				case DataType::LONGVARCHAR:
				{
					const long nBlockNo = aStr.ToInt32();	// Blocknummer lesen
					if (nBlockNo > 0 && m_pMemoStream) // Daten aus Memo-Datei lesen, nur wenn
					{
						if ( !ReadMemo(nBlockNo, (_rRow->get())[i]->get()) )
							break;
					}
					else
						(_rRow->get())[i]->setNull();
				}	break;
				default:
					OSL_ASSERT("Falscher Type");
			}
			(_rRow->get())[i]->setTypeKind(nType);
		}

		nByteOffset += nLen;
		OSL_ENSURE( nByteOffset <= m_nBufferSize ,"ByteOffset > m_nBufferSize!");
	}
	return sal_True;
}