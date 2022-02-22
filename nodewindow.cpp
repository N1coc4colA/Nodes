#include "nodewindow.h"

#include "topbar.h"
#include "nodewidget.h"
#include "nodepalette.h"
#include "nodeeditor.h"
#include "nodeitem.h"

#include "nodesdiscover.h"
#include "nodesopener.h"
#include "loadingdialog.h"
#include "linksviewer.h"
#include "relayer.h"

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
	vwr = new LinksViewer(this);

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
	connect(_save, &QAction::triggered, Relayer::instance(), &Relayer::runSave);
	connect(_open, &QAction::triggered, Relayer::instance(), &Relayer::runLoad);
	connect(_add, &QAction::triggered, Relayer::instance(), &Relayer::addNode);
	connect(_edit, &QAction::triggered, Relayer::instance(), &Relayer::editCurrent);
	connect(_remove, &QAction::triggered, Relayer::instance(), &Relayer::rmElement);

	connect(Relayer::instance(), &Relayer::reqEnable, this, [this]() {setEnabled(true);});
	connect(Relayer::instance(), &Relayer::reqDisable, this, [this]() {setEnabled(false);});
	connect(Relayer::instance(), &Relayer::reqAddNode, this, [nodeWidget, this](){
		NodeEditor *edits = new NodeEditor;
		edits->resize(400, 200);
		edits->move(x() + (width() - edits->width())/2, y() + (height() - edits->height())/2);
		connect(edits, &NodeEditor::returned, nodeWidget, [nodeWidget](NodeItem *it) {
			nodeWidget->addNode(it);
		});
    });
	connect(Relayer::instance(), &Relayer::reqEditNode, this, [this](NodeItem *it){
		NodeEditor *edits = new NodeEditor(it);
		centerWidget(this, edits);
	});
	connect(Relayer::instance(), &Relayer::reqSave, this, [this, nodeWidget, bar]() {
		QUrl file = QFileDialog::getSaveFileUrl(this, tr("Save a source"), QUrl("computer://"), tr("Node source (*.node);;Tout type (* *.*)"));
        if (!file.isEmpty()) {
			auto d = new NodesDiscover(bar->author(), file.toString(), qobject_cast<NodeScene *>(nodeWidget->scene())->view);
			delete d;
        }
    });
	connect(Relayer::instance(), &Relayer::reqLoad, this, [this, nodeWidget, bar]() {
		QUrl file = QFileDialog::getOpenFileUrl(this, tr("Open a source"), QUrl("computer://"), tr("Node source (*.node);;Tout type (* *.*)"));
        if (!file.isEmpty()) {
			NodesOpener *opener = new NodesOpener(file.toString(), qobject_cast<NodeScene *>(nodeWidget->scene()));
            LoadingDialog *dial = new LoadingDialog(opener);
            connect(opener, &NodesOpener::nameFound, bar, &TopBar::setAuthor);
            dial->run();
			dial->deleteLater();
        }
    });
}

NodeWindow::~NodeWindow()
{
}
