import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.3
import com.sanctity 1.0

Window {
    visible: true
    width: 300
    height: 480
    title: qsTr("Hello World")

    color: "lightgrey"

    HueRequestManager {
        id: manager
    }

    Slider {
        id: red_slider

        // Formatting

        anchors.leftMargin: 40
        anchors.rightMargin: 40
        anchors.right: green_slider.left
        anchors.verticalCenter: green_slider.verticalCenter
        orientation: Qt.Vertical

        from: 0; to: 255
        value: 255
        stepSize: 1

        onPressedChanged: {
            if(!pressed)
            {
                manager.changeLights(value, green_slider.value, blue_slider.value)
            }
        }
    }

    Slider {
        id: green_slider

        // Formatting

        anchors.leftMargin: 40
        anchors.rightMargin: 40
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        orientation: Qt.Vertical

        from: 0; to: 255
        value: 255
        stepSize: 1

        onPressedChanged: {
            if(!pressed)
            {
                manager.changeLights(red_slider.value, value, blue_slider.value)
            }
        }
    }

    Slider {
        id: blue_slider

        // Formatting

        anchors.leftMargin: 40
        anchors.rightMargin: 40
        anchors.left: green_slider.right
        anchors.verticalCenter: green_slider.verticalCenter
        orientation: Qt.Vertical

        from: 0; to: 255
        value: 255
        stepSize: 1

        // Uncomment this if you wanna stress test my color updating implementation.
        // Spoilers: It doesn't do smooth updating. Like, at all.

//        onValueChanged: {
//            manager.changeLights(red_slider.value, green_slider.value, value)
//        }

        onPressedChanged: {
            if(!pressed)
            {
                manager.changeLights(red_slider.value, green_slider.value, value)
            }
        }
    }

    Rectangle {
        id: border
    }

    Rectangle {
        width: 50; height: 50

        border.color: "black"
        anchors.margins: 40
        anchors.top: green_slider.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        color: Qt.rgba(red_slider.value / 255, green_slider.value / 255, blue_slider.value / 255, 1.0)

        onColorChanged: {
            console.log("Color changed to " + color)
        }
    }
}
