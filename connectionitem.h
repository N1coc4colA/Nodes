#ifndef CONNECTIONITEM_H
#define CONNECTIONITEM_H

#include "linkstype.h"
#include <QGraphicsPathItem>

class PortItem;

class ConnectionItem : public QGraphicsPathItem
{
public:
    explicit ConnectionItem(QGraphicsItem *parent = nullptr);
    ~ConnectionItem() override;

    void remove();
    PortItem *startPort();
    PortItem *endPort();
    QPointF startPos();
    QPointF endPos();
    void setEndPort(PortItem *port);
    void setStartPort(PortItem *port);
    void setStartPos(QPointF p);
    void setEndPos(QPointF p);
	void setWarned(bool enable);
    void setHighlighted(bool highlight);
    QList<PortItem *> nodes();
    void updateStartEndPos();
    void updatePath();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
	int UID();

private:
    QPointF start_pos;
	QPointF end_pos;
    PortItem *m_startPort = nullptr;
    PortItem *m_endPort = nullptr;
	int m_uid = 0;
	bool m_highlighted = false;
	bool m_warned = false;
};

#endif // CONNECTIONITEM_H
