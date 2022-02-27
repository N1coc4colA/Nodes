#include "nodeitem.h"

#include "helpers.h"
#include "portitem.h"
#include "connectionitem.h"
#include "nodepalette.h"
#include "relayer.h"
#include "sharedinstances.h"

#include <QMenu>
#include <QPainter>
#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>


NodeItem::NodeItem(QGraphicsItem *parent) : QGraphicsPathItem(parent)
{
    setFlag(QGraphicsPathItem::ItemIsMovable);
    setFlag(QGraphicsPathItem::ItemIsSelectable);
    m_uid = UIDC::uid();

	QObject::connect(SharedInstances::instance()->relayer(), &Relayer::nodePaletteChanged, SharedInstances::instance()->relayer(), [this]() {
		update();
	});
}

qint64 NodeItem::UID()
{
    return m_uid;
}

int NodeItem::width()
{
    return m_width;
}

QString NodeItem::title()
{
    return m_title;
}

QString NodeItem::textType()
{
    return m_type;
}

void NodeItem::setTitle(QString t)
{
    m_title = t;
}

void NodeItem::setTextType(QString t)
{
    m_type = t;
}

QList<PortItem *> NodeItem::ports()
{
    return m_ports;
}

void NodeItem::removePort(PortItem *i)
{
    m_ports.removeAll(i);
	scene()->removeItem(i);
}

PortItem *NodeItem::addPort(QString name, bool isOutput, LnTypeHolder t, int flags, void *ptr)
{
    PortItem *port = new PortItem(this);
    port->setIsOutput(isOutput);
    port->setNode(this);
    port->setName(name);
    port->setPortFlags(flags);
	port->setPtr(ptr);
	port->setLnType(t);
	port->setZValue(1);
	//port->setParentItem(this);

    m_ports << port;

    return port;
}

void NodeItem::appendPort(PortItem *it)
{
    it->setParentItem(this);
    m_ports << it;
}

void NodeItem::selectConnections(bool value)
{
    int i = 0;
    while (i<m_ports.length()) {
        if (m_ports[i]->isConnected()) {
            ConnectionItem *conn = m_ports[i]->connection();
            conn->setHighlighted(value);
            conn->updatePath();
        }
        i++;
    }
}

void NodeItem::remove()
{
    int i = 0;
    QList<ConnectionItem *> list;
    while (i<m_ports.length()) {
        if (m_ports[i]->isConnected()) {
            list << m_ports[i]->connection();
        }
        i++;
    }

    i = 0;
    int len = list.length();
    while (i<len) {
        list[i]->remove();
        i++;
    }

    scene()->removeItem(this);
    this->~NodeItem();
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (isSelected()) {
        painter->setPen(QPen(QPalette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight), 2));
		painter->setBrush(SharedInstances::instance()->nodePalette()->getInner());
    } else {
		painter->setPen(SharedInstances::instance()->nodePalette()->getOutter());
		painter->setBrush(SharedInstances::instance()->nodePalette()->getInner());
    }
    painter->drawPath(path());

    painter->setPen(Qt::NoPen);
	painter->setBrush(SharedInstances::instance()->nodePalette()->getTitle());
    painter->drawPath(m_title_path);

	painter->setBrush(SharedInstances::instance()->nodePalette()->getType());
	painter->drawPath(m_type_path);
}

void NodeItem::build()
{
    m_title_path = QPainterPath();
	m_type_path = QPainterPath();

    m_width = 0;
    m_height = 0;

    QPainterPath p;

    QFont titleFont("Lucida Sans Unicode");
    QFont typeFont = titleFont;
    QFont portFont = titleFont;

    titleFont.setPointSize(16);
    typeFont.setPointSize(8);

    QFontMetrics tyM(typeFont);
    QFontMetrics tiM(titleFont);
    QFontMetrics poM(portFont);

    int tiH = tiM.height(), tyH = tyM.height();
	int tiW = tiM.horizontalAdvance(m_title), tyW = tyM.horizontalAdvance("(" + m_type + ")");
    int totW = ((tiW < tyW) ? tyW : tiW), totH = ((tiH < tyH) ? tyH : tiH);
    int poH = poM.height();

    //Add height for each port
    int i = 0;
    while (i<m_ports.length()) {
		int w = m_ports[i]->boundingRect().width();

        if (w > totW) {
            totW = w;
        }
        totH += poH;
        i++;
    }
    totH+=poH;

    totH += m_vertMargin;
    totW += m_horMargin;
	p.addRoundedRect(0, 0, totW, totH, 5, 5, Qt::AbsoluteSize);

    internal_width = totW;
    internal_height = totH;

	m_title_path.addText((totW - tiW)/2, m_vertMargin/2 + (tiH/3*2), titleFont, m_title);
	m_type_path.addText((totW - tyW)/2, (m_vertMargin/2)+tiH+10, typeFont, "(" + m_type + ")");


	/*
	 * lA = 8 (inner) + 2 (borders) + 1 (dot):
	 * [--------]o
	 *
	 * lB = 15 (innerB) + 6 (marginB) + 2 (bordersB):
	 * [***---------------***]
	 *
	 * Align on right side:
	 * XA = lB - lA + marginB/2 + XB
	 *
	 * Align on left side:
	 * XA = marginB/2 + XB
	 *
	 */

	int y = (m_vertMargin/2)+tiH+10+tyH;
    i = 0;
    while (i<m_ports.length()) {
		if (m_ports[i]->isOutput()) {
			qreal pad = totW - m_ports[i]->boundingRect().width() - m_horMargin/2;

			if (m_ports[i]->pos().x() != pad) {
				m_ports[i]->setPos(pad, y);
			}
        } else {
			m_ports[i]->setPos(m_horMargin/2, y);
        }
		y += poH;
		m_ports[i]->update();
        i++;
    }

    m_width = totW;
	m_height = totH;

    setPath(p);
}

int NodeItem::getHeight()
{
    return internal_height;
}

int NodeItem::getWidth()
{
    return internal_width;
}

void NodeItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *del = menu.addAction(QObject::tr("Delete node"));
    QAction *edit = menu.addAction(QObject::tr("Edit node"));
    QAction *act = menu.exec(menu.mapToGlobal(QPoint(event->pos().x(), event->pos().y())));

    if (act == del) {
		SharedInstances::instance()->relayer()->rmElement();
	} else if (act == edit) {
		SharedInstances::instance()->relayer()->editNode(this);
	}
}

QVariant NodeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    QVariant var = QGraphicsPathItem::itemChange(change, value);
    if (change == QGraphicsItem::GraphicsItemChange::ItemSelectedChange) {
        if (!isSelected()) {
            setZValue(2);
        } else {
            setZValue(1);
        }
    }
    return var;
}

PortItem *NodeItem::getPortAt(QPointF pos)
{
	for (auto i : m_ports) {
		if (i->pointIn(pos)) {
			return i;
		}
	}
	return nullptr;
}
