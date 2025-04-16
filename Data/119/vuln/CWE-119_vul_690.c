PassRefPtr<SkImageFilter> FEColorMatrix::createImageFilter(SkiaImageFilterBuilder* builder)
{
    RefPtr<SkImageFilter> input(builder->build(inputEffect(0), operatingColorSpace()));
    SkAutoTUnref<SkColorFilter> filter(createColorFilter(m_type, m_values.data()));
    SkImageFilter::CropRect rect = getCropRect(builder->cropOffset());
    return adoptRef(SkColorFilterImageFilter::Create(filter, input.get(), &rect));
}