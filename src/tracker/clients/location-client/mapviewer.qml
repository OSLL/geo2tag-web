import Qt 4.7
import QtMobility.location 1.2
import com.nokia.meego 1.0

 Item {
     width: 500
     height: 500

     focus : true
     TitleBar { id: titleBar; z: 5; width: parent.width; height: 40; /*opacity: 0.9*/ }
     RecButton {id: recbutton;width: 100; height: 100;  z:5}

     Map {
         id: map
         z : 1
         plugin : Plugin {
                             name : "nokia"
                         }
         size.width: parent.width
         size.height: parent.height
         zoomLevel: 7
         center: Coordinate {
             latitude: 60
             longitude: 30
         }



         MapCircle {
             id : circle
             center : Coordinate {
                         latitude : 60
                         longitude : 30
                     }
             color : "#80FF0000"
             radius : 1000.0
             MapMouseArea {
                 onPositionChanged: {
                     if (mouse.button == Qt.LeftButton)
                         circle.center = mouse.coordinate
                     if (mouse.button == Qt.RightButton)
                         circle.radius = circle.center.distanceTo(mouse.coordinate)
                 }
             }
         }

         MapMouseArea {
             property int lastX : -1
             property int lastY : -1

             onPressed : {
                 lastX = mouse.x
                 lastY = mouse.y
             }
             onReleased : {
                 lastX = -1
                 lastY = -1
             }
             onPositionChanged: {
                 if (mouse.button == Qt.LeftButton) {
                     if ((lastX != -1) && (lastY != -1)) {
                         var dx = mouse.x - lastX
                         var dy = mouse.y - lastY
                         map.pan(-dx, -dy)
                     }
                     lastX = mouse.x
                     lastY = mouse.y
                 }
             }
             onDoubleClicked: {
                 map.center = mouse.coordinate
                 map.zoomLevel += 1
                 lastX = -1
                 lastY = -1
             }
         }
     }

     Keys.onPressed: {
         if (event.key == Qt.Key_Plus) {
             map.zoomLevel += 1
         } else if (event.key == Qt.Key_Minus) {
             map.zoomLevel -= 1
         } else if (event.key == Qt.Key_T) {
             if (map.mapType == Map.StreetMap) {
                 map.mapType = Map.SatelliteMapDay
             } else if (map.mapType == Map.SatelliteMapDay) {
                 map.mapType = Map.StreetMap
             }
         }
     }

 }