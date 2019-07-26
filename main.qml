import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.3
import com.sanctity 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    HueRequestManager {
        id: manager
    }

    RoundButton {
        text: "Change"

        anchors.centerIn: parent

        onPressed: {
            manager.changeLights(255, 0, 0);
        }
    }
}
