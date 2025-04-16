bool OMXNodeInstance::handleMessage(omx_message &msg) {
    const sp<GraphicBufferSource>& bufferSource(getGraphicBufferSource());

    if (msg.type == omx_message::FILL_BUFFER_DONE) {
        OMX_BUFFERHEADERTYPE *buffer =
            findBufferHeader(msg.u.extended_buffer_data.buffer);

        {
            Mutex::Autolock _l(mDebugLock);
            mOutputBuffersWithCodec.remove(buffer);

            CLOG_BUMPED_BUFFER(
                    FBD, WITH_STATS(FULL_BUFFER(
                            msg.u.extended_buffer_data.buffer, buffer, msg.fenceFd)));

            unbumpDebugLevel_l(kPortIndexOutput);
        }

        BufferMeta *buffer_meta =
            static_cast<BufferMeta *>(buffer->pAppPrivate);

        if (buffer->nOffset + buffer->nFilledLen < buffer->nOffset
                || buffer->nOffset + buffer->nFilledLen > buffer->nAllocLen) {
            CLOG_ERROR(onFillBufferDone, OMX_ErrorBadParameter,
                    FULL_BUFFER(NULL, buffer, msg.fenceFd));
        }
        buffer_meta->CopyFromOMX(buffer);

        if (bufferSource != NULL) {
            // fix up the buffer info (especially timestamp) if needed
            bufferSource->codecBufferFilled(buffer);

            msg.u.extended_buffer_data.timestamp = buffer->nTimeStamp;
        }
    } else if (msg.type == omx_message::EMPTY_BUFFER_DONE) {
        OMX_BUFFERHEADERTYPE *buffer =
            findBufferHeader(msg.u.buffer_data.buffer);

        {
            Mutex::Autolock _l(mDebugLock);
            mInputBuffersWithCodec.remove(buffer);

            CLOG_BUMPED_BUFFER(
                    EBD, WITH_STATS(EMPTY_BUFFER(msg.u.buffer_data.buffer, buffer, msg.fenceFd)));
        }

        if (bufferSource != NULL) {
            // This is one of the buffers used exclusively by
            // GraphicBufferSource.
            // Don't dispatch a message back to ACodec, since it doesn't
            // know that anyone asked to have the buffer emptied and will
            // be very confused.
            bufferSource->codecBufferEmptied(buffer, msg.fenceFd);
            return true;
        }
    }

    return false;
}