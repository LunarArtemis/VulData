void RGBFramebufferModel::load(
  Imf::MultiPartInputFile& file, int partId, bool hasAlpha)
{
    QFuture<void> imageLoading = QtConcurrent::run([this,
                                                    &file,
                                                    partId,
                                                    hasAlpha]() {
        try {
            Imf::InputPart part(file, partId);

            Imath::Box2i datW = part.header().dataWindow();
            m_width           = datW.max.x - datW.min.x + 1;
            m_height          = datW.max.y - datW.min.y + 1;

            m_pixelAspectRatio = part.header().pixelAspectRatio();

            m_dataWindow = QRect(datW.min.x, datW.min.y, m_width, m_height);

            Imath::Box2i dispW = part.header().displayWindow();

            int dispW_width  = dispW.max.x - dispW.min.x + 1;
            int dispW_height = dispW.max.y - dispW.min.y + 1;

            m_displayWindow
              = QRect(dispW.min.x, dispW.min.y, dispW_width, dispW_height);

            // Check if there is specific chromaticities tied to the color
            // representation in this part.
            const Imf::ChromaticitiesAttribute* c
              = part.header().findTypedAttribute<Imf::ChromaticitiesAttribute>(
                "chromaticities");

            Imf::Chromaticities chromaticities;

            if (c != nullptr) {
                chromaticities = c->value();
            }

            m_pixelBuffer = new float[4 * m_width * m_height];

            // Check if there is alpha channel
            if (hasAlpha) {
                std::string      aLayer = m_parentLayer + "A";
                Imf::FrameBuffer framebuffer;

                Imf::Slice aSlice = Imf::Slice::Make(
                  Imf::PixelType::FLOAT,
                  &m_pixelBuffer[3],
                  datW,
                  4 * sizeof(float),
                  4 * m_width * sizeof(float));

                framebuffer.insert(aLayer, aSlice);

                part.setFrameBuffer(framebuffer);
                part.readPixels(datW.min.y, datW.max.y);

            } else {
                for (int y = 0; y < m_height; y++) {
                    for (int x = 0; x < m_width; x++) {
                        m_pixelBuffer[4 * (y * m_width + x) + 3] = 1.f;
                    }
                }
            }

            switch (m_layerType) {
                case Layer_RGB: {
                    std::string rLayer = m_parentLayer + "R";
                    std::string gLayer = m_parentLayer + "G";
                    std::string bLayer = m_parentLayer + "B";

                    Imf::FrameBuffer framebuffer;

                    Imf::Slice rSlice = Imf::Slice::Make(
                      Imf::PixelType::FLOAT,
                      &m_pixelBuffer[0],
                      datW,
                      4 * sizeof(float),
                      4 * m_width * sizeof(float));

                    Imf::Slice gSlice = Imf::Slice::Make(
                      Imf::PixelType::FLOAT,
                      &m_pixelBuffer[1],
                      datW,
                      4 * sizeof(float),
                      4 * m_width * sizeof(float));

                    Imf::Slice bSlice = Imf::Slice::Make(
                      Imf::PixelType::FLOAT,
                      &m_pixelBuffer[2],
                      datW,
                      4 * sizeof(float),
                      4 * m_width * sizeof(float));

                    framebuffer.insert(rLayer, rSlice);
                    framebuffer.insert(gLayer, gSlice);
                    framebuffer.insert(bLayer, bSlice);

                    part.setFrameBuffer(framebuffer);
                    part.readPixels(datW.min.y, datW.max.y);

                    // Handle custom chromaticities
                    Imath::M44f RGB_XYZ = Imf::RGBtoXYZ(chromaticities, 1.f);
                    Imath::M44f XYZ_RGB
                      = Imf::XYZtoRGB(Imf::Chromaticities(), 1.f);

                    Imath::M44f conversionMatrix = RGB_XYZ * XYZ_RGB;

                    #pragma omp parallel for
                    for (int y = 0; y < m_height; y++) {
                        for (int x = 0; x < m_width; x++) {
                            const float r
                              = m_pixelBuffer[4 * (y * m_width + x) + 0];
                            const float g
                              = m_pixelBuffer[4 * (y * m_width + x) + 1];
                            const float b
                              = m_pixelBuffer[4 * (y * m_width + x) + 2];

                            Imath::V3f rgb(r, g, b);
                            rgb *= conversionMatrix;

                            m_pixelBuffer[4 * (y * m_width + x) + 0] = rgb.x;
                            m_pixelBuffer[4 * (y * m_width + x) + 1] = rgb.y;
                            m_pixelBuffer[4 * (y * m_width + x) + 2] = rgb.z;
                        }
                    }
                } break;

                case Layer_YC: {
                    std::string yLayer  = m_parentLayer + "Y";
                    std::string ryLayer = m_parentLayer + "RY";
                    std::string byLayer = m_parentLayer + "BY";

                    Imf::FrameBuffer framebuffer;

                    Imf::Rgba* buff1 = new Imf::Rgba[m_width * m_height];
                    Imf::Rgba* buff2 = new Imf::Rgba[m_width * m_height];

                    float* yBuffer  = new float[m_width * m_height];
                    float* ryBuffer = new float[m_width / 2 * m_height / 2];
                    float* byBuffer = new float[m_width / 2 * m_height / 2];

                    Imf::Slice ySlice = Imf::Slice::Make(
                      Imf::PixelType::FLOAT,
                      &yBuffer[0],
                      datW,
                      sizeof(float),
                      m_width * sizeof(float));

                    Imf::Slice rySlice = Imf::Slice::Make(
                      Imf::PixelType::FLOAT,
                      &ryBuffer[0],
                      datW,
                      sizeof(float),
                      m_width / 2 * sizeof(float),
                      2,
                      2);

                    Imf::Slice bySlice = Imf::Slice::Make(
                      Imf::PixelType::FLOAT,
                      &byBuffer[0],
                      datW,
                      sizeof(float),
                      m_width / 2 * sizeof(float),
                      2,
                      2);

                    framebuffer.insert(yLayer, ySlice);
                    framebuffer.insert(ryLayer, rySlice);
                    framebuffer.insert(byLayer, bySlice);

                    part.setFrameBuffer(framebuffer);
                    part.readPixels(datW.min.y, datW.max.y);

                    // Filling missing values for chroma in the image
                    // TODO: now, naive reconstruction.
                    // Use later Imf::RgbaYca::reconstructChromaHoriz and
                    // Imf::RgbaYca::reconstructChromaVert to reconstruct missing
                    // pixels
                    #pragma omp parallel for
                    for (int y = 0; y < m_height; y++) {
                        for (int x = 0; x < m_width; x++) {
                            const float l = yBuffer[y * m_width + x];

                            /*
                            float ry = 0, by = 0;

                            if (y % 2 == 0) {
                                if (x % 2 == 0) {
                                    ry = ryBuffer[y / 2 * m_width / 2 + x / 2];
                                    by = byBuffer[y / 2 * m_width / 2 + x / 2];
                                } else {
                                    ry = .5 * (ryBuffer[y / 2 * m_width / 2 + x / 2] + ryBuffer[y / 2 * m_width / 2 + x / 2 + 1]);
                                    by = .5 * (byBuffer[y / 2 * m_width / 2 + x / 2] + byBuffer[y / 2 * m_width / 2 + x / 2 + 1]);
                                }
                            } else {
                                if (x % 2 == 0) {
                                    ry = .5 * (ryBuffer[y / 2 * m_width / 2 + x / 2] + ryBuffer[(y / 2 + 1) * m_width / 2 + x / 2]);
                                    by = .5 * (byBuffer[y / 2 * m_width / 2 + x / 2] + byBuffer[(y / 2 + 1) * m_width / 2 + x / 2]);
                                } else {
                                    ry = .25 * (ryBuffer[y / 2 * m_width / 2 + x / 2] + ryBuffer[(y / 2 + 1) * m_width / 2 + x / 2] + ryBuffer[y / 2 * m_width / 2 + x / 2 + 1] + ryBuffer[(y / 2 + 1) * m_width / 2 + x / 2 + 1]);
                                    by = .25 * (byBuffer[y / 2 * m_width / 2 + x / 2] + byBuffer[(y / 2 + 1) * m_width / 2 + x / 2] + byBuffer[y / 2 * m_width / 2 + x / 2 + 1] + byBuffer[(y / 2 + 1) * m_width / 2 + x / 2 + 1]);
                                }
                            }
                            */

                            const float ry
                              = ryBuffer[y / 2 * m_width / 2 + x / 2];
                            const float by
                              = byBuffer[y / 2 * m_width / 2 + x / 2];

                            buff1[y * m_width + x].r = ry;
                            buff1[y * m_width + x].g = l;
                            buff1[y * m_width + x].b = by;
                            // Do not forget the alpha values read earlier
                            buff1[y * m_width + x].a
                              = m_pixelBuffer[4 * (y * m_width + x) + 3];
                        }
                    }

                    Imath::V3f yw = Imf::RgbaYca::computeYw(chromaticities);

                    // Proceed to the YCA -> RGBA conversion
                    #pragma omp parallel for
                    for (int y = 0; y < m_height; y++) {
                        Imf::RgbaYca::YCAtoRGBA(
                          yw,
                          m_width,
                          &buff1[y * m_width],
                          &buff1[y * m_width]);
                    }

                    // Fix over saturated pixels
                    #pragma omp parallel for
                    for (int y = 0; y < m_height; y++) {
                        const Imf::Rgba* scanlines[3];

                        if (y == 0) {
                            scanlines[0] = &buff1[(y + 1) * m_width];
                        } else {
                            scanlines[0] = &buff1[(y - 1) * m_width];
                        }

                        scanlines[1] = &buff1[y * m_width];

                        if (y == m_height - 1) {
                            scanlines[2] = &buff1[(y - 1) * m_width];
                        } else {
                            scanlines[2] = &buff1[(y + 1) * m_width];
                        }

                        Imf::RgbaYca::fixSaturation(
                          yw,
                          m_width,
                          scanlines,
                          &buff2[y * m_width]);
                    }

                    // Handle custom chromaticities
                    Imath::M44f RGB_XYZ = Imf::RGBtoXYZ(chromaticities, 1.f);
                    Imath::M44f XYZ_RGB
                      = Imf::XYZtoRGB(Imf::Chromaticities(), 1.f);

                    Imath::M44f conversionMatrix = RGB_XYZ * XYZ_RGB;

                    #pragma omp parallel for
                    for (int y = 0; y < m_height; y++) {
                        for (int x = 0; x < m_width; x++) {
                            Imath::V3f rgb(
                              buff2[y * m_width + x].r,
                              buff2[y * m_width + x].g,
                              buff2[y * m_width + x].b);

                            rgb = rgb * conversionMatrix;

                            m_pixelBuffer[4 * (y * m_width + x) + 0] = rgb.x;
                            m_pixelBuffer[4 * (y * m_width + x) + 1] = rgb.y;
                            m_pixelBuffer[4 * (y * m_width + x) + 2] = rgb.z;
                        }
                    }

                    delete[] yBuffer;
                    delete[] ryBuffer;
                    delete[] byBuffer;
                    delete[] buff1;
                    delete[] buff2;
                }

                break;

                case Layer_Y: {
                    std::string yLayer = m_parentLayer;

                    Imf::FrameBuffer framebuffer;

                    Imf::Slice ySlice = Imf::Slice::Make(
                      Imf::PixelType::FLOAT,
                      &m_pixelBuffer[0],
                      datW,
                      4 * sizeof(float),
                      4 * m_width * sizeof(float));

                    framebuffer.insert(yLayer, ySlice);

                    part.setFrameBuffer(framebuffer);
                    part.readPixels(datW.min.y, datW.max.y);

                    #pragma omp parallel for
                    for (int i = 0; i < m_height * m_width; i++) {
                        m_pixelBuffer[4 * i + 1] = m_pixelBuffer[4 * i + 0];
                        m_pixelBuffer[4 * i + 2] = m_pixelBuffer[4 * i + 0];
                        m_pixelBuffer[4 * i + 3] = 1.f;
                    }
                } break;
            }

            m_image = QImage(m_width, m_height, QImage::Format_RGBA8888);
            m_isImageLoaded = true;

            emit imageLoaded();

            updateImage();
        } catch (std::exception& e) {
            emit loadFailed(e.what());
            return;
        }
    });

    m_imageLoadingWatcher->setFuture(imageLoading);
}