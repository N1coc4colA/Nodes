#ifndef NODECONNECTOR_H
#define NODECONNECTOR_H

#include <QGraphicsScene>

class ConnectionItem;
class PortItem;
class NodeItem;

class NodeConnector : public QObject
{
    Q_OBJECT
public:
	explicit NodeConnector(QObject *parent = nullptr);

    void install(QGraphicsScene *scene);
    QGraphicsItem *itemAt(QPointF p);

    bool eventFilter(QObject *watched, QEvent *event);

	Q_SLOT void handleNodesDeletion();

private:
    QGraphicsScene *m_scene = nullptr;
    PortItem *m_port = nullptr;
    ConnectionItem *m_connection = nullptr;
    NodeItem *m_lastSelected = nullptr;
};

#endif // NODECONNECTOR_H
