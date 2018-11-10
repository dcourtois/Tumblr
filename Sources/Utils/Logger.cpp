#include "TumblrPCH.h"
#include "Logger.h"


//! Mutex used to avoid races when logging
static QMutex mutex;

//!
//! The message handler
//!
static void MessageHandler(QtMsgType type, const QMessageLogContext & /* context */, const QString & message)
{
	QMutexLocker lock(&mutex);
	QString path = QCoreApplication::applicationDirPath() + "/Logs.txt";
	QFile file(path);
	file.open(QIODevice::Append);
	QTextStream out(&file);
	switch (type)
	{
		case QtDebugMsg:	out << "debug    - ";	break;
		case QtInfoMsg:		out << "info     - ";	break;
		case QtWarningMsg:	out << "warning  - ";	break;
		case QtCriticalMsg:	out << "critical - ";	break;
		case QtFatalMsg:	out << "fatal    - ";	break;
	}
	out << message << "\n";
}

//!
//! Constructor
//!
Logger::Logger(void)
	: m_Enabled(false)
{
	this->SetEnabled(true);
}

//!
//! Returns wether the logger is enabled or not
//!
bool Logger::IsEnabled(void) const
{
	return m_Enabled;
}

//!
//! Set the enabled state of the logger
//!
void Logger::SetEnabled(bool enabled)
{
	if (enabled != m_Enabled)
	{
		QFile(QCoreApplication::applicationDirPath() + "/Logs.txt").remove();
		m_Enabled = enabled;
		qInstallMessageHandler(m_Enabled == true ? MessageHandler : nullptr);
		emit enabledChanged();
	}
}
