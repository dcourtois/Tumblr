import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Tumblr 0.1


//
// Main application window
//
ApplicationWindow {
	id: app
	visible: true
	minimumWidth: 700
	width: 800
	height: 600
	title: "Tumblr"

	// custom properties
	property int countWidth: 100

	// our database
	TumblrDatabase {
		id: tumblrDatabase
	}

	// the settings for the window's state, position, size.
	WindowSettings {
		category: "MainWindow"
		window: app
	}

	// global settings
	TumblrSettings {
		id: tumblrSettings
		tumblrDatabase: tumblrDatabase
	}

	// settings for a single blog (is also used to add new blogs)
	BlogSettings {
		id: blogSettings
	}

	// blog removal confirmation
	RemoveBlog {
		id: removeBlog
		tumblrDatabase: tumblrDatabase
	}

	// main layout
	ColumnLayout {
		anchors { fill: parent; margins: 10 }

		// display the blogs
		Repeater {
			model: tumblrDatabase

			RowLayout {
				Image {
					source: avatar
				}

				Label {
					text: name
				}

				Item {
					Layout.fillWidth: true
				}

				Label {
					text: info
				}

				Item {
					Layout.minimumWidth: 20
				}

				Label {
					text: todoCount + "  /  " + mediaCount
					Layout.minimumWidth: countWidth
				}

				Button {
					text: updating === false ? "Update" : "Cancel"
					onClicked: updating === false ? tumblrDatabase.update(index) : tumblrDatabase.cancel(index)
				}

				Button {
					text: "Remove"
					enabled: updating === false
					onClicked: {
						removeBlog.blogIndex = index;
						removeBlog.open();
					}
				}

				Button {
					text: "Settings"
					enabled: updating === false
					onClicked: {
						blogSettings.blog = tumblrDatabase.blog(index);
						blogSettings.open();
					}
				}
			}
		}

		// fill space
		Item {
			Layout.fillHeight: true
		}

		// controls
		RowLayout {
			Button {
				text: "Update All"
				onClicked: tumblrDatabase.updateAll()
			}

			Item {
				Layout.fillWidth: true
			}

			Button {
				text: "Settings"
				onClicked: tumblrSettings.open();
			}

			Item {
				Layout.fillWidth: true
			}

			Button {
				text: "Add New Blog"
				onClicked: {
					blogSettings.blog = undefined;
					blogSettings.open();
				}
			}
		}
	}
}
