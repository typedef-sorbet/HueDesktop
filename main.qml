import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.3
import com.sanctity 1.0
import "Utilities.js" as Utils
import "Icon.js" as MdiFont

Window {
    id: window

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
                manager.changeLights(value, green_slider.value, blue_slider.value, Utils.calc_alpha(value, green_slider.value, blue_slider.value))
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
                manager.changeLights(red_slider.value, value, blue_slider.value, Utils.calc_alpha(red_slider.value, value, blue_slider.value))
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
                manager.changeLights(red_slider.value, green_slider.value, value, Utils.calc_alpha(red_slider.value, green_slider.value, value))
            }
        }
    }

    Rectangle {
        id: border
    }

    Rectangle {
        id: color_preview

        width: 50; height: 50

        border.color: "black"
        anchors.margins: 40
        anchors.top: green_slider.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        color: Qt.rgba(red_slider.value / 255, green_slider.value / 255, blue_slider.value / 255, 1.0)

        Binding on color {
            when: hex_edit.activeFocus
            value: hex_edit.text.length == 6 ? Qt.rgba(parseInt(hex_edit.text.substring(0, 2), 16) / 255, parseInt(hex_edit.text.substring(2, 4), 16) / 255, parseInt(hex_edit.text.substring(4, 6), 16) / 255, 1.0) : Qt.rgba(red_slider.value / 255, green_slider.value / 255, blue_slider.value / 255, 1.0)
        }

        onColorChanged: {
            console.log("Color changed to " + color)
        }
    }

    Text {
        id: hash_sign

        text: "#"
        font.pointSize: 12

        anchors.margins: 3
        anchors.right: hex_edit.left
        anchors.verticalCenter: hex_edit.verticalCenter
    }

    TextField {
        id: hex_edit

        text: Qt.rgba(red_slider.value / 255, green_slider.value / 255, blue_slider.value / 255, 1.0).toString().substring(1, 8)
        font.pointSize: 12
        font.family: "Courier New"

        width: 75; height: 30
        anchors.margins: 10
        anchors.top: color_preview.bottom
        anchors.horizontalCenter: color_preview.horizontalCenter

        onTextEdited: {
            if (text.length == 6) {
                var red = parseInt(text.substring(0, 2), 16)
                var green = parseInt(text.substring(2, 4), 16)
                var blue = parseInt(text.substring(4, 6), 16)

                if((!red && red !== 0) || (!green && green !== 0) || (!blue && blue !== 0))
                {
                    console.log("bad text given to hex_edit")
                    return;
                }

                red_slider.value = red
                green_slider.value = green
                blue_slider.value = blue

                manager.changeLights(red, green, blue, Utils.calc_alpha(red, green, blue))
            }
        }
    }

    RoundButton {
        id: on_off

        property bool next_action: false

        anchors.margins: 30
        anchors.top: hex_edit.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Material Design Icons"
        font.pixelSize: 24
        text: MdiFont.Icon.power
        opacity: 0.8

        onPressed: {
            manager.switchLights(next_action)
            next_action = !next_action
        }
    }
}
