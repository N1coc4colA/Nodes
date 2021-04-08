#include "nodeeditor.h"

#include "helpers.h"
#include "connectionitem.h"
#include "portitem.h"
#include "nodeitem.h"

#include <QEvent>
#include <QKeyEvent>
#include <QWidgetItem>
#include <QMetaObject>
#include <QGraphicsSceneMouseEvent>

#include <iostream>

NodeEditor::NodeEditor(QObject *parent) : QObject(parent) {}

void NodeEditor::install(QGraphicsScene *scene)
{
    m_scene = scene;
    scene->installEventFilter(this);
}

QGraphicsItem *NodeEditor::itemAt(QPointF p)
{
    QList<QGraphicsItem *> list = m_scene->items(QRectF(p - QPointF(1, 1), QSize(3, 3)));

    if (list.isEmpty()) {
        return nullptr;
    } else {
        return list[0];
    }
}

bool NodeEditor::eventFilter(QObject *watched, QEvent *e)
{
    if (isinstance<QWidgetItem>(e)) {
        return false;
    }

    if (e->type() == QEvent::Type::GraphicsSceneMousePress) {
        QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent *>(e);
        if (event != nullptr) {
            if (event->button() == Qt::LeftButton) {
                QGraphicsItem *item = itemAt(event->scenePos());

                if (isinstance<PortItem>(item)) {
                    m_connection = new ConnectionItem;
                    m_scene->addItem(m_connection);
                    m_port = dynamic_cast<PortItem *>(item);

                    m_connection->setStartPos(item->scenePos());
                    m_connection->setEndPos(event->scenePos());
                    m_connection->updatePath();
                    return true;

                } else if (isinstance<ConnectionItem>(item)) {
                    ConnectionItem *it = dynamic_cast<ConnectionItem *>(item);
                    m_connection = new ConnectionItem;
                    m_connection->setStartPos(it->startPos());
                    m_scene->addItem(m_connection);
                    m_port = it->startPort();
                    m_connection->setEndPos(event->scenePos());
                    m_connection->updateStartEndPos();
                    return true;

                } else if (isinstance<NodeItem>(item)) {
                    if (m_lastSelected != nullptr) {
                        m_lastSelected->selectConnections(false);
                    }
                    m_lastSelected = dynamic_cast<NodeItem *>(item);
                    m_lastSelected->selectConnections(true);

                } else {
                    if (m_lastSelected != nullptr) {
                        m_lastSelected->selectConnections(false);
                    }
                    m_lastSelected = nullptr;
                }

            } else if (event->button() == Qt::RightButton) {
                //Context menu
            }
        }
    } else if (e->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent *>(e)->key() == Qt::Key_Delete) {
            Q_FOREACH(QGraphicsItem *item, m_scene->selectedItems()) {
                if (isinstance<ConnectionItem>(item)) {
                    dynamic_cast<ConnectionItem *>(item)->remove();
                 } else if (isinstance<NodeItem>(item)) {
                    dynamic_cast<NodeItem *>(item)->remove();
                }
            }
            return true;
        }

    } else if (e->type() == QEvent::GraphicsSceneMouseMove) {
        if (m_connection != nullptr) {
            m_connection->setEndPos(dynamic_cast<QGraphicsSceneMouseEvent *>(e)->scenePos());
            m_connection->updatePath();
            return true;
        }

    } else if (e->type() == QEvent::GraphicsSceneMouseRelease) {
        QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent *>(e);

        if (m_connection != nullptr && event->button() == Qt::LeftButton) {
            QGraphicsItem *item = itemAt(event->scenePos());

            //Connect to the target port
            if (isinstance<PortItem>(item)) {
                PortItem *it = dynamic_cast<PortItem *>(item);

                //Check there's no more than one connection or anything else
                if (m_port->canConnectTo(it)) {
                    if (it->isConnected()) {
                        it->connection()->remove();
                        it->setConnection(nullptr);
                    }

                    m_port->clearConnection();
                    it->clearConnection();
                    m_connection->setStartPort(m_port);
                    m_connection->setEndPort(it);
                    m_connection->updateStartEndPos();
                    m_connection = nullptr;
                } else {
                    m_connection->remove();
                    m_connection = nullptr;
                }
            }

            if (m_connection != nullptr) {
                m_connection->remove();
            }
            m_connection = nullptr;
            m_port = nullptr;
            return true;
        }
    }
    return QObject::eventFilter(watched, e);
}
