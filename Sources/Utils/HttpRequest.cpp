#include "TumblrPCH.h"
#include "HttpRequest.h"


//!
//! Asynchronously request a url.
//!
//! @param networkManager
//!		The network manager to use for this request
//!
//! @param url
//!		The url to request.
//!
//! @param success
//!		A function that will be called when the request successfully completed.
//!
//! @param failure
//!		A function that will be called if the request failed.
//!
void RequestUrl(QNetworkAccessManager & networkManager, const QString & url, SuccessCallback success, FailureCallback failure)
{
	// prepare the request
	QNetworkRequest request;
	request.setUrl(QUrl(url));

	// send
	QNetworkReply * reply = networkManager.get(request);
	QObject::connect(reply, &QNetworkReply::finished, [=] (void) {
		QNetworkReply::NetworkError error = reply->error();
		if (error != QNetworkReply::NoError)
		{
			QString errorString = reply->errorString();
			reply->deleteLater();
			failure(error, errorString);
		}
		else
		{
			QByteArray result = reply->readAll();
			reply->deleteLater();
			success(result);
		}
	});

}

//!
//! Synchronously request a url.
//!
//! @param networkManager
//!		The network manager to use for this request
//!
//! @param url
//!		The url to request.
//!
//! @returns
//!		The reply or an empty array.
//!
QByteArray RequestUrl(QNetworkAccessManager & networkManager, const QString & url)
{
	// prepare the request
	QNetworkRequest request;
	request.setUrl(QUrl(url));

	// send
	QNetworkReply * reply = networkManager.get(request);

	// wait for the reply
	QEventLoop loop;
	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();

	// return the result
	QNetworkReply::NetworkError error = reply->error();
	if (error != QNetworkReply::NoError)
	{
		reply->deleteLater();
		return QByteArray();
	}
	else
	{
		QByteArray result = reply->readAll();
		reply->deleteLater();
		return result;
	}

}
