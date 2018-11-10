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
				id: key
				Layout.fillWidth: true
				Layout.minimumWidth: _keyWidth
				placeholderText: "Tumblr API's OAuth Key"
				text: tumblrDatabase.key
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
				id: secret
				Layout.fillWidth: true
				Layout.minimumWidth: _keyWidth
				placeholderText: "Tumblr API's OAuth Secret"
				text: tumblrDatabase.secret
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
				id: location
				Layout.fillWidth: true
				model: [
					"System",
					"Application"
				]
				Component.onCompleted: currentIndex = find(tumblrDatabase.location)
			}
		}

		// enabled logging to file
		RowLayout {
			spacing: 10
			Label {
				Layout.minimumWidth: _labelWidth
				text: "Enabled Logs"
				horizontalAlignment: Text.AlignRight
			}
			CheckBox {
				id: logging
				Layout.fillWidth: false
				Component.onCompleted: checked = logger.enabled
			}
		}

		// import database from somewhere
		Button {
			Layout.fillWidth: true
			text: "Import Database"
			onPressed: chooseDatabase.open()
			Dialogs.FileDialog {
				id: chooseDatabase
				onAccepted: tumblrDatabase.importDatabase(fileUrl.toString().replace("file:///", ""));
			}
		}

		// fill the remaining space
		Item {
			Layout.fillHeight: true
			Layout.columnSpan: 2
		}
	}

	// on ok, set the database options
	onAccepted: {
		logger.enabled = logging.checked;
		tumblrDatabase.location = location.currentText;
		tumblrDatabase.secret = secret.text;
		tumblrDatabase.key = key.text;
	}
}
