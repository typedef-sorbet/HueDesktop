import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.3
import com.sanctity 1.0
import "Utilities.js" as Utils
import "Icon.js" as MdiFont

ApplicationWindow {
    id: window

    visible: true
    width: 300
    height: 600
    title: qsTr("Hue Lights Controller")

    color: "lightgrey"

    property bool on_state: true

    function getWhich()
    {
        var which_box = (lights_or_groups.currentIndex == 0 ? choose_which_light : choose_which_group)
        return which_box.model[which_box.currentIndex]
    }

    header: TabBar {
        id: bar
        anchors.margins: 30
        width: parent.width

        currentIndex: page_manager.currentIndex

        TabButton {
            text: qsTr("Color")

            onPressed: page_manager.setCurrentIndex(0)
        }
        TabButton {
            text: qsTr("Scenes")

            onPressed: page_manager.setCurrentIndex(1)
        }
    }

    SwipeView {
        id: page_manager
        anchors.fill: parent

        currentIndex: 0

        Item {
            id: colors_page

            ComboBox {
                id: lights_or_groups

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 30
                anchors.rightMargin: 25
                width: 90

                currentIndex: 0

                model: ListModel {
                    ListElement {
                        text: "Light"
                    }

                    ListElement {
                        text: "Group"
                    }
                }

                onCurrentIndexChanged:
                {
                    if(model[currentIndex] === "Light")
                    {
                        choose_which_light.visible = true
                        choose_which_group.visible = false
                    }
                    else
                    {
                        choose_which_light.visible = false
                        choose_which_group.visible = true
                    }
                }
            }

            ComboBox {
                id: choose_which_light

                anchors.margins: 30
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.left: lights_or_groups.right

                model: lights_model
                visible: true

                currentIndex: 0
            }

            ComboBox {
                id: choose_which_group

                anchors.margins: 30
                anchors.rightMargin: 25
                anchors.top: parent.top
                anchors.left: lights_or_groups.right

                model: groups_model
                visible: false

                currentIndex: 0
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
                        manager.changeLights(value, green_slider.value, blue_slider.value, brightness_slider.value, lights_or_groups.currentIndex, getWhich())
                    }
                }
            }

            Slider {
                id: green_slider

                // Formatting

                anchors.leftMargin: 40
                anchors.rightMargin: 40
                anchors.topMargin: 15
                anchors.top: lights_or_groups.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                orientation: Qt.Vertical

                from: 0; to: 255
                value: 255
                stepSize: 1

                onPressedChanged: {
                    if(!pressed)
                    {
                        manager.changeLights(red_slider.value, value, blue_slider.value, brightness_slider.value, lights_or_groups.currentIndex, getWhich())
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
                        manager.changeLights(red_slider.value, green_slider.value, value, brightness_slider.value, lights_or_groups.currentIndex, getWhich())
                    }
                }
            }

            Slider {
                id: brightness_slider

                from: 1; to: 254
                value: 254
                stepSize: 1

                anchors.topMargin: 30
                anchors.bottomMargin: 30
                anchors.top: green_slider.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                onPressedChanged: {
                    if(!pressed)
                    {
                        manager.changeLights(red_slider.value, green_slider.value, blue_slider.value, value, lights_or_groups.currentIndex, getWhich())

                        brightness_slider_scene.value = value
                    }
                }
            }

            Text {
                id: brightness_label_lo

                anchors.right: brightness_slider.left
                anchors.verticalCenter: brightness_slider.verticalCenter

                font.family: "Material Design Font"
                font.pixelSize: 12
                text: MdiFont.Icon.brightness3
            }

            Text {
                id: brightness_label_hi

                anchors.left: brightness_slider.right
                anchors.verticalCenter: brightness_slider.verticalCenter

                font.family: "Material Design Font"
                font.pixelSize: 12
                text: MdiFont.Icon.brightness5
            }

            Rectangle {
                id: color_preview

                width: 50; height: 50

                border.color: "black"
                anchors.margins: 30
                anchors.top: brightness_slider.bottom
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

                        manager.changeLights(red, green, blue, brightness_slider.value, lights_or_groups.currentIndex, getWhich())
                    }
                }
            }

            RoundButton {
                id: on_off

                property bool next_action: !window.on_state

                anchors.margins: 30
                anchors.top: hex_edit.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: "Material Design Icons"
                font.pixelSize: 24
                text: MdiFont.Icon.power
                opacity: 0.8

                onPressed: {
                    manager.switchLights(next_action)
                    window.on_state = next_action
                }
            }
        }

        Item {
            id: scenes_page

            ComboBox {
                id: scene_selector

                anchors.centerIn: parent
                anchors.margins: 30
                model: scenes_model

                currentIndex: 0

                onCurrentIndexChanged: {
                    console.log("Scene changed to " + model[currentIndex])
                    manager.setScene(model[currentIndex])
                }
            }

            Slider {
                id: brightness_slider_scene

                from: 1; to: 254
                value: 254
                stepSize: 1

                anchors.topMargin: 30
                anchors.bottomMargin: 30
                anchors.top: scene_selector.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                onPressedChanged: {
                    if(!pressed)
                    {
                        manager.changeLights(red_slider.value, green_slider.value, blue_slider.value, value)

                        brightness_slider.value = value
                    }
                }
            }

            Text {
                id: brightness_label_lo_scene

                anchors.right: brightness_slider_scene.left
                anchors.verticalCenter: brightness_slider_scene.verticalCenter

                font.family: "Material Design Font"
                font.pixelSize: 12
                text: MdiFont.Icon.brightness3
            }

            Text {
                id: brightness_label_hi_scene

                anchors.left: brightness_slider_scene.right
                anchors.verticalCenter: brightness_slider_scene.verticalCenter

                font.family: "Material Design Font"
                font.pixelSize: 12
                text: MdiFont.Icon.brightness5
            }

            RoundButton {
                id: on_off_scene

                property bool next_action: !window.on_state

                anchors.margins: 30
                anchors.top: brightness_slider_scene.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: "Material Design Icons"
                font.pixelSize: 24
                text: MdiFont.Icon.power
                opacity: 0.8

                onPressed: {
                    manager.switchLights(next_action)
                    window.on_state = next_action
                }
            }
        }
    }
}
