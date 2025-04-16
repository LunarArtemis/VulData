ModuleExport size_t analyzeImage(Image **images,const int argc,
  const char **argv,ExceptionInfo *exception)
{
  char
    text[MagickPathExtent];

  double
    area,
    brightness,
    brightness_mean,
    brightness_standard_deviation,
    brightness_kurtosis,
    brightness_skewness,
    brightness_sum_x,
    brightness_sum_x2,
    brightness_sum_x3,
    brightness_sum_x4,
    hue,
    saturation,
    saturation_mean,
    saturation_standard_deviation,
    saturation_kurtosis,
    saturation_skewness,
    saturation_sum_x,
    saturation_sum_x2,
    saturation_sum_x3,
    saturation_sum_x4;

  Image
    *image;

  assert(images != (Image **) NULL);
  assert(*images != (Image *) NULL);
  assert((*images)->signature == MagickCoreSignature);
  (void) argc;
  (void) argv;
  image=(*images);
  for ( ; image != (Image *) NULL; image=GetNextImageInList(image))
  {
    CacheView
      *image_view;

    ssize_t
      y;

    MagickBooleanType
      status;

    brightness_sum_x=0.0;
    brightness_sum_x2=0.0;
    brightness_sum_x3=0.0;
    brightness_sum_x4=0.0;
    brightness_mean=0.0;
    brightness_standard_deviation=0.0;
    brightness_kurtosis=0.0;
    brightness_skewness=0.0;
    saturation_sum_x=0.0;
    saturation_sum_x2=0.0;
    saturation_sum_x3=0.0;
    saturation_sum_x4=0.0;
    saturation_mean=0.0;
    saturation_standard_deviation=0.0;
    saturation_kurtosis=0.0;
    saturation_skewness=0.0;
    area=0.0;
    status=MagickTrue;
    image_view=AcquireVirtualCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
    #pragma omp parallel for schedule(static,4) shared(status) \
      magick_threads(image,image,image->rows,1)
#endif
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      register const Quantum
        *p;

      register ssize_t
        x;

      if (status == MagickFalse)
        continue;
      p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
      if (p == (const Quantum *) NULL)
        {
          status=MagickFalse;
          continue;
        }
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        ConvertRGBToHSL(GetPixelRed(image,p),GetPixelGreen(image,p),
          GetPixelBlue(image,p),&hue,&saturation,&brightness);
        brightness*=QuantumRange;
        brightness_sum_x+=brightness;
        brightness_sum_x2+=brightness*brightness;
        brightness_sum_x3+=brightness*brightness*brightness;
        brightness_sum_x4+=brightness*brightness*brightness*brightness;
        saturation*=QuantumRange;
        saturation_sum_x+=saturation;
        saturation_sum_x2+=saturation*saturation;
        saturation_sum_x3+=saturation*saturation*saturation;
        saturation_sum_x4+=saturation*saturation*saturation*saturation;
        area++;
        p+=GetPixelChannels(image);
      }
    }
    image_view=DestroyCacheView(image_view);
    if (area <= 0.0)
      break;
    brightness_mean=brightness_sum_x/area;
    (void) FormatLocaleString(text,MagickPathExtent,"%g",brightness_mean);
    (void) SetImageProperty(image,"filter:brightness:mean",text,
      exception);
    brightness_standard_deviation=sqrt(brightness_sum_x2/area-(brightness_sum_x/
      area*brightness_sum_x/area));
    (void) FormatLocaleString(text,MagickPathExtent,"%g",
      brightness_standard_deviation);
    (void) SetImageProperty(image,"filter:brightness:standard-deviation",text,
      exception);
    if (fabs(brightness_standard_deviation) >= MagickEpsilon)
      brightness_kurtosis=(brightness_sum_x4/area-4.0*brightness_mean*
        brightness_sum_x3/area+6.0*brightness_mean*brightness_mean*
        brightness_sum_x2/area-3.0*brightness_mean*brightness_mean*
        brightness_mean*brightness_mean)/(brightness_standard_deviation*
        brightness_standard_deviation*brightness_standard_deviation*
        brightness_standard_deviation)-3.0;
    (void) FormatLocaleString(text,MagickPathExtent,"%g",brightness_kurtosis);
    (void) SetImageProperty(image,"filter:brightness:kurtosis",text,
      exception);
    if (brightness_standard_deviation != 0)
      brightness_skewness=(brightness_sum_x3/area-3.0*brightness_mean*
        brightness_sum_x2/area+2.0*brightness_mean*brightness_mean*
        brightness_mean)/(brightness_standard_deviation*
        brightness_standard_deviation*brightness_standard_deviation);
    (void) FormatLocaleString(text,MagickPathExtent,"%g",brightness_skewness);
    (void) SetImageProperty(image,"filter:brightness:skewness",text,
      exception);
    saturation_mean=saturation_sum_x/area;
    (void) FormatLocaleString(text,MagickPathExtent,"%g",saturation_mean);
    (void) SetImageProperty(image,"filter:saturation:mean",text,
      exception);
    saturation_standard_deviation=sqrt(saturation_sum_x2/area-(saturation_sum_x/
      area*saturation_sum_x/area));
    (void) FormatLocaleString(text,MagickPathExtent,"%g",
      saturation_standard_deviation);
    (void) SetImageProperty(image,"filter:saturation:standard-deviation",text,
      exception);
    if (fabs(saturation_standard_deviation) >= MagickEpsilon)
      saturation_kurtosis=(saturation_sum_x4/area-4.0*saturation_mean*
        saturation_sum_x3/area+6.0*saturation_mean*saturation_mean*
        saturation_sum_x2/area-3.0*saturation_mean*saturation_mean*
        saturation_mean*saturation_mean)/(saturation_standard_deviation*
        saturation_standard_deviation*saturation_standard_deviation*
        saturation_standard_deviation)-3.0;
    (void) FormatLocaleString(text,MagickPathExtent,"%g",saturation_kurtosis);
    (void) SetImageProperty(image,"filter:saturation:kurtosis",text,
      exception);
    if (fabs(saturation_standard_deviation) >= MagickEpsilon)
      saturation_skewness=(saturation_sum_x3/area-3.0*saturation_mean*
        saturation_sum_x2/area+2.0*saturation_mean*saturation_mean*
        saturation_mean)/(saturation_standard_deviation*
        saturation_standard_deviation*saturation_standard_deviation);
    (void) FormatLocaleString(text,MagickPathExtent,"%g",saturation_skewness);
    (void) SetImageProperty(image,"filter:saturation:skewness",text,
      exception);
  }
  return(MagickImageFilterSignature);
}