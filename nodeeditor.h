#ifndef NODEEDITOR_H
#define NODEEDITOR_H

#include <QGraphicsScene>

class ConnectionItem;
class PortItem;
class NodeItem;

class NodeEditor : public QObject
{
    Q_OBJECT
public:
    explicit NodeEditor(QObject *parent = nullptr);

    void install(QGraphicsScene *scene);
    QGraphicsItem *itemAt(QPointF p);

    bool eventFilter(QObject *watched, QEvent *event);
private:
    QGraphicsScene *m_scene = nullptr;
    PortItem *m_port = nullptr;
    ConnectionItem *m_connection = nullptr;
    NodeItem *m_lastSelected = nullptr;
};

#endif // NODEEDITOR_H
