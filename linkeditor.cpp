#include "linkeditor.h"

#include "linkstype.h"
#include "relayer.h"
#include "helpers.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QLabel>

LinkNameValidator::LinkNameValidator(bool v, QObject *p) : QValidator(p), inv(v) {}

QValidator::State LinkNameValidator::validate(QString &inp, int &) const
{
	return (LinksTypeHolder::instance()->typeNames().contains(inp)
				? (inv
					? QValidator::State::Invalid
					: QValidator::State::Acceptable)
				: (inv
					? QValidator::State::Acceptable
					: QValidator::State::Intermediate));
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

LinkEditor::LinkEditor(QWidget *parent)
	: QWidget(parent), c_name_vtor(true, this), i_name_vtor(false, this), list(this), name(this), hold(nullptr, this), picker(Qt::green, this), editMode(false)
{
	QVBoxLayout *mLayout = new QVBoxLayout(this);
	QHBoxLayout *bLayout = new QHBoxLayout;
	QHBoxLayout *cLayout = new QHBoxLayout;

	QPushButton *add = new QPushButton(tr("Add"), this);
	QPushButton *rmv = new QPushButton(tr("Remove"), this);
	QPushButton *ok = new QPushButton(tr("Add type"), this);

	name.setValidator(&c_name_vtor);
	inh.setValidator(&i_name_vtor);

	cLayout->addWidget(new QLabel(tr("Color: "), this));
	cLayout->addWidget(&picker);

	bLayout->addWidget(add);
	bLayout->addWidget(rmv);

	mLayout->addWidget(&name);
	mLayout->addLayout(cLayout);
	mLayout->addWidget(&list);
	mLayout->addWidget(&inh);
	mLayout->addLayout(bLayout);
	mLayout->addWidget(ok);

	setLayout(mLayout);
	setWindowFlags(windowFlags() | Qt::Dialog);
	setWindowModality(Qt::WindowModality::ApplicationModal);

	connect(rmv, &QPushButton::clicked, this, &LinkEditor::rmvFromInherits);
	connect(add, &QPushButton::clicked, this, &LinkEditor::addToInherits);
	connect(&inh, &QLineEdit::returnPressed, this, &LinkEditor::addToInherits);
	connect(ok, &QPushButton::clicked, this, &LinkEditor::generateLink);
	connect(Relayer::instance(), &Relayer::reqLnEditor, this, [this](QWidget *p) {
		cleanup();
		old = p;
		centerWidget(old, this);
		show();
		editMode = false;
		picker.setColor(Qt::gray);
	});
	connect(Relayer::instance(), &Relayer::reqLnEditorFrom, this, [this](QWidget *p, LnTypeHolder h) {
		cleanup();
		old = p;
		centerWidget(old, this);
		show();

		if (h.valid) {
			int i = 0;
			while (i < h.type->inherits.length()) {
				QListWidgetItem *it = new QListWidgetItem(&list);
				it->setText(LinksTypeHolder::instance()->getByUID(h.type->inherits[i]).type->name);
				list.addItem(it);
				i++;
			}
			editMode = true;
			hold = h;
			picker.setColor(hold.type->color);
		}
	});
}

void LinkEditor::closeEvent(QCloseEvent *e)
{
	old->setEnabled(true);
	return QWidget::closeEvent(e);
}

void LinkEditor::showEvent(QShowEvent *e)
{
	old->setEnabled(false);
	setEnabled(true);
	name.setFocus();
	return QWidget::showEvent(e);
}

void LinkEditor::addToInherits()
{
	QString n = inh.text();

	if (!list.findItems(n, Qt::MatchFlag::MatchExactly).isEmpty()) {
		return;
	}
	int p = 0;
	if (i_name_vtor.validate(n, p) == QValidator::State::Acceptable) {
		QListWidgetItem *it = new QListWidgetItem(&list);
		it->setText(n);
		list.addItem(it);
	}
}

void LinkEditor::rmvFromInherits()
{
	int r = list.currentRow();
	if (r > -1) {
		QListWidgetItem *it = list.takeItem(r);
		delete it;
	}
}

void LinkEditor::generateLink()
{
	QList<int> nt;
	int i = 0;
	while (i < list.count()) {
		QString p = list.item(i)->text();
		LnTypeHolder hold = LinksTypeHolder::instance()->getByName(p);
		if (hold.valid) {
			nt << hold.type->uid;
		}
		i++;
	}

	if (editMode) {
		if (hold.valid) {
			hold.type->name = name.text();
			hold.type->inherits = nt;
			hold.type->color = picker.color();
			close();
			hold.type->notifyEdit();
			hold = nullptr;
			return;
		}
	}
	QString n = name.text();
	int p = 0;
	if (c_name_vtor.validate(n, p) == QValidator::State::Acceptable) {
		LinksTypeHolder::instance()->addType(n, nt, picker.color());
		close();
	}
}

void LinkEditor::cleanup()
{
	name.clear();
	inh.clear();
	list.clear();
}
