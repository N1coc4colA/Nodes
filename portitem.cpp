#include "portitem.h"

#include "helpers.h"
#include "connectionitem.h"
#include "nodeitem.h"

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsWidget>

#include <iostream>

PortItem::PortItem(QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
    metrics(font)
{
    margin = 2;
    radius = 5;

    setFlag(GraphicsItemFlag::ItemSendsScenePositionChanges);

    portTextHeight = metrics.height();

    m_uid = UIDC::uid();
}

int PortItem::UID()
{
    return m_uid;
}

bool PortItem::isConnected()
{
    return (m_connection != nullptr);
}

bool PortItem::canConnectTo(PortItem *port)
{
    return ((port != nullptr) || (port->node() != m_node) || (m_isOutput != port->isOutput()));
}

bool PortItem::isOutput()
{
    return m_isOutput;
}

QString PortItem::name()
{
    return m_name;
}

NodeItem *PortItem::node()
{
    return m_node;
}

void PortItem::setPortFlags(int flags)
{
    m_flags = flags;
}

void PortItem::setName(QString name)
{
    m_name = name;
    portTextWidth = metrics.width(name);
    updateSize();
}

void PortItem::makeUpdateOnPaint()
{
    needsSizeUpdate = true;
}

void PortItem::updateSize()
{
    QPainterPath m_path;

    if (m_isOutput) {
        int x;
        if (m_node != nullptr) {
            x = - m_node->getWidth()/2 - radius - margin*2 - portTextWidth;
            if (needsSizeUpdate) {
                x += m_node->getWidth()/2 - 20;
            }
        } else {
            x = - radius*2 - margin*2 - portTextWidth;
        }
        int y = portTextHeight/4;

        int px;
        if (needsSizeUpdate) {
            px = -m_node->getWidth()/2 + 60;
        } else {
            px = -m_node->getWidth()/2-radius-18;
        }

        if (needsSizeUpdate) {
            x -= 30;
        }

        m_path.addEllipse(px, -radius, 2*radius, 2*radius);
        textPath.addText(x-20, y, font, m_name);
    } else {
        int x = radius + margin;
        int y = portTextHeight/4;

        m_path.addEllipse(-radius-margin, -radius, 2*radius, 2*radius);
        textPath.addText(x, y, font, m_name);
    }
    setPath(m_path);

    if (parentItem() != nullptr) {
        parentItem()->update();
    }

    update();
}

void PortItem::setNode(NodeItem *item)
{
    m_node = item;
}

void PortItem::setIsOutput(bool val)
{
    m_isOutput = val;

    disablePainting = true;
    update();
    update();
}

void PortItem::setPtr(void *ptr)
{
    m_ptr = ptr;
}

void PortItem::clearConnection()
{
    if (m_connection != nullptr)
        m_connection->remove();
}

void PortItem::setConnection(ConnectionItem *item)
{
    m_connection = item;
}

ConnectionItem *PortItem::connection()
{
    return m_connection;
}

void PortItem::remove()
{
    clearConnection();
    m_node->removePort(this);
    //scene()->removeItem(this);
    //this->~PortItem();
}

QVariant PortItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == GraphicsItemChange::ItemScenePositionHasChanged) {
        if (m_connection != nullptr) {
            m_connection->updateStartEndPos();
        }
    }
    return value;
}

void PortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (needsSizeUpdate) {
        updateSize();
        needsSizeUpdate = false;
        std::cout << "PX: " << m_node->x() + m_node->width() << ", CX: " << this->x() + textPath.boundingRect().width() + path().boundingRect().width()  << std::endl;
    }

    painter->setPen(QPen(1));
    painter->setBrush(Qt::green);
    painter->drawPath(path());

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::green);
    painter->drawPath(textPath);
}
