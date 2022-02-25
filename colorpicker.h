#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QPushButton>
#include <QColorDialog>

class ColorPicker : public QPushButton
{
	Q_OBJECT
public:
	explicit ColorPicker(QWidget *parent = nullptr);
	explicit ColorPicker(QColor c, QWidget *p = nullptr);
	~ColorPicker();

	QColor color();
	void setColor(QColor c);
	Q_SIGNAL void colorChanged(QColor c);

protected:
	void setupColor(QColor c);

private:
	QColorDialog dial;
};

#endif // COLORPICKER_H
