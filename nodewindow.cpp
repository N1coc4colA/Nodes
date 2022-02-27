#include "nodewindow.h"

#include "topbar.h"
#include "nodewidget.h"
#include "nodepalette.h"
#include "nodeeditor.h"
#include "nodeitem.h"

#include "nodessaver.h"
#include "nodesloader.h"
#include "loadingdialog.h"
#include "linksviewer.h"
#include "relayer.h"
#include "sharedinstances.h"
#include "helpers.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>

#include <QAction>
#include <QShortcut>

#include <QMetaEnum>


NodeWindow::NodeWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Nodes");
	QWidget *mainWidget = new QWidget(this);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	NodeWidget *nodeWidget = new NodeWidget(this);
	TopBar *bar = new TopBar(this);
	vwr = new LinksViewer(/*this*/); //This wonderful thing somehow gets deleted TWICE, TWICE! While added as child.

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(nodeWidget);

    mainLayout->addWidget(bar);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);

	//Setup shortcuts
	QAction *_save = new QAction(this);
	QAction *_open = new QAction(this);
	QAction *_add = new QAction(this);
	QAction *_edit = new QAction(this);
	QAction *_remove = new QAction(this);

	_save->setShortcut(QKeySequence::StandardKey::Save);
	_open->setShortcut(QKeySequence::StandardKey::Open);
	_remove->setShortcut(QKeySequence::StandardKey::Delete);
	//Don't ask me why CTRL & SHIFT are not recognized well by Qt on all Linux platforms bouh!
	_add->setShortcut(QKeySequence("Shift+N"));
	_edit->setShortcut(QKeySequence("Shift+E"));

	addActions({_save, _open, _add, _edit, _remove});

	//Setup connections
	connect(_save, &QAction::triggered, SharedInstances::instance()->relayer(), &Relayer::runSave);
	connect(_open, &QAction::triggered, SharedInstances::instance()->relayer(), &Relayer::runLoad);
	connect(_add, &QAction::triggered, SharedInstances::instance()->relayer(), &Relayer::addNode);
	connect(_edit, &QAction::triggered, SharedInstances::instance()->relayer(), &Relayer::editCurrent);
	connect(_remove, &QAction::triggered, SharedInstances::instance()->relayer(), &Relayer::rmElement);

	connect(SharedInstances::instance()->relayer(), &Relayer::reqEnable, this, [this]() {setEnabled(true);});
	connect(SharedInstances::instance()->relayer(), &Relayer::reqDisable, this, [this]() {setEnabled(false);});
	connect(SharedInstances::instance()->relayer(), &Relayer::reqAddNode, this, [nodeWidget, this](){
		NodeEditor *edits = new NodeEditor;
		edits->resize(400, 200);
		edits->move(x() + (width() - edits->width())/2, y() + (height() - edits->height())/2);
		connect(edits, &NodeEditor::returned, nodeWidget, [nodeWidget](NodeItem *it) {
			nodeWidget->addNode(it);
		});
    });
	connect(SharedInstances::instance()->relayer(), &Relayer::reqEditNode, this, [this](NodeItem *it){
		NodeEditor *edits = new NodeEditor(it);
		centerWidget(this, edits);
	});
	connect(SharedInstances::instance()->relayer(), &Relayer::reqSave, this, [this, nodeWidget, bar]() {
		QUrl file = QFileDialog::getSaveFileUrl(this, tr("Save a source"), QUrl("computer://"), tr("Node source (*.node);;Tout type (* *.*)"));
        if (!file.isEmpty()) {
			auto d = new NodesSaver(bar->author(), file.toString(), qobject_cast<NodeScene *>(nodeWidget->scene())->view);
			delete d;
        }
    });
	connect(SharedInstances::instance()->relayer(), &Relayer::reqLoad, this, [this, nodeWidget, bar]() {
		QUrl file = QFileDialog::getOpenFileUrl(this, tr("Open a source"), QUrl("computer://"), tr("Node source (*.node);;Tout type (* *.*)"));
        if (!file.isEmpty()) {
			NodesLoader *opener = new NodesLoader(file.toString(), qobject_cast<NodeScene *>(nodeWidget->scene()));
            LoadingDialog *dial = new LoadingDialog(opener);
			connect(opener, &NodesLoader::nameFound, bar, &TopBar::setAuthor);
            dial->run();
			dial->deleteLater();
        }
    });
}

NodeWindow::~NodeWindow()
{
}
