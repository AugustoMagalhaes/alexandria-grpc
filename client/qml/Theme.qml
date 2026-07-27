pragma Singleton
import QtQuick

QtObject {
    readonly property color backgroundColor: "#F7F7F5"
    readonly property color surfaceColor: "#FFFFFF"
    readonly property color primaryColor: "#2C5F6D"
    readonly property color primaryColorDark: "#234B57"
    readonly property color successColor: "#4C8C6B"
    readonly property color errorColor: "#C0504D"
    readonly property color textPrimary: "#2B2B2B"
    readonly property color textSecondary: "#6B6B6B"
    readonly property color borderColor: "#E0E0DC"

    readonly property int spacingSmall: 8
    readonly property int spacingMedium: 16
    readonly property int spacingLarge: 24

    readonly property int radiusSmall: 6
    readonly property int radiusMedium: 10

    readonly property int fontSizeSmall: 12
    readonly property int fontSizeBody: 14
    readonly property int fontSizeTitle: 20
    readonly property int fontSizeHeading: 26
}