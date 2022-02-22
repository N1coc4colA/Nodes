#include "nodewidget.h"

#include "nodeconnector.h"
#include "view.h"
#include "nodeitem.h"

#include "relayer.h"

#include <QVBoxLayout>
#include <QCoreApplication>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneDragDropEvent>

NodeItem *createInput()
{
    NodeItem *i = new NodeItem;
    i->setTitle("A");
    i->setTextType("input");
    i->addPorts("output", true);
    i->build();
    return i;
}

NodeItem *createOutput()
{
    NodeItem *i = new NodeItem;
    i->setTitle("A");
    i->setTextType("output");
    i->addPorts("input");
    i->build();
    return i;
}

NodeItem *createAnd()
{
    NodeItem *i = new NodeItem;
    i->setTitle("AND");
    i->setTextType("built-in");
    i->addPorts("input A");
    i->addPorts("input B");
    i->addPorts("output", true);
    i->build();
    return i;
}

NodeItem *createNot()
{
    NodeItem *i = new NodeItem;
    i->setTitle("NOT");
    i->setTextType("built-in");
    i->addPorts("input");
    i->addPorts("output", true);
    i->build();
    return i;
}

NodeItem *createNor()
{
    NodeItem *i = new NodeItem;
    i->setTitle("NOR");
    i->setTextType("built-in");
    i->addPorts("input");
    i->addPorts("output", true);
    i->build();
    return i;
}


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
	connect(Relayer::instance(), &Relayer::reqSetScaling, this, &NodeWidget::requestScaling);
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
    NodeItem *node;
    if (name == "Input") {
        node = createInput();
    } else if (name == "Output") {
        node = createOutput();
    } else if (name == "Nor") {
        node = createNor();
    } else if (name == "Not") {
        node = createNot();
    } else if (name == "And") {
        node = createAnd();
    } else {
        node = new NodeItem;
    }

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
