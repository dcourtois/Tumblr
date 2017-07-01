#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__


typedef std::function< void (QByteArray reply) > SuccessCallback;
typedef std::function< void (QNetworkReply::NetworkError error, QString errorString) > FailureCallback;

void		RequestUrl(QNetworkAccessManager & networkManager, const QString & url, SuccessCallback success, FailureCallback failure);
QByteArray	RequestUrl(QNetworkAccessManager & networkManager, const QString & url);


#endif // __HTTP_REQUEST_H__
