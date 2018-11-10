#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H


//!
//! Really simple class that allows the user to install a message
//! handler to log everything to file, and to expose this in QML
//!
class Logger
	: public QObject
{

	Q_OBJECT

	Q_PROPERTY(bool enabled READ IsEnabled WRITE SetEnabled NOTIFY enabledChanged)

signals:

	void enabledChanged(void);

public:

	Logger(void);

	// public C++ API
	bool	IsEnabled(void) const;
	void	SetEnabled(bool enabled);

private:

	//! Enabled state of the logger
	bool m_Enabled;

};


#endif // UTILS_LOGGER_H
