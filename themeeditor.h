#ifndef THEMEEDITOR_H
#define THEMEEDITOR_H

#include "colorpicker.h"

class QCheckBox;

class ThemeEditor : public QWidget
{
	Q_OBJECT
public:
	explicit ThemeEditor(QWidget *parent = nullptr);

protected:
	bool event(QEvent *e) override;
	void closeEvent(QCloseEvent *e) override;
	void showEvent(QShowEvent *e) override;

	void loadColors();
	void setColorsFromTheme();
	void changeAutomaticTheming(int);
	void setColors();

private:
	QWidget *container = nullptr;
	QCheckBox *automatic = nullptr;
	ColorPicker *pTitle = nullptr;
	ColorPicker *pType = nullptr;
	ColorPicker *pOutter = nullptr;
	ColorPicker *pInner = nullptr;
	ColorPicker *pRight = nullptr;
	ColorPicker *pLeft = nullptr;

	bool updatedRightNow = false;
};

#endif // THEMEEDITOR_H
