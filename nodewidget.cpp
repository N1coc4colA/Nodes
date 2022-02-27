#include "nodewidget.h"

#include "nodeconnector.h"
#include "view.h"
#include "nodeitem.h"
#include "sharedinstances.h"
#include "relayer.h"

#include <QVBoxLayout>
#include <QCoreApplication>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneDragDropEvent>

NodeWidget::NodeWidget(QWidget *p) : QWidget(p)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

	m_nodeEditor = new NodeConnector(this);
    m_nodeScene = new NodeScene(this);
    m_view = new View(this);
    m_nodeScene->view = m_view;
    m_view->setScene(m_nodeScene);
    m_nodeEditor->install(m_nodeScene);

    mainLayout->addWidget(m_view);

	connect(m_view, &View::requestNode, this, &NodeWidget::createNode);
	connect(SharedInstances::instance()->relayer(), &Relayer::reqSetScaling, this, &NodeWidget::requestScaling);
}

QGraphicsScene *NodeWidget::scene()
{
    return m_nodeScene;
}

void NodeWidget::addNode(NodeItem *node)
{
    m_nodeScene->addItem(node);
    node->setPos(m_view->mapToScene(this->rect().center()));
}

void NodeWidget::createNode(QString name)
{
	NodeItem *node = new NodeItem;
	node->setTitle(name);
	m_nodeScene->addItem(node);
	node->setPos(m_view->mapToScene(m_view->mapFromGlobal(QCursor().pos())));
}

void NodeWidget::requestScaling(double s)
{
    m_view->requestScaling(s);
}

NodeScene::NodeScene(QObject *p) : QGraphicsScene(p) {}

void NodeScene::dragEnterEvent(QGraphicsSceneDragDropEvent *e)
{
    e->acceptProposedAction();
}

void NodeScene::dropEvent(QGraphicsSceneDragDropEvent *e)
{
    QGraphicsItem *it = itemAt(e->scenePos(), view->transform());
    if (it->acceptDrops()) {
        //We guess it will automatically redistribute it...
        QGraphicsScene::dropEvent(e);
    }
}

void NodeScene::dragMoveEvent(QGraphicsSceneDragDropEvent *e)
{
    e->acceptProposedAction();
}
