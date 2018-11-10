#include "TumblrPCH.h"
#include "RegisterTypes.h"
#include "Utils/Logger.h"


//!
//! Entry point of the application
//!
int main(int argc, char *argv[])
{
	// create the application
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);
	app.setOrganizationDomain(ORGANIZATION_DOMAIN);
	app.setOrganizationName(ORGANIZATION_NAME);
	app.setApplicationName(APPLICATION_NAME);
	app.setApplicationVersion(APPLICATION_VERSION);

	// register our types
	Tumblr::RegisterTypes();

	// set style
	QQuickStyle::setStyle("Material");

	// create the QML engine
	QQmlApplicationEngine * engine = NEW QQmlApplicationEngine();

	// set global objects used to communicate between QML and C++
	Logger logger;
	engine->rootContext()->setContextProperty("logger", &logger);

	// load the QML app
	engine->load(QUrl("qrc:/Main.qml"));

	// run
	int result = app.exec();

	// cleanup and return
	DELETE engine;
	return result;
}
