#ifndef UTILS_HTTP_REQUEST_H
#define UTILS_HTTP_REQUEST_H


//!
//! Defines the signature of a function like object that will be called when
//! a request has been successfully retrieved.
//!
typedef std::function< void (QByteArray reply) > SuccessCallback;

//!
//! Defines the signature of a function like object that will be called when
//! a request couldn't be completed.
//!
typedef std::function< void (QNetworkReply::NetworkError error, QString errorString) > FailureCallback;

//!
//! Defines the signature of a function like object that returns true if the
//! current request should be interrupted, false otherwise.
//!
typedef std::function< bool (void) > InterruptionCallback;

void		RequestUrl(QNetworkAccessManager & networkManager, const QString & url, SuccessCallback success, FailureCallback failure);
QByteArray	RequestUrl(QNetworkAccessManager & networkManager, const QString & url, InterruptionCallback interrupt);


#endif // UTILS_HTTP_REQUEST_H
