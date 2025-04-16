void printMP3Headers(FILE *f)
{
  unsigned long flags;
  int frameLen, numFrames = 0;
  int bitrate=0, bitrate_idx, samplerate=0, samplerate_idx;
  int version, layer, channels, padding;
  int bitrateSum = 0;
  int length;

  for(;;)
  {
    int flags_char;
    int i;
    /* get 4-byte header, bigendian */
    if((flags = fgetc(f)) == EOF)
      break;

    /* XXX - fix this mad hackery */
    if(flags == 'I' && fgetc(f) == 'D' && fgetc(f) == '3')
    {
      do
      {
	flags = fgetc(f);
      }
      while(flags != 0xFF && flags != EOF);
    }

    if(flags == EOF)
      break;

    flags <<= 24;
    for (i = 2; i >= 0; --i)
    {
      if ((flags_char = fgetc(f)) == EOF)
      {
        error("truncated file");
      }
      else
      {
        flags += flags_char << (i * 8);
      }
    }

    if((flags & MP3_FRAME_SYNC) != MP3_FRAME_SYNC)
      break;
    /*      error("bad sync on MP3 block!"); */

    ++numFrames;

    bitrate_idx = (flags & MP3_BITRATE) >> MP3_BITRATE_SHIFT;
    samplerate_idx = (flags & MP3_SAMPLERATE) >> MP3_SAMPLERATE_SHIFT;
    if (samplerate_idx < 0 || samplerate_idx > MP3_SAMPLERATE_IDX_MAX)
    {
      error("invalid samplerate index");
    }

    channels = ((flags & MP3_CHANNEL) == MP3_CHANNEL_MONO) ? 1 : 2;

    switch(flags & MP3_VERSION)
    {
      case MP3_VERSION_1:  version = 1; break;
      case MP3_VERSION_2:  version = 2; break;
      case MP3_VERSION_25: version = 25; break;
      default: error("unknown MP3 version!");
    }
    switch(flags & MP3_LAYER)
    {
      case MP3_LAYER_1: layer = 1; break;
      case MP3_LAYER_2: layer = 2; break;
      case MP3_LAYER_3: layer = 3; break;
      default: error("unknown MP3 layer!");
    }

    bitrateSum += bitrate;

    if(version == 1)
    {
      samplerate = mp1_samplerate_table[samplerate_idx];

      if(layer == 1)
	bitrate = mp1l1_bitrate_table[bitrate_idx];

      else if(layer == 2)
	bitrate = mp1l2_bitrate_table[bitrate_idx];

      else if(layer == 3)
	bitrate = mp1l3_bitrate_table[bitrate_idx];
    }
    else
    {
      if(version == 2)
	samplerate = mp2_samplerate_table[samplerate_idx];
      else
	samplerate = mp25_samplerate_table[samplerate_idx];

      if(layer == 1)
	bitrate = mp2l1_bitrate_table[bitrate_idx];
      else
	bitrate = mp2l23_bitrate_table[bitrate_idx];
    }

    padding = (flags & MP3_PADDING) ? 1 : 0;

    if(layer == 1)
      padding <<= 2;

    if(version == 1)
      frameLen = 144 * bitrate * 1000 / samplerate + padding;
    else
      frameLen = 72 * bitrate * 1000 / samplerate + padding;

    printf("frame %i: MP%i layer %i, %i Hz, %ikbps, %s, length=%i, protect %s\n",
	   numFrames, version, layer, samplerate, bitrate,
	   (channels==2) ? "stereo" : "mono", frameLen,
	   (flags&MP3_PROTECT) ? "on" : "off");

    skipBytes(f, frameLen-4);
  }

  if (numFrames == 0)
    error("no valid frame found");

  putchar('\n');

  length = numFrames*(samplerate > 3200 ? 1152 : 576)/samplerate;

  printf("Number of frames: %i\n", numFrames);
  printf("Average bitrate: %i\n", bitrateSum/numFrames);
  printf("Length: %i:%02i\n", length/60, length%60);
}