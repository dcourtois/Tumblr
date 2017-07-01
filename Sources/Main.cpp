#include "TumblrPCH.h"
#include "RegisterTypes.h"


#if defined(QT_DEBUG)

//!
//! The QML compilation errors
//!
QString errors;

//!
//! Temporary message handler used to catch QML compilation errors
//!
void MessageHandler(QtMsgType /* type */, const QMessageLogContext & /* context */, const QString & message)
{
	errors += message + "\n";
}

//!
//! Set the application engine with our main QML file
//!
void Setup(QQmlApplicationEngine *& engine, const QString & qmlPath)
{
	// re-create the engine
	DELETE engine;
	engine = NEW QQmlApplicationEngine();
	engine->addImportPath(qmlPath);

	// install a message handler to catch compilation errors
	errors.clear();
	//qInstallMessageHandler(MessageHandler);

	// set the source
	engine->load(qmlPath + "Main.qml");

	// restore the default message handler
	//qInstallMessageHandler(0);

	// check errors
	if (engine->rootObjects().empty() == true)
	{
		// display the errors
		engine->loadData(QString(
			"import QtQuick 2.3\n"
			"import QtQuick.Window 2.2\n"
			"import QtQuick.Controls 1.4\n"
			"Window {\n"
			"	visible: true\n"
			"	width: 1000\n"
			"	height: 500\n"
			"	ScrollView {\n"
			"		anchors.fill: parent\n"
			"		contentItem: Text {\n"
			"			font.family: \"Courier\"\n"
			"			text: \"" + errors.replace("\"", "\\\"") + "\"\n"
			"		}\n"
			"	}\n"
			"}\n"
		).toLatin1());
	}
}

#endif // QT_DEBUG

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

	// create and setup the Qml engine
#if defined(QT_DEBUG)
	// get the qml source path
	QString qmlPath = QDir::currentPath() + "/../../../../Sources/Qml/";

	// setup the engine
	QQmlApplicationEngine * engine = nullptr;
	Setup(engine, qmlPath);

	// hot reload
	QFileSystemWatcher watcher;
	watcher.addPath(qmlPath + "BlogSettings.qml");
	watcher.addPath(qmlPath + "Main.qml");
	watcher.addPath(qmlPath + "TumblrSettings.qml");
	watcher.addPath(qmlPath + "WindowSettings.qml");
	watcher.addPath(qmlPath + "WindowState.qml");
	QObject::connect(&watcher, &QFileSystemWatcher::fileChanged, [&] (const QString &) {
		Setup(engine, qmlPath);
	});
#else
	QQmlApplicationEngine * engine = NEW QQmlApplicationEngine();
	engine->load(QUrl("qrc:/Main.qml"));
#endif // QT_DEBUG

	// run
	int result = app.exec();

	// cleanup and return
	DELETE engine;
	return result;
}
