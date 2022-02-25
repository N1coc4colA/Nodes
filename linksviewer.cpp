#include "linksviewer.h"

#include "linkstype.h"
#include "linkeditor.h"
#include "relayer.h"
#include "helpers.h"
#include "sharedinstances.h"

#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>

#include <QEvent>


void cleanup(QListWidget *w)
{
	while (w->count() != 0) {
		QListWidgetItem *item = w->takeItem(0);
		delete item;
	}
}

LinksViewer::LinksViewer(QWidget *parent)
	: QWidget(parent), availableList(this), inheritsList(this), splitter(this)
{
	lnEditor = new LinkEditor(this);

	QVBoxLayout *lay = new QVBoxLayout(this);
	lay->addWidget(&splitter);
	splitter.addWidget(&availableList);
	splitter.addWidget(&inheritsList);

	QHBoxLayout *subL = new QHBoxLayout;
	subL->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	QPushButton *add = new QPushButton(tr("Add"), this);
	QPushButton *edit = new QPushButton(tr("Edit"), this);
	subL->addWidget(add);
	subL->addWidget(edit);

	lay->addLayout(subL);

	setLayout(lay);
	setWindowFlags(Qt::Dialog | windowFlags());
	setWindowModality(Qt::WindowModality::ApplicationModal);
	hide();

	connect(SharedInstances::instance()->relayer(), &Relayer::reqLinksList, this, &LinksViewer::handleShow);
	connect(SharedInstances::instance()->relayer(), &Relayer::reqLinksListUpd, this, &LinksViewer::handleLinksUpdate);
	connect(&availableList, &QListWidget::doubleClicked, this, [this](const QModelIndex &index) {
		viewInheritanceOf(availableList.model()->data(index).toString());
	});
	connect(&inheritsList, &QListWidget::doubleClicked, this, [this](const QModelIndex &index) {
		//If the inherited type is in the list, then show it and chage the selected item to the correct one.
		QString t = inheritsList.model()->data(index).toString();
		QList<QListWidgetItem *> l = availableList.findItems(t, Qt::MatchFlag::MatchExactly);
		if (!l.isEmpty()) {
			availableList.setCurrentRow(availableList.row(l.first()));
			viewInheritanceOf(t);
		}
	});
	connect(add, &QPushButton::clicked, this, [this]() {
		SharedInstances::instance()->relayer()->linkEditor(this);
	});
	connect(edit, &QPushButton::clicked, SharedInstances::instance()->relayer(), [this]() {
		SharedInstances::instance()->relayer()->linkEditorFrom(this, SharedInstances::instance()->typesHolder()->getByName(availableList.currentItem()->text()));
	});
}

LinksViewer::~LinksViewer()
{
	printTrace();
}

void LinksViewer::handleShow()
{
	SharedInstances::instance()->relayer()->disable();
	setEnabled(true);
	handleLinksUpdate();
	centerWidget(parentWidget(), this);
	availableList.setFocus();
	show();
}

void LinksViewer::closeEvent(QCloseEvent *e)
{
	SharedInstances::instance()->relayer()->enable();
	return QWidget::closeEvent(e);
}

void LinksViewer::handleLinksUpdate()
{
	//This function might be called twice while showing the widget. So use a mutex-like.
	if (updating) {
		return;
	}
	updating = true;
	availableList.clear();
	inheritsList.clear();

	for (auto t : SharedInstances::instance()->typesHolder()->types) {
		QListWidgetItem *it = new QListWidgetItem(&availableList);
		it->setText(t.type->name);
		availableList.addItem(it);
	}
	updating = false;
}

void LinksViewer::viewInheritanceOf(QString v)
{
	cleanup(&inheritsList);

	QList<QString> inherits = SharedInstances::instance()->typesHolder()->resolveInherits(v);
	for (auto s : inherits) {
		QListWidgetItem *it = new QListWidgetItem(&inheritsList);
		it->setText(s);
		inheritsList.addItem(it);
	}
}
