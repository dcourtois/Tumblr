#ifndef __IMAGE_H__
#define __IMAGE_H__


QString		ImageToString(const QImage & image);
QUrl		ImageToUrl(const QImage & image);
QImage		StringToImage(const QString & string);


#endif // __IMAGE_H__
