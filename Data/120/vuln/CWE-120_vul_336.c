void YFramebufferModel::load(Imf::MultiPartInputFile& file, int partId)
{
    QFuture<void> imageLoading = QtConcurrent::run([this, &file, partId]() {
        try {
            Imf::InputPart part(file, partId);

            Imath::Box2i datW = part.header().dataWindow();
            m_width           = datW.max.x - datW.min.x + 1;
            m_height          = datW.max.y - datW.min.y + 1;

            m_pixelAspectRatio = part.header().pixelAspectRatio();

            Imf::Slice graySlice;
            // TODO: Check it that can be guess from the header
            // also, check if this can be nested
            if (m_layer == "BY" || m_layer == "RY") {
                m_width /= 2;
                m_height /= 2;

                m_dataWindow = QRect(datW.min.x, datW.min.y, m_width, m_height);

                Imath::Box2i dispW = part.header().displayWindow();

                int dispW_width  = dispW.max.x - dispW.min.x + 1;
                int dispW_height = dispW.max.y - dispW.min.y + 1;

                m_displayWindow = QRect(
                  dispW.min.x,
                  dispW.min.y,
                  dispW_width / 2,
                  dispW_height / 2);

                m_pixelBuffer = new float[m_width * m_height];

                // Luminance Chroma channels
                graySlice = Imf::Slice::Make(
                  Imf::PixelType::FLOAT,
                  m_pixelBuffer,
                  datW,
                  sizeof(float),
                  m_width * sizeof(float),
                  2,
                  2);
            } else {
                m_dataWindow = QRect(datW.min.x, datW.min.y, m_width, m_height);

                Imath::Box2i dispW = part.header().displayWindow();

                int dispW_width  = dispW.max.x - dispW.min.x + 1;
                int dispW_height = dispW.max.y - dispW.min.y + 1;

                m_displayWindow
                  = QRect(dispW.min.x, dispW.min.y, dispW_width, dispW_height);

                m_pixelBuffer = new float[m_width * m_height];

                graySlice = Imf::Slice::Make(
                  Imf::PixelType::FLOAT,
                  m_pixelBuffer,
                  datW);
            }

            Imf::FrameBuffer framebuffer;

            framebuffer.insert(m_layer, graySlice);

            part.setFrameBuffer(framebuffer);
            part.readPixels(datW.min.y, datW.max.y);

            // Determine min and max of the dataset
            m_datasetMin = std::numeric_limits<double>::infinity();
            m_datasetMax = -std::numeric_limits<double>::infinity();

            for (int i = 0; i < m_width * m_height; i++) {
                m_datasetMin = std::min(m_datasetMin, (double)m_pixelBuffer[i]);
                m_datasetMax = std::max(m_datasetMax, (double)m_pixelBuffer[i]);
            }

            m_image         = QImage(m_width, m_height, QImage::Format_RGB888);
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