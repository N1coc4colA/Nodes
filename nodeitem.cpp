#include "nodeitem.h"

#include "helpers.h"
#include "portitem.h"
#include "connectionitem.h"

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
}

int NodeItem::UID()
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
}

PortItem *NodeItem::addPorts(QString name, bool isOutput, int flags, void *ptr)
{
    PortItem *port = new PortItem(this);
    port->setIsOutput(isOutput);
    port->setNode(this);
    port->setName(name);
    port->setPortFlags(flags);
    port->setPtr(ptr);
    //scene()->addItem(port);

    /*if (scene() != nullptr) {
        scene()->addItem(port);
    }*/
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
        list[0]->remove();
        i++;
    }

    scene()->removeItem(this);
    this->~NodeItem();
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (isSelected()) {
        painter->setPen(QPen(QPalette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight), 2));
        painter->setBrush(m_nodeColor);
    } else {
        painter->setPen(m_nodeColor.lighter());
        painter->setBrush(m_nodeColor);
    }

    painter->drawPath(path());
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    painter->drawPath(m_title_path);
    painter->drawPath(m_type_path);
    painter->drawPath(m_misc_path);

    /*int i = 0;
    while (i<m_ports.length()) {
        m_ports[i]->paint(painter, option, widget);
        i++;
    }*/
}

void NodeItem::build()
{
    m_title_path = QPainterPath();
    m_type_path = QPainterPath();
    m_misc_path = QPainterPath();

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
    int tiW = tiM.width(m_title), tyW = tyM.width("(" + m_type + ")");
    int totW = ((tiW < tyW) ? tyW : tiW), totH = ((tiH < tyH) ? tyH : tiH);
    int poH = poM.height();

    //Add height for each port
    int i = 0;
    while (i<m_ports.length()) {
        int w = poM.width(m_ports[i]->name());

        if (w > totW) {
            totW = w;
        }
        totH += poH;
        i++;
    }

    totH+=poH;

    totH += m_vertMargin;
    totW += m_horMargin;

    p.addRoundRect(-totW/2, -totH/2, totW, totH, 5, 5);

    internal_width = totW;
    internal_height = totH;

    m_title_path.addText(-tiW/2, (-totH/2) + tiH, titleFont, m_title);
    m_type_path.addText(-tyW/2, (-totH/2)+tiH+tyH, typeFont, "(" + m_type + ")");

    int y = (-totH/2)+tiH+tyH+poH;
    i = 0;
    while (i<m_ports.length()) {
        if (m_ports[i]->isOutput()) {
            m_ports[i]->setPos(totW/2+10, y);
        } else {
            m_ports[i]->setPos(-totW/2+10, y);
        }
        y += poH;
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
    }
}

#include <iostream>

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
