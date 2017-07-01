#include "TumblrPCH.h"
#include "RegisterTypes.h"

#include "Tumblr/TumblrDatabase.h"


namespace Tumblr
{

	//! The Uri
	static const char * uri = "Tumblr";

	//! Major version number
	static const int major = 0;

	//! Minor version number
	static const int minor = 1;

	//!
	//! Register all our custom types. This must be called before
	//! loading the main Qml file.
	//!
	void RegisterTypes(void)
	{
		// required to use custom types as properties
		qRegisterMetaType< Blog * >("Blog*");

		// register our QMLtypes
		qmlRegisterType< Tumblr::Database >(uri, major, minor, "TumblrDatabase");
	}

} // namespace Topo
