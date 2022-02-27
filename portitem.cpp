#include "portitem.h"

#include "helpers.h"
#include "connectionitem.h"
#include "nodeitem.h"
#include "nodepalette.h"
#include "relayer.h"
#include "sharedinstances.h"

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsWidget>


PortItem::PortItem(QGraphicsItem *parent) :
    QGraphicsPathItem(parent),
	t(nullptr),
	metrics(font)
{
    margin = 2;
    radius = 5;
    portTextHeight = metrics.height();
    m_uid = UIDC::uid();
	setFlag(GraphicsItemFlag::ItemSendsScenePositionChanges);

	QObject::connect(SharedInstances::instance()->relayer(), &Relayer::nodePaletteChanged, SharedInstances::instance()->relayer(), [this]() {
		update();
	});
}

qint64 PortItem::UID()
{
    return m_uid;
}

bool PortItem::isConnected()
{
    return (m_connection != nullptr);
}

bool PortItem::canConnectTo(PortItem *port)
{
	return ((port != nullptr) && (port->node() != m_node) && (m_isOutput != port->isOutput()));
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
	portTextWidth = metrics.horizontalAdvance(name);
	portTextHeight = metrics.height();
    updateSize();
}

void PortItem::makeUpdateOnPaint()
{
    needsSizeUpdate = true;
}

QRectF PortItem::boundingRect() const
{
	return QRectF(0, 0, portTextWidth + 10 + radius*2, portTextHeight);
}

void PortItem::updateSize()
{
	textPath.clear();
	dotPath.clear();

	qreal y = radius*2;

	if (m_isOutput) {
		textPath.addText(0, y, font, m_name);
		dotPath.addEllipse(portTextWidth + 10, 0, 2*radius, 2*radius);
    } else {
		dotPath.addEllipse(0, 0, 2*radius, 2*radius);
		textPath.addText(radius +10, y, font, m_name);
	}

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

void PortItem::setLnType(LnTypeHolder &t)
{
	this->t = t;
}

LnTypeHolder &PortItem::lnType()
{
	return t;
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

void PortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (needsSizeUpdate) {
        updateSize();
		needsSizeUpdate = false;
	}

    painter->setPen(Qt::NoPen);
	painter->setBrush(m_isOutput ? SharedInstances::instance()->nodePalette()->getRight() : SharedInstances::instance()->nodePalette()->getLeft());
    painter->drawPath(textPath);

	painter->setPen(Qt::NoPen);
	painter->fillPath(dotPath, t.type->color);
}

QPointF PortItem::dotPos()
{
	if (m_isOutput) {
		return QPointF(portTextWidth + 10 + radius + scenePos().x(), radius + scenePos().y());
	}
	return QPointF(radius + scenePos().x(), radius + scenePos().y());
}

bool PortItem::pointIn(QPointF p)
{
	qreal x = p.x(), y = p.y(),
		px = scenePos().x(), py = scenePos().y(),
		bx = px + portTextWidth + 10 + radius*2, by = py + portTextHeight;
	return (px <= x && py <= y && bx >= x && by >= y);
}
