#ifndef	__TUMBLR_PCH_H__
#define	__TUMBLR_PCH_H__


//------------------------------------------------------------------------------
// a few defines

#define ORGANIZATION_DOMAIN		"Citron.org"
#define ORGANIZATION_NAME		"Citron"
#define APPLICATION_NAME		"Tumblr"
#define APPLICATION_VERSION		"1.0"


//------------------------------------------------------------------------------
// disable a few warnings before including Qt

#if defined(WINDOWS)
#	pragma warning ( push )
#	pragma warning ( disable : 4127 )	// conditional expression is constant
#	pragma warning ( disable : 4244 )	// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#	pragma warning ( disable : 4251 )	// needs to have dll-interface to be used by clients of class
#	pragma warning ( disable : 4512 )	// assignment operator could not be generated
#	pragma warning ( disable : 4800 )	// forcing value to bool 'true' or 'false' (performance warning)
#endif


//------------------------------------------------------------------------------
// Qt

#include <QAbstractItemModel>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QJsonDocument>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QObject>
#include <QPainter>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStringList>
#include <QtDebug>
#include <QtQml>


//------------------------------------------------------------------------------
// restore warnings

#if defined(WINDOWS)
#	pragma warning ( pop )
#endif


//------------------------------------------------------------------------------
// STL

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <limits>


//------------------------------------------------------------------------------
// Memory tracker

#include "Utils/Memory.h"


#endif // __TUMBLR_PCH_H__
