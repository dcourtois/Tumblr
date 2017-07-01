import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


Dialog {
	title: "Are You Sure ?"
	standardButtons: Dialog.Yes | Dialog.No

	// private properties
	property int _labelWidth: 100
	property var _blog: (tumblrDatabase && blogIndex) ? tumblrDatabase.blog(blogIndex) : undefined

	// externally set
	property var tumblrDatabase
	property var blogIndex

	// main layout
	ColumnLayout {
		anchors.fill: parent

		// Indications
		Label {
			text: _blog ? ("Removing Blog " + _blog.name) : ""
		}

		// Remove images
		RowLayout {
			spacing: 10
			Label {
				Layout.minimumWidth: _labelWidth
				text: "Delete Medias"
				horizontalAlignment: Text.AlignRight
			}
			CheckBox {
				id: deleteMedias
				Layout.fillWidth: true
			}
		}

		// fill the remaining space
		Item {
			Layout.fillHeight: true
			Layout.columnSpan: 2
		}
	}

	// the user confirmed
	onAccepted: tumblrDatabase.remove(blogIndex, deleteMedias.checked);
}
