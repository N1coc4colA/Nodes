#include "connectionitem.h"

#include "helpers.h"
#include "portitem.h"

#include <QPen>
#include <QColor>
#include <QPalette>
#include <QPainter>
#include <QGraphicsScene>
#include <QPainterPath>


ConnectionItem::ConnectionItem(QGraphicsItem *parent) : QGraphicsPathItem(parent)
{
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
    setPen(QPen(QColor(200, 200, 200), 2));
    setBrush(Qt::NoBrush);
	setZValue(-1);
    m_uid = UIDC::uid();
}

ConnectionItem::~ConnectionItem() {}

int ConnectionItem::UID()
{
    return m_uid;
}

void ConnectionItem::remove()
{
    if (m_startPort != nullptr) {
        m_startPort->setConnection(nullptr);
        m_startPort = nullptr;
    }
    if (m_endPort != nullptr) {
        m_endPort->setConnection(nullptr);
        m_endPort = nullptr;
    }
    if (scene()) {
        if (scene()->items().contains(this)) {
            scene()->removeItem(this);
        }
    }
    this->~ConnectionItem();
}

PortItem *ConnectionItem::endPort()
{
    return m_endPort;
}

PortItem *ConnectionItem::startPort()
{
    return m_startPort;
}

QPointF ConnectionItem::startPos()
{
    return start_pos;
}

QPointF ConnectionItem::endPos()
{
    return end_pos;
}

void ConnectionItem::setHighlighted(bool highlight)
{
	m_highlighted = highlight;
	update();
}

void ConnectionItem::setWarned(bool enable)
{
	bool rd = enable != m_warned;
	m_warned = enable;
	if (rd) {
		update();
	}
}

void ConnectionItem::setEndPort(PortItem *port)
{
    m_endPort = port;
    m_endPort->setConnection(this);
}

void ConnectionItem::setStartPort(PortItem *port)
{
    m_startPort = port;
    m_startPort->setConnection(this);
}

void ConnectionItem::setEndPos(QPointF p)
{
    end_pos = p;
}

void ConnectionItem::setStartPos(QPointF p)
{
    start_pos = p;
}

QList<PortItem *> ConnectionItem::nodes()
{
    return {m_startPort, m_endPort};
}

void ConnectionItem::updateStartEndPos()
{
    if (m_startPort != nullptr && (m_startPort->isOutput() == false)) {
        PortItem *temp = m_endPort;
        m_endPort = m_startPort;
        m_startPort = temp;
    }
	if (m_startPort != nullptr) {
		start_pos = m_startPort->dotPos();
    }
    if (m_endPort != nullptr) {
		end_pos = m_endPort->dotPos();
    }
    updatePath();
}

void ConnectionItem::updatePath()
{
    QPainterPath m_path;
    m_path.moveTo(start_pos);
    double dx = end_pos.x() - start_pos.x();
    double dy = end_pos.y() - start_pos.y();
    QPointF ctr1(start_pos.x() + dx * 0.5, start_pos.y());
    QPointF ctr2(start_pos.x() + dx * 0.5, start_pos.y() + dy);
    m_path.cubicTo(ctr1, ctr2, end_pos);
    setPath(m_path);
}

void ConnectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
	if (m_warned) {
		painter->setPen(QPen(Qt::red, 3));
	} else if (isSelected() || m_highlighted) {
		painter->setPen(QPen(QPalette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight), 2.5));
    } else {
		if (m_startPort) {
			painter->setPen(QPen(m_startPort->lnType().type->color, 1.5));
		} else if (m_endPort) {
			painter->setPen(QPen(m_endPort->lnType().type->color, 1.5));
		} else {
			painter->setPen(QPen(QPalette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight), 1.5));
		}
    }
    painter->drawPath(path());
}
