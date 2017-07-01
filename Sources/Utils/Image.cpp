#include "TumblrPCH.h"
#include "Image.h"


//!
//! Convert an image to a base64 encoded string
//!
QString ImageToString(const QImage & image)
{
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "png");
	return QString(bytes.toBase64());
}

//!
//! Convert an image to a url usable as a source of the Image QML class
//!
QUrl ImageToUrl(const QImage & image)
{
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "png");
	QString url = buffer.size() > 0 ? "data:image/png;base64," + QString(bytes.toBase64()) : "";
	return QUrl::fromEncoded(url.toUtf8());
}

//!
//! Convert a base64 encoded back to an image
//!
QImage StringToImage(const QString & string)
{
	QByteArray bytes(QByteArray::fromBase64(string.toUtf8()));
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::ReadOnly);
	QImage image;
	image.load(&buffer, "png");
	return image;
}
