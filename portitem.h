#ifndef PORTITEM_H
#define PORTITEM_H

#include "linkstype.h"

#include <QFont>
#include <QFontMetrics>
#include <QGraphicsPathItem>

class NodeItem;
class ConnectionItem;

class PortItem : public QGraphicsPathItem
{
public:
    explicit PortItem(QGraphicsItem *parent = nullptr);

    void setIsOutput(bool isOutput);
    void setName(QString name);
    void setNode(NodeItem *item);
    void setPortFlags(int flags);
    void setPtr(void *ptr);
    void setConnection(ConnectionItem *item);
	void setLnType(LnTypeHolder &t);

    void clearConnection();
    void remove();

    void updateSize();
    void makeUpdateOnPaint();

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    bool canConnectTo(PortItem *port);
    bool isConnected();

    QString name();
    bool isOutput();
    NodeItem *node();
    ConnectionItem *connection();
    int UID();
	LnTypeHolder &lnType();

	QPointF dotPos();

	QRectF boundingRect() const override;

private:
    bool needsSizeUpdate = false;
    int m_flags = 0;
    bool m_isOutput = false;
    void *m_ptr = nullptr;
    QString m_name;
    NodeItem *m_node = nullptr;
    ConnectionItem *m_connection = nullptr;
	LnTypeHolder t;

    QPainterPath textPath;
	QPainterPath dotPath;
    QFont font;
    QFontMetrics metrics;

    int margin = 2;
    int radius = 5;
	qreal portTextWidth = 0;
	qreal portTextHeight = 0;
    bool disablePainting = false;
    int m_uid = 0;
};

#endif // PORTITEM_H
