#include "nodewindow.h"

#include "topbar.h"
#include "nodewidget.h"
#include "nodepalette.h"
#include "singlenodeedits.h"
#include "nodeitem.h"

#include "nodesdiscover.h"
#include "nodesopener.h"
#include "loadingdialog.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>

NodeWindow::NodeWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Nodes");
    QWidget *mainWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    NodeWidget *nodeWidget = new NodeWidget;
    TopBar *bar = new TopBar;

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(nodeWidget);

    mainLayout->addWidget(bar);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);

    connect(bar, &TopBar::setScaling, nodeWidget, &NodeWidget::requestScaling);
    connect(nodeWidget, &NodeWidget::scaled, bar, &TopBar::updateScaling);
    connect(bar, &TopBar::generateNode, this, [nodeWidget](){
        connect((new SingleNodeEdits()), &SingleNodeEdits::returned, nodeWidget, &NodeWidget::addNode);
    });
    connect(bar, &TopBar::runSave, this, [this, nodeWidget, bar]() {
        QUrl file = QFileDialog::getSaveFileUrl(this, tr("Enregistrer une source"), QUrl("computer://"), tr("Node source (*.node);;Tout type (* *.*)"));
        if (!file.isEmpty()) {
            new NodesDiscover(bar->author(), file.toString(), qobject_cast<NodeScene *>(nodeWidget->scene())->view);
        }
    });
    connect(bar, &TopBar::runLoad, this, [this, nodeWidget, bar]() {
        QUrl file = QFileDialog::getOpenFileUrl(this, tr("Ouvrir une source"), QUrl("computer://"), tr("Node source (*.node);;Tout type (* *.*)"));
        if (!file.isEmpty()) {
            NodesOpener *opener = new NodesOpener(file.toString(), qobject_cast<NodeScene *>(nodeWidget->scene()));
            LoadingDialog *dial = new LoadingDialog(opener);
            connect(opener, &NodesOpener::nameFound, bar, &TopBar::setAuthor);
            dial->run();
        }
    });
}
