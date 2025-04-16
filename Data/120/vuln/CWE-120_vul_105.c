sal_Bool ODbaseTable::UpdateBuffer(OValueRefVector& rRow, OValueRefRow pOrgRow,const Reference<XIndexAccess>& _xCols)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "dbase", "Ocke.Janssen@sun.com", "ODbaseTable::UpdateBuffer" );
	OSL_ENSURE(m_pBuffer,"Buffer is NULL!");
	if ( !m_pBuffer )
		return sal_False;
	sal_Int32 nByteOffset  = 1;

	// Felder aktualisieren:
	Reference<XPropertySet> xCol;
	Reference<XPropertySet> xIndex;
	sal_uInt16 i;
	::rtl::OUString aColName;
	const sal_Int32 nColumnCount = m_pColumns->getCount();
	::std::vector< Reference<XPropertySet> > aIndexedCols(nColumnCount);

	::comphelper::UStringMixEqual aCase(isCaseSensitive());

	Reference<XIndexAccess> xColumns = m_pColumns;
	// first search a key that exist already in the table
	for (i = 0; i < nColumnCount; ++i)
	{
		sal_Int32 nPos = i;
		if(_xCols != xColumns)
		{
			m_pColumns->getByIndex(i) >>= xCol;
			OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
			xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;

			for(nPos = 0;nPos<_xCols->getCount();++nPos)
			{
				Reference<XPropertySet> xFindCol;
				::cppu::extractInterface(xFindCol,_xCols->getByIndex(nPos));
				OSL_ENSURE(xFindCol.is(),"ODbaseTable::UpdateBuffer column is null!");
				if(aCase(getString(xFindCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))),aColName))
					break;
			}
			if (nPos >= _xCols->getCount())
				continue;
		}

		++nPos;
		xIndex = isUniqueByColumnName(i);
		aIndexedCols[i] = xIndex;
		if (xIndex.is())
		{
			// first check if the value is different to the old one and when if it conform to the index
			if(pOrgRow.isValid() && (rRow.get()[nPos]->getValue().isNull() || rRow.get()[nPos] == (pOrgRow->get())[nPos]))
				continue;
			else
			{
				//	ODbVariantRef xVar = (pVal == NULL) ? new ODbVariant() : pVal;
				Reference<XUnoTunnel> xTunnel(xIndex,UNO_QUERY);
				OSL_ENSURE(xTunnel.is(),"No TunnelImplementation!");
				ODbaseIndex* pIndex = reinterpret_cast< ODbaseIndex* >( xTunnel->getSomething(ODbaseIndex::getUnoTunnelImplementationId()) );
				OSL_ENSURE(pIndex,"ODbaseTable::UpdateBuffer: No Index returned!");

				if (pIndex->Find(0,*rRow.get()[nPos]))
				{
					// es existiert kein eindeutiger Wert
					if ( !aColName.getLength() )
					{
						m_pColumns->getByIndex(i) >>= xCol;
						OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
						xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;
						xCol.clear();
					} // if ( !aColName.getLength() )
                    const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                            STR_DUPLICATE_VALUE_IN_COLUMN
                            ,"$columnname$", aColName
                         ) );
                    ::dbtools::throwGenericSQLException( sError, *this );
				}
			}
		}
	}

	// when we are here there is no double key in the table

	for (i = 0; i < nColumnCount && nByteOffset <= m_nBufferSize ; ++i)
	{
		// Laengen je nach Datentyp:
		OSL_ENSURE(i < m_aPrecisions.size(),"Illegal index!");
		sal_Int32 nLen = 0;
		sal_Int32 nType = 0;
		sal_Int32 nScale = 0;
		if ( i < m_aPrecisions.size() )
		{
			nLen	= m_aPrecisions[i];
			nType	= m_aTypes[i];
			nScale	= m_aScales[i];
		}
		else
		{
			m_pColumns->getByIndex(i) >>= xCol;
			if ( xCol.is() )
			{
				xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_PRECISION))	>>= nLen;
				xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_TYPE))		>>= nType;
				xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_SCALE))		>>= nScale;
			}
		}

        bool bSetZero = false;
		switch (nType)
		{
            case DataType::INTEGER:
            case DataType::DOUBLE:
            case DataType::TIMESTAMP:
                bSetZero = true;
            case DataType::LONGVARBINARY:
			case DataType::DATE:
            case DataType::BIT:			
			case DataType::LONGVARCHAR:
                nLen = m_aRealFieldLengths[i]; 
                break;
			case DataType::DECIMAL:
				nLen = SvDbaseConverter::ConvertPrecisionToDbase(nLen,nScale);
				break;	// das Vorzeichen und das Komma
			default:					
                break;

		} // switch (nType)

		sal_Int32 nPos = i;
		if(_xCols != xColumns)
		{
			m_pColumns->getByIndex(i) >>= xCol;
			OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
			xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;
			for(nPos = 0;nPos<_xCols->getCount();++nPos)
			{
				Reference<XPropertySet> xFindCol;
				::cppu::extractInterface(xFindCol,_xCols->getByIndex(nPos));
				if(aCase(getString(xFindCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME))),aColName))
					break;
			}
			if (nPos >= _xCols->getCount())
			{
				nByteOffset += nLen;
				continue;
			}
		}



		++nPos; // the row values start at 1
		// Ist die Variable ueberhaupt gebunden?
		if ( !rRow.get()[nPos]->isBound() )
		{
			// Nein - naechstes Feld.
			nByteOffset += nLen;
			continue;
		}
		if (aIndexedCols[i].is())
		{
			Reference<XUnoTunnel> xTunnel(aIndexedCols[i],UNO_QUERY);
			OSL_ENSURE(xTunnel.is(),"No TunnelImplementation!");
			ODbaseIndex* pIndex = reinterpret_cast< ODbaseIndex* >( xTunnel->getSomething(ODbaseIndex::getUnoTunnelImplementationId()) );
			OSL_ENSURE(pIndex,"ODbaseTable::UpdateBuffer: No Index returned!");
			// Update !!
			if (pOrgRow.isValid() && !rRow.get()[nPos]->getValue().isNull() )//&& pVal->isModified())
				pIndex->Update(m_nFilePos,*(pOrgRow->get())[nPos],*rRow.get()[nPos]);
			else
				pIndex->Insert(m_nFilePos,*rRow.get()[nPos]);
		}

		char* pData = (char *)(m_pBuffer + nByteOffset);
		if (rRow.get()[nPos]->getValue().isNull())
		{
            if ( bSetZero )
                memset(pData,0,nLen);	// Zuruecksetzen auf NULL
            else
			    memset(pData,' ',nLen);	// Zuruecksetzen auf NULL
			nByteOffset += nLen;
			OSL_ENSURE( nByteOffset <= m_nBufferSize ,"ByteOffset > m_nBufferSize!");
			continue;
		}

        sal_Bool bHadError = sal_False;
		try
		{
			switch (nType)
			{
                case DataType::TIMESTAMP:
                    {
                        sal_Int32 nJulianDate = 0, nJulianTime = 0;
                        lcl_CalcJulDate(nJulianDate,nJulianTime,rRow.get()[nPos]->getValue());
                        // Genau 8 Byte kopieren:
					    memcpy(pData,&nJulianDate,4);
                        memcpy(pData+4,&nJulianTime,4);
                    }
                    break;
				case DataType::DATE:
				{
					::com::sun::star::util::Date aDate;
					if(rRow.get()[nPos]->getValue().getTypeKind() == DataType::DOUBLE)
						aDate = ::dbtools::DBTypeConversion::toDate(rRow.get()[nPos]->getValue().getDouble());
					else
						aDate = rRow.get()[nPos]->getValue();
					char s[9];
					snprintf(s,
						sizeof(s),
						"%04d%02d%02d",
						(int)aDate.Year,
						(int)aDate.Month,
						(int)aDate.Day);

					// Genau 8 Byte kopieren:
					strncpy(pData,s,sizeof s - 1);
				} break;
                case DataType::INTEGER:
                    {
                        sal_Int32 nValue = rRow.get()[nPos]->getValue();
                        memcpy(pData,&nValue,nLen);
                    }
                    break;
                case DataType::DOUBLE:
                    {
                        const double d = rRow.get()[nPos]->getValue();
                        m_pColumns->getByIndex(i) >>= xCol;
                        
                        if (getBOOL(xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISCURRENCY)))) // Currency wird gesondert behandelt
                        {
                            sal_Int64 nValue = 0;
                            if ( m_aScales[i] )
                                nValue = (sal_Int64)(d * pow(10.0,(int)m_aScales[i]));
                            else
                                nValue = (sal_Int64)(d);
                            memcpy(pData,&nValue,nLen);
                        } // if (getBOOL(xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_ISCURRENCY)))) // Currency wird gesondert behandelt
                        else
                            memcpy(pData,&d,nLen);
                    }
                    break;
				case DataType::DECIMAL:
				{
					memset(pData,' ',nLen);	// Zuruecksetzen auf NULL

					const double n = rRow.get()[nPos]->getValue();

					// ein const_cast, da GetFormatPrecision am SvNumberFormat nicht const ist, obwohl es das eigentlich
					// sein koennte und muesste

					const ByteString aDefaultValue( ::rtl::math::doubleToString( n, rtl_math_StringFormat_F, nScale, '.', NULL, 0));
                    sal_Bool bValidLength  = aDefaultValue.Len() <= nLen;
                    if ( bValidLength )
                    {
					    strncpy(pData,aDefaultValue.GetBuffer(),nLen);
					    // write the resulting double back
					    *rRow.get()[nPos] = toDouble(aDefaultValue);
                    }
                    else
					{
						m_pColumns->getByIndex(i) >>= xCol;
						OSL_ENSURE(xCol.is(),"ODbaseTable::UpdateBuffer column is null!");
						xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;
                        ::std::list< ::std::pair<const sal_Char* , ::rtl::OUString > > aStringToSubstitutes;
                        aStringToSubstitutes.push_back(::std::pair<const sal_Char* , ::rtl::OUString >("$columnname$", aColName));
                        aStringToSubstitutes.push_back(::std::pair<const sal_Char* , ::rtl::OUString >("$precision$", String::CreateFromInt32(nLen)));
                        aStringToSubstitutes.push_back(::std::pair<const sal_Char* , ::rtl::OUString >("$scale$", String::CreateFromInt32(nScale)));
                        aStringToSubstitutes.push_back(::std::pair<const sal_Char* , ::rtl::OUString >("$value$", ::rtl::OStringToOUString(aDefaultValue,RTL_TEXTENCODING_UTF8)));

                        const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                                STR_INVALID_COLUMN_DECIMAL_VALUE
                                ,aStringToSubstitutes
                             ) );
                        ::dbtools::throwGenericSQLException( sError, *this );
					}
				} break;
				case DataType::BIT:
					*pData = rRow.get()[nPos]->getValue().getBool() ? 'T' : 'F';
					break;
                case DataType::LONGVARBINARY:
				case DataType::LONGVARCHAR:
				{
					char cNext = pData[nLen]; // merken und temporaer durch 0 ersetzen
					pData[nLen] = '\0';		  // das geht, da der Puffer immer ein Zeichen groesser ist ...

					sal_uIntPtr nBlockNo = strtol((const char *)pData,NULL,10);	// Blocknummer lesen

					// Naechstes Anfangszeichen wieder restaurieren:
					pData[nLen] = cNext;
					if (!m_pMemoStream || !WriteMemo(rRow.get()[nPos]->get(), nBlockNo))
						break;

					ByteString aStr;
					ByteString aBlock(ByteString::CreateFromInt32(nBlockNo));
					aStr.Expand(static_cast<sal_uInt16>(nLen - aBlock.Len()), '0' );
					aStr += aBlock;
					// Zeichen kopieren:
					memset(pData,' ',nLen);	// Zuruecksetzen auf NULL
					memcpy(pData, aStr.GetBuffer(), nLen);
				}	break;
				default:
				{
					memset(pData,' ',nLen);	// Zuruecksetzen auf NULL

                    ::rtl::OUString sStringToWrite( rRow.get()[nPos]->getValue().getString() );

                    // convert the string, using the connection's encoding
                    ::rtl::OString sEncoded;
                   
                    DBTypeConversion::convertUnicodeStringToLength( sStringToWrite, sEncoded, nLen, m_eEncoding );
                    memcpy( pData, sEncoded.getStr(), sEncoded.getLength() );

				}
                break;
			}
		}
		catch( SQLException&  )
        {
            throw;
        }
		catch ( Exception& ) { bHadError = sal_True; }

		if ( bHadError )
		{
			m_pColumns->getByIndex(i) >>= xCol;
			OSL_ENSURE( xCol.is(), "ODbaseTable::UpdateBuffer column is null!" );
            if ( xCol.is() )
			    xCol->getPropertyValue(OMetaConnection::getPropMap().getNameByIndex(PROPERTY_ID_NAME)) >>= aColName;

			const ::rtl::OUString sError( getConnection()->getResources().getResourceStringWithSubstitution(
                    STR_INVALID_COLUMN_VALUE,
                    "$columnname$", aColName
                 ) );
            ::dbtools::throwGenericSQLException( sError, *this );
		}
		// Und weiter ...
		nByteOffset += nLen;
		OSL_ENSURE( nByteOffset <= m_nBufferSize ,"ByteOffset > m_nBufferSize!");
	}
	return sal_True;
}