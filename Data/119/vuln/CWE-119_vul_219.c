ReverbConvolverStage::ReverbConvolverStage(const float* impulseResponse, size_t, size_t reverbTotalLatency, size_t stageOffset, size_t stageLength,
                                           size_t fftSize, size_t renderPhase, size_t renderSliceSize, ReverbAccumulationBuffer* accumulationBuffer, bool directMode)
    : m_accumulationBuffer(accumulationBuffer)
    , m_accumulationReadIndex(0)
    , m_inputReadIndex(0)
    , m_directMode(directMode)
{
    ASSERT(impulseResponse);
    ASSERT(accumulationBuffer);

    if (!m_directMode) {
        m_fftKernel = adoptPtr(new FFTFrame(fftSize));
        m_fftKernel->doPaddedFFT(impulseResponse + stageOffset, stageLength);
        m_fftConvolver = adoptPtr(new FFTConvolver(fftSize));
    } else {
        m_directKernel = adoptPtr(new AudioFloatArray(fftSize / 2));
        m_directKernel->copyToRange(impulseResponse + stageOffset, 0, fftSize / 2);
        m_directConvolver = adoptPtr(new DirectConvolver(renderSliceSize));
    }
    m_temporaryBuffer.allocate(renderSliceSize);

    // The convolution stage at offset stageOffset needs to have a corresponding delay to cancel out the offset.
    size_t totalDelay = stageOffset + reverbTotalLatency;

    // But, the FFT convolution itself incurs fftSize / 2 latency, so subtract this out...
    size_t halfSize = fftSize / 2;
    if (!m_directMode) {
        ASSERT(totalDelay >= halfSize);
        if (totalDelay >= halfSize)
            totalDelay -= halfSize;
    }

    // We divide up the total delay, into pre and post delay sections so that we can schedule at exactly the moment when the FFT will happen.
    // This is coordinated with the other stages, so they don't all do their FFTs at the same time...
    int maxPreDelayLength = std::min(halfSize, totalDelay);
    m_preDelayLength = totalDelay > 0 ? renderPhase % maxPreDelayLength : 0;
    if (m_preDelayLength > totalDelay)
        m_preDelayLength = 0;

    m_postDelayLength = totalDelay - m_preDelayLength;
    m_preReadWriteIndex = 0;
    m_framesProcessed = 0; // total frames processed so far

    size_t delayBufferSize = m_preDelayLength < fftSize ? fftSize : m_preDelayLength;
    delayBufferSize = delayBufferSize < renderSliceSize ? renderSliceSize : delayBufferSize;
    m_preDelayBuffer.allocate(delayBufferSize);
}