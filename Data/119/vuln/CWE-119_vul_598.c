void BiquadDSPKernel::getFrequencyResponse(int nFrequencies,
                                           const float* frequencyHz,
                                           float* magResponse,
                                           float* phaseResponse)
{
    bool isGood = nFrequencies > 0 && frequencyHz && magResponse && phaseResponse;
    ASSERT(isGood);
    if (!isGood)
        return;

    Vector<float> frequency(nFrequencies);

    double nyquist = this->nyquist();

    // Convert from frequency in Hz to normalized frequency (0 -> 1),
    // with 1 equal to the Nyquist frequency.
    for (int k = 0; k < nFrequencies; ++k)
        frequency[k] = narrowPrecisionToFloat(frequencyHz[k] / nyquist);

    // We want to get the final values of the coefficients and compute
    // the response from that instead of some intermediate smoothed
    // set. Forcefully update the coefficients even if they are not
    // dirty.

    updateCoefficientsIfNecessary(false, true);

    m_biquad.getFrequencyResponse(nFrequencies, frequency.data(), magResponse, phaseResponse);
}