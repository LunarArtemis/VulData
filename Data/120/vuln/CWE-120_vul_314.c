int CEditorMap::Load(class IStorage *pStorage, const char *pFileName, int StorageType)
{
	CDataFileReader DataFile;
	if(!DataFile.Open(pStorage, pFileName, StorageType))
		return 0;

	Clean();

	// check version
	CMapItemVersion *pItem = (CMapItemVersion *)DataFile.FindItem(MAPITEMTYPE_VERSION, 0);
	if(!pItem)
	{
		return 0;
	}
	else if(pItem->m_Version == CMapItemVersion::CURRENT_VERSION)
	{
		// load map info
		{
			CMapItemInfo *pItem = (CMapItemInfo *)DataFile.FindItem(MAPITEMTYPE_INFO, 0);
			if(pItem && pItem->m_Version == 1)
			{
				if(pItem->m_Author > -1)
					str_copy(m_MapInfo.m_aAuthor, (char *)DataFile.GetData(pItem->m_Author), sizeof(m_MapInfo.m_aAuthor));
				if(pItem->m_MapVersion > -1)
					str_copy(m_MapInfo.m_aVersion, (char *)DataFile.GetData(pItem->m_MapVersion), sizeof(m_MapInfo.m_aVersion));
				if(pItem->m_Credits > -1)
					str_copy(m_MapInfo.m_aCredits, (char *)DataFile.GetData(pItem->m_Credits), sizeof(m_MapInfo.m_aCredits));
				if(pItem->m_License > -1)
					str_copy(m_MapInfo.m_aLicense, (char *)DataFile.GetData(pItem->m_License), sizeof(m_MapInfo.m_aLicense));
			}
		}

		// load images
		{
			int Start, Num;
			DataFile.GetType( MAPITEMTYPE_IMAGE, &Start, &Num);
			for(int i = 0; i < Num; i++)
			{
				CMapItemImage *pItem = (CMapItemImage *)DataFile.GetItem(Start+i, 0, 0);
				char *pName = (char *)DataFile.GetData(pItem->m_ImageName);

				// copy base info
				CEditorImage *pImg = new CEditorImage(m_pEditor);
				pImg->m_External = pItem->m_External;

				if(pItem->m_External || (pItem->m_Version > 1 && pItem->m_Format != CImageInfo::FORMAT_RGB && pItem->m_Format != CImageInfo::FORMAT_RGBA))
				{
					char aBuf[IO_MAX_PATH_LENGTH];
					str_format(aBuf, sizeof(aBuf),"mapres/%s.png", pName);

					// load external
					CEditorImage ImgInfo(m_pEditor);
					if(m_pEditor->Graphics()->LoadPNG(&ImgInfo, aBuf, IStorage::TYPE_ALL))
					{
						*pImg = ImgInfo;
						pImg->m_Texture = m_pEditor->Graphics()->LoadTextureRaw(ImgInfo.m_Width, ImgInfo.m_Height, ImgInfo.m_Format, ImgInfo.m_pData, CImageInfo::FORMAT_AUTO, IGraphics::TEXLOAD_MULTI_DIMENSION);
						ImgInfo.m_pData = 0;
						pImg->m_External = 1;
					}
				}
				else
				{
					pImg->m_Width = pItem->m_Width;
					pImg->m_Height = pItem->m_Height;
					pImg->m_Format = pItem->m_Version == 1 ? CImageInfo::FORMAT_RGBA : pItem->m_Format;
					int PixelSize = pImg->m_Format == CImageInfo::FORMAT_RGB ? 3 : 4;

					// copy image data
					int DataSize = pImg->m_Width * pImg->m_Height * PixelSize;
					void *pData = DataFile.GetData(pItem->m_ImageData);
					pImg->m_pData = mem_alloc(DataSize);
					mem_copy(pImg->m_pData, pData, DataSize);
					pImg->m_Texture = m_pEditor->Graphics()->LoadTextureRaw(pImg->m_Width, pImg->m_Height, pImg->m_Format, pImg->m_pData, CImageInfo::FORMAT_AUTO, IGraphics::TEXLOAD_MULTI_DIMENSION);
				}

				// copy image name
				if(pName)
					str_copy(pImg->m_aName, pName, 128);

				// load auto mapper file
				pImg->LoadAutoMapper();

				m_lImages.add(pImg);

				// unload image
				DataFile.UnloadData(pItem->m_ImageData);
				DataFile.UnloadData(pItem->m_ImageName);
			}
		}

		// load groups
		{
			int LayersStart, LayersNum;
			DataFile.GetType(MAPITEMTYPE_LAYER, &LayersStart, &LayersNum);

			int Start, Num;
			DataFile.GetType(MAPITEMTYPE_GROUP, &Start, &Num);
			for(int g = 0; g < Num; g++)
			{
				CMapItemGroup *pGItem = (CMapItemGroup *)DataFile.GetItem(Start+g, 0, 0);

				if(pGItem->m_Version < 1 || pGItem->m_Version > CMapItemGroup::CURRENT_VERSION)
					continue;

				CLayerGroup *pGroup = NewGroup();
				pGroup->m_ParallaxX = pGItem->m_ParallaxX;
				pGroup->m_ParallaxY = pGItem->m_ParallaxY;
				pGroup->m_OffsetX = pGItem->m_OffsetX;
				pGroup->m_OffsetY = pGItem->m_OffsetY;

				if(pGItem->m_Version >= 2)
				{
					pGroup->m_UseClipping = pGItem->m_UseClipping;
					pGroup->m_ClipX = pGItem->m_ClipX;
					pGroup->m_ClipY = pGItem->m_ClipY;
					pGroup->m_ClipW = pGItem->m_ClipW;
					pGroup->m_ClipH = pGItem->m_ClipH;
				}

				// load group name
				if(pGItem->m_Version >= 3)
					IntsToStr(pGItem->m_aName, sizeof(pGroup->m_aName)/sizeof(int), pGroup->m_aName);

				for(int l = 0; l < pGItem->m_NumLayers; l++)
				{
					CLayer *pLayer = 0;
					CMapItemLayer *pLayerItem = (CMapItemLayer *)DataFile.GetItem(LayersStart+pGItem->m_StartLayer+l, 0, 0);
					if(!pLayerItem)
						continue;

					if(pLayerItem->m_Type == LAYERTYPE_TILES)
					{
						CMapItemLayerTilemap *pTilemapItem = (CMapItemLayerTilemap *)pLayerItem;
						CLayerTiles *pTiles = 0;

						if(pTilemapItem->m_Flags&TILESLAYERFLAG_GAME)
						{
							pTiles = new CLayerGame(pTilemapItem->m_Width, pTilemapItem->m_Height);
							MakeGameLayer(pTiles);
							MakeGameGroup(pGroup);
						}
						else
						{
							pTiles = new CLayerTiles(pTilemapItem->m_Width, pTilemapItem->m_Height);
							pTiles->m_pEditor = m_pEditor;
							pTiles->m_Color = pTilemapItem->m_Color;
							pTiles->m_ColorEnv = pTilemapItem->m_ColorEnv;
							pTiles->m_ColorEnvOffset = pTilemapItem->m_ColorEnvOffset;
						}

						pLayer = pTiles;

						pGroup->AddLayer(pTiles);
						void *pData = DataFile.GetData(pTilemapItem->m_Data);
						pTiles->m_Image = pTilemapItem->m_Image;
						pTiles->m_Game = pTilemapItem->m_Flags&TILESLAYERFLAG_GAME;

						// load layer name
						if(pTilemapItem->m_Version >= 3)
							IntsToStr(pTilemapItem->m_aName, sizeof(pTiles->m_aName)/sizeof(int), pTiles->m_aName);

						// get tile data
						if(pTilemapItem->m_Version > 3)
							pTiles->ExtractTiles((CTile *)pData);
						else
							mem_copy(pTiles->m_pTiles, pData, pTiles->m_Width*pTiles->m_Height*sizeof(CTile));


						if(pTiles->m_Game && pTilemapItem->m_Version == MakeVersion(1, *pTilemapItem))
						{
							for(int i = 0; i < pTiles->m_Width*pTiles->m_Height; i++)
							{
								if(pTiles->m_pTiles[i].m_Index)
									pTiles->m_pTiles[i].m_Index += ENTITY_OFFSET;
							}
						}

						DataFile.UnloadData(pTilemapItem->m_Data);
					}
					else if(pLayerItem->m_Type == LAYERTYPE_QUADS)
					{
						CMapItemLayerQuads *pQuadsItem = (CMapItemLayerQuads *)pLayerItem;
						CLayerQuads *pQuads = new CLayerQuads;
						pQuads->m_pEditor = m_pEditor;
						pLayer = pQuads;
						pQuads->m_Image = pQuadsItem->m_Image;
						if(pQuads->m_Image < -1 || pQuads->m_Image >= m_lImages.size())
							pQuads->m_Image = -1;

						// load layer name
						if(pQuadsItem->m_Version >= 2)
							IntsToStr(pQuadsItem->m_aName, sizeof(pQuads->m_aName)/sizeof(int), pQuads->m_aName);

						void *pData = DataFile.GetDataSwapped(pQuadsItem->m_Data);
						pGroup->AddLayer(pQuads);
						pQuads->m_lQuads.set_size(pQuadsItem->m_NumQuads);
						mem_copy(pQuads->m_lQuads.base_ptr(), pData, sizeof(CQuad)*pQuadsItem->m_NumQuads);
						DataFile.UnloadData(pQuadsItem->m_Data);
					}

					if(pLayer)
						pLayer->m_Flags = pLayerItem->m_Flags;
				}
			}
		}

		// load envelopes
		{
			CEnvPoint *pEnvPoints = 0;
			{
				int Start, Num;
				DataFile.GetType(MAPITEMTYPE_ENVPOINTS, &Start, &Num);
				if(Num)
					pEnvPoints = (CEnvPoint *)DataFile.GetItem(Start, 0, 0);
			}

			int Start, Num;
			DataFile.GetType(MAPITEMTYPE_ENVELOPE, &Start, &Num);
			for(int e = 0; e < Num; e++)
			{
				CMapItemEnvelope *pItem = (CMapItemEnvelope *)DataFile.GetItem(Start+e, 0, 0);
				CEnvelope *pEnv = new CEnvelope(pItem->m_Channels);
				pEnv->m_lPoints.set_size(pItem->m_NumPoints);
				for(int n = 0; n < pItem->m_NumPoints; n++)
				{
					if(pItem->m_Version >= 3)
					{
						pEnv->m_lPoints[n] = pEnvPoints[pItem->m_StartPoint + n];
					}
					else
					{
						// backwards compatibility
						CEnvPoint_v1 *pEnvPoint_v1 = &((CEnvPoint_v1 *)pEnvPoints)[pItem->m_StartPoint + n];
						mem_zero((void*)&pEnv->m_lPoints[n], sizeof(CEnvPoint));

						pEnv->m_lPoints[n].m_Time = pEnvPoint_v1->m_Time;
						pEnv->m_lPoints[n].m_Curvetype = pEnvPoint_v1->m_Curvetype;

						for(int c = 0; c < pItem->m_Channels; c++)
						{
							pEnv->m_lPoints[n].m_aValues[c] = pEnvPoint_v1->m_aValues[c];
						}
					}
				}

				if(pItem->m_aName[0] != -1)	// compatibility with old maps
					IntsToStr(pItem->m_aName, sizeof(pItem->m_aName)/sizeof(int), pEnv->m_aName);
				m_lEnvelopes.add(pEnv);
				if(pItem->m_Version >= 2)
					pEnv->m_Synchronized = pItem->m_Synchronized;
			}
		}
	}
	else
		return 0;

	return 1;
}