#ifndef NODESDISCOVER_H
#define NODESDISCOVER_H

#include <QObject>
#include <QGraphicsView>

class NodesDiscover : public QObject
{
    Q_OBJECT
public:
    explicit NodesDiscover(QString authorName, QString fp, QGraphicsView *parent = nullptr);
};

#endif // NODESDISCOVER_H
