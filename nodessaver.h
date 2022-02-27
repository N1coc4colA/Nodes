#ifndef NODESSAVER_H
#define NODESSAVER_H

#include <QObject>
#include <QGraphicsView>

class NodesSaver : public QObject
{
    Q_OBJECT
public:
	explicit NodesSaver(QString authorName, QString fp, QGraphicsView *parent = nullptr);
};

#endif // NODESSAVER_H
