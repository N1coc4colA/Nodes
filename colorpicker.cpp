#include "colorpicker.h"


ColorPicker::ColorPicker(QWidget *p)
	: QPushButton(p), dial(Qt::yellow)
{
	setFixedSize(50, 30);
	connect(this, &ColorPicker::clicked, &dial, &QColorDialog::exec);
	connect(&dial, &QColorDialog::colorSelected, this, &ColorPicker::setupColor);

	dial.setOption(QColorDialog::ShowAlphaChannel);
	setColor(Qt::yellow);
}


ColorPicker::ColorPicker(QColor c, QWidget *p)
	: QPushButton(p), dial(p)
{
	setFixedSize(50, 30);
	connect(this, &ColorPicker::clicked, &dial, &QColorDialog::exec);
	connect(&dial, &QColorDialog::colorSelected, this, &ColorPicker::setupColor);
	setColor(c);
}

ColorPicker::~ColorPicker() {}

void ColorPicker::setColor(QColor c)
{
	dial.setCurrentColor(c);
	setupColor(c);
}

void ColorPicker::setupColor(QColor c)
{
	setStyleSheet("background-color: " + c.name() + ";");
	Q_EMIT colorChanged(c);
}

QColor ColorPicker::color()
{
	return dial.currentColor();
}
