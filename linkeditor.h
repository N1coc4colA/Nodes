#ifndef LINKEDITOR_H
#define LINKEDITOR_H

#include "linkstype.h"
#include "colorpicker.h"

#include <QListWidget>
#include <QLineEdit>
#include <QValidator>

class LinkNameValidator : public QValidator
{
	Q_OBJECT
public:
	explicit LinkNameValidator(bool invert = false, QObject *p = nullptr);

	QValidator::State validate(QString &inp, int &p) const override;

private:
	bool inv;
};

class LinkEditor : public QWidget
{
	Q_OBJECT
public:
	explicit LinkEditor(QWidget *parent = nullptr);

public Q_SLOTS:
	void addToInherits();
	void rmvFromInherits();
	void generateLink();

protected:
	void closeEvent(QCloseEvent *e) override;
	void showEvent(QShowEvent *e) override;

	void cleanup();

private:
	LinkNameValidator c_name_vtor;
	LinkNameValidator i_name_vtor;
	QListWidget list;
	QLineEdit name;
	QLineEdit inh;
	QWidget *old;
	LnTypeHolder hold;
	ColorPicker picker;
	bool editMode;
};

#endif // LINKEDITOR_H
