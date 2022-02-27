#include "nodeconnector.h"

#include "helpers.h"
#include "connectionitem.h"
#include "portitem.h"
#include "nodeitem.h"
#include "relayer.h"
#include "linkstype.h"
#include "sharedinstances.h"

#include <QEvent>
#include <QKeyEvent>
#include <QWidgetItem>
#include <QMetaObject>
#include <QGraphicsSceneMouseEvent>


NodeConnector::NodeConnector(QObject *parent) : QObject(parent)
{
	connect(SharedInstances::instance()->relayer(), &Relayer::reqRmElement, this, &NodeConnector::handleNodesDeletion);
}

void NodeConnector::install(QGraphicsScene *scene)
{
    m_scene = scene;
    scene->installEventFilter(this);
}

QGraphicsItem *NodeConnector::itemAt(QPointF p)
{
    QList<QGraphicsItem *> list = m_scene->items(QRectF(p - QPointF(1, 1), QSize(3, 3)));

    if (list.isEmpty()) {
        return nullptr;
    } else {
        return list[0];
    }
}

void NodeConnector::handleNodesDeletion()
{
	Q_FOREACH(QGraphicsItem *item, m_scene->selectedItems()) {
		if (isinstance<ConnectionItem>(item)) {
			dynamic_cast<ConnectionItem *>(item)->remove();
		 } else if (isinstance<NodeItem>(item)) {
			dynamic_cast<NodeItem *>(item)->remove();
		}
	}
}

bool NodeConnector::eventFilter(QObject *watched, QEvent *e)
{
    if (isinstance<QWidgetItem>(e)) {
        return false;
    }

    if (e->type() == QEvent::Type::GraphicsSceneMousePress) {
		QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent *>(e);
		if (event != nullptr) {
			if (event->button() == Qt::LeftButton) {
				QGraphicsItem *item = itemAt(event->scenePos());

				if (NodeItem *node = dynamic_cast<NodeItem *>(item)) {
					if (PortItem *p = node->getPortAt(event->scenePos())) {
						m_connection = new ConnectionItem;
						m_scene->addItem(m_connection);
						m_port = p;

						m_connection->setStartPos(m_port->dotPos());
						m_connection->setEndPos(event->scenePos());
						m_connection->updatePath();
						return true;
					} else {
						if (m_lastSelected != nullptr) {
							m_lastSelected->selectConnections(false);
						}
						m_lastSelected = node;
						m_lastSelected->selectConnections(true);
					}
				} else if (ConnectionItem *it = dynamic_cast<ConnectionItem *>(item)) {
					m_connection = new ConnectionItem;
					m_connection->setStartPos(it->startPos());
					m_scene->addItem(m_connection);
					m_port = it->startPort();
					m_connection->setEndPos(event->scenePos());
					m_connection->updateStartEndPos();
					return true;
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
	} else if (e->type() == QEvent::GraphicsSceneMouseMove) {
        if (m_connection != nullptr) {
			QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent *>(e);
			m_connection->setEndPos(event->scenePos());

			QGraphicsItem *item = itemAt(event->scenePos());

			bool warn = false;
			if (NodeItem *node = dynamic_cast<NodeItem *>(item)) {
				if (PortItem *it = node->getPortAt(event->scenePos())) {
					if (m_port == it ||
							!m_port->canConnectTo(it) ||
							!((m_port->isOutput()) ? SharedInstances::instance()->typesHolder()->doesSInheritsE(m_port->lnType().type->uid, it->lnType().type->uid)
												  : SharedInstances::instance()->typesHolder()->doesSInheritsE(it->lnType().type->uid, m_port->lnType().type->uid))) {
						warn = true;
					}
				}
			}

			m_connection->setWarned(warn);
            m_connection->updatePath();
            return true;
        }
    } else if (e->type() == QEvent::GraphicsSceneMouseRelease) {
        QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent *>(e);

        if (m_connection != nullptr && event->button() == Qt::LeftButton) {
			QGraphicsItem *item = itemAt(event->scenePos());

            //Connect to the target port
			if (NodeItem *node = dynamic_cast<NodeItem *>(item)) {
				if (PortItem *it = node->getPortAt(event->scenePos())) {
					//Check if there's no more than one connection or anything else.
					//Moreover to check if it can be connected, check the inheritance.
					if (m_port != it && m_port->canConnectTo(it) &&
							((m_port->isOutput()) ? SharedInstances::instance()->typesHolder()->doesSInheritsE(m_port->lnType().type->uid, it->lnType().type->uid)
												  : SharedInstances::instance()->typesHolder()->doesSInheritsE(it->lnType().type->uid, m_port->lnType().type->uid))) {
						if (it->isConnected()) {
							it->connection()->remove();
							it->setConnection(nullptr);
						}

						m_port->clearConnection();
						it->clearConnection();
						if (m_port->isOutput()) {
							m_connection->setStartPort(it);
							m_connection->setEndPort(m_port);
						} else {
							m_connection->setStartPort(m_port);
							m_connection->setEndPort(it);
						}
						m_connection->updateStartEndPos();
						m_connection = nullptr;
					}
				}
			}

			if (m_connection) {
				m_connection->remove();
			}
            m_connection = nullptr;
            m_port = nullptr;
            return true;
        }
    }
    return QObject::eventFilter(watched, e);
}
