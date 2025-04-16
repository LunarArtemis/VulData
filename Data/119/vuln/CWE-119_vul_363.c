void WebClipboardImpl::writeImage(const WebImage& image,
                                  const WebURL& url,
                                  const WebString& title) {
  ScopedClipboardWriterGlue scw(client_);

  if (!image.isNull()) {
    const SkBitmap& bitmap = image.getSkBitmap();
    SkAutoLockPixels locked(bitmap);
    scw.WriteBitmapFromPixels(bitmap.getPixels(), image.size());
  }

  if (!url.isEmpty()) {
    scw.WriteBookmark(title, url.spec());
#if !defined(OS_MACOSX)
    // When writing the image, we also write the image markup so that pasting
    // into rich text editors, such as Gmail, reveals the image. We also don't
    // want to call writeText(), since some applications (WordPad) don't pick
    // the image if there is also a text format on the clipboard.
    // We also don't want to write HTML on a Mac, since Mail.app prefers to use
    // the image markup over attaching the actual image. See
    // http://crbug.com/33016 for details.
    scw.WriteHTML(base::UTF8ToUTF16(URLToImageMarkup(url, title)),
                  std::string());
#endif
  }
}