import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2 as Dialogs
import Qt.labs.platform 1.0 as Platform
import QtQuick.Layouts 1.3


Dialog {
	title: "Settings"
	standardButtons: blog ? Dialog.Ok : Dialog.Ok | Dialog.Cancel

	// private properties
	property int _labelWidth: 150

	// externally set
	property var blog

	// main layout
	ColumnLayout {
		anchors.fill: parent

		//
		// The blog's url
		//
		RowLayout {
			spacing: 10
			Label {
				Layout.minimumWidth: _labelWidth
				text: "Address"
				horizontalAlignment: Text.AlignRight
			}
			TextField {
				Layout.fillWidth: true
				id: address
				placeholderText: "Url of the Tumblr blog"
				text: blog ? blog.url : "";
				readOnly: blog ? true : false
				validator: RegExpValidator {
					regExp: /(https?:\/\/)?[^\.]+\.tumblr\.com\/?/
				}
			}
		}

		//
		// The output folder
		//
		RowLayout {
			spacing: 10
			Label {
				Layout.minimumWidth: _labelWidth
				text: "Output Folder"
				horizontalAlignment: Text.AlignRight
			}
			TextField {
				Layout.fillWidth: true
				id: output
				placeholderText: "Where the medias will be saved"
				text: blog ? blog.output : "";
				readOnly: true
				onTextChanged: if (blog) { blog.output = text; }
			}
			Button {
				text: "Browse"

				Dialogs.FileDialog {
					id: outputFolder
					selectFolder: true
					folder: output.text ? output.text : Platform.StandardPaths.standardLocations(Platform.StandardPaths.PicturesLocation)[0]
					onAccepted: output.text = folder.toString().replace("file://", "");
				}

				onClicked: outputFolder.open()
			}
		}

		//
		// The max numbeer of pages to scan
		//
		RowLayout {
			spacing: 10
			Label {
				Layout.minimumWidth: _labelWidth
				text: "Max Page Update"
				horizontalAlignment: Text.AlignRight
			}
			TextField {
				Layout.fillWidth: true
				id: maxPageUpdate
				placeholderText: "The maximum number of pages to scan on updates. -1 for 'no limit'."
				text: blog ? blog.maxPageUpdate : "10";
				onTextChanged: if (blog) { blog.maxPageUpdate = parseInt(text); }
				validator: IntValidator { }
			}
		}

		//
		// fill the remaining space
		//
		Item {
			Layout.fillHeight: true
			Layout.columnSpan: 2
		}
	}

	//
	// add the blog when the user pressed Ok
	//
	onAccepted: {
		if (blog !== undefined) {
			return;
		}

		// validate settings
		if (address.text.length > 0 && output.text.length > 0) {
			tumblrDatabase.addBlog(address.text, output.text, parseInt(maxPageUpdate.text));
		}
	}
}
