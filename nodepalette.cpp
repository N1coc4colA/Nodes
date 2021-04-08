#include "nodepalette.h"

NodePalette::NodePalette() : QPalette()
{
    setColor(QPalette::Window, QColor(27, 35, 38));
            setColor(QPalette::WindowText, QColor(234, 234, 234));
            setColor(QPalette::Base, QColor(27, 35, 38));
            setColor(QPalette::Disabled, QPalette::Base, QColor(27 + 5, 35 + 5, 38 + 5));
            setColor(QPalette::AlternateBase, QColor(12, 15, 16));
            setColor(QPalette::ToolTipBase, QColor(27, 35, 38));
            setColor(QPalette::ToolTipText, Qt::white);
            setColor(QPalette::Text, QColor(200, 200, 200));
            setColor(QPalette::Disabled, QPalette::Text, QColor(100, 100, 100));
            setColor(QPalette::Button, QColor(27, 35, 38));
            setColor(QPalette::ButtonText, Qt::white);
            setColor(QPalette::BrightText, QColor(100, 215, 222));
            setColor(QPalette::Link, QColor(126, 71, 130));
            setColor(QPalette::Highlight, QColor(126, 71, 130));
            setColor(QPalette::HighlightedText, Qt::white);
            setColor(QPalette::Disabled, QPalette::Light, Qt::black);
            setColor(QPalette::Disabled, QPalette::Shadow, QColor(12, 15, 16));
}
