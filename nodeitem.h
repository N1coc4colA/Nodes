#ifndef NODEITEM_H
#define NODEITEM_H

#include "linkstype.h"

#include <QGraphicsItem>

class PortItem;
class QContextMenuEvent;

class NodeItem : public QGraphicsPathItem
{
public:
    explicit NodeItem(QGraphicsItem *parent = nullptr);

    int width();
    QString title();
    void setTitle(QString t);
    QString textType();
    void setTextType(QString t);

    void build();
	PortItem *addPort(QString name, bool isOutput = false, LnTypeHolder t = LnTypeHolder(nullptr), int flags = 0, void *ptr = nullptr);
    void selectConnections(bool value);
    void remove();
    void removePort(PortItem *i);
    void appendPort(PortItem *it);
	PortItem *getPortAt(QPointF pos);

    int getWidth();
    int getHeight();

	qint64 UID();
    QList<PortItem *> ports();

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

	enum { Type = UserType + 2 };
	inline int type() const override
	{
		return Type;
	}

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    QString m_title = "Title";
    QString m_type = "Base";

    int m_width = 30;
    int m_height = 30;
    int m_horMargin = 30;
    int m_vertMargin = 15;

	QList<PortItem *> m_ports;

    QPainterPath m_type_path;
    QPainterPath m_title_path;

	qint64 m_uid = 0;

    int internal_width = 0;
	int internal_height = 0;
};

#endif // NODEITEM_H
