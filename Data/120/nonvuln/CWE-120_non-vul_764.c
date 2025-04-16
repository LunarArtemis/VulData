OIIO_PLUGIN_EXPORTS_BEGIN

OIIO_EXPORT ImageInput*
ico_input_imageio_create()
{
    return new ICOInput;
}