import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2 as Dialogs
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0


Dialog {
	title: "Settings"
	standardButtons: Dialog.Ok

	// private properties
	property int _labelWidth: 150
	property int _keyWidth: 550

	// externally set
	property var tumblrDatabase
	property var settings

	// the settings
	Settings {
		id: settings
		property string oauthKey: ""
		property string oauthSecret: ""
		property string databaseLocation: "System"
	}

	// main layout
	ColumnLayout {
		anchors.fill: parent

		// OAuth key
		RowLayout {
			spacing: 10
			Label {
				Layout.minimumWidth: _labelWidth
				text: "Key"
				horizontalAlignment: Text.AlignRight
			}
			TextField {
				Layout.fillWidth: true
				Layout.minimumWidth: _keyWidth
				placeholderText: "Tumblr API's OAuth Key"
				text: settings.oauthKey
				onTextChanged: settings.oauthKey = text
			}
		}

		// OAuth secret
		RowLayout {
			spacing: 10
			Label {
				Layout.minimumWidth: _labelWidth
				text: "Secret"
				horizontalAlignment: Text.AlignRight
			}
			TextField {
				Layout.fillWidth: true
				Layout.minimumWidth: _keyWidth
				placeholderText: "Tumblr API's OAuth Secret"
				text: settings.oauthSecret
				onTextChanged: settings.oauthSecret = text
			}
		}

		// where to save our database
		RowLayout {
			spacing: 10
			Label {
				Layout.minimumWidth: _labelWidth
				text: "Database Location"
				horizontalAlignment: Text.AlignRight
			}
			ComboBox {
				Layout.fillWidth: true
				currentIndex: find(settings.databaseLocation);
				model: [
					"System",
					"Application"
				]
				onCurrentTextChanged: settings.databaseLocation = currentText
			}
		}

		// import database from somewhere
		Button {
			Layout.fillWidth: true
			text: "Import Database"
			onPressed: chooseDatabase.open()
			Dialogs.FileDialog {
				id: chooseDatabase
				onAccepted: tumblrDatabase.importDatabase(fileUrl.toString().replace("file://", ""));
			}
		}

		// fill the remaining space
		Item {
			Layout.fillHeight: true
			Layout.columnSpan: 2
		}
	}
}
