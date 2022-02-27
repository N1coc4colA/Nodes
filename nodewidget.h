#ifndef NODEWIDGET_H
#define NODEWIDGET_H

#include <QWidget>
#include <QGraphicsScene>

class NodeItem;

class NodeScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit NodeScene(QObject *parent = nullptr);

    QGraphicsView *view;

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *e) override;
    void dropEvent(QGraphicsSceneDragDropEvent *e) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *e) override;
};

class View;
class NodeConnector;

class NodeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NodeWidget(QWidget *parent = nullptr);

	void addNode(NodeItem *);
	void createNode(QString name);
    QGraphicsScene *scene();

public Q_SLOTS:
    void requestScaling(double scaling);

private:
	NodeConnector *m_nodeEditor;
    NodeScene *m_nodeScene;
    View *m_view;
};

#endif // NODEWIDGET_H
