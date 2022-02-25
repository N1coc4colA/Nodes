#ifndef NODESOPENER_H
#define NODESOPENER_H

#include "dom.h"

#include <QMap>
#include <QGraphicsScene>

class NodesOpener : public QObject
{
    Q_OBJECT
public:
    explicit NodesOpener(QString fp, QGraphicsScene *parent);
    void load();
    QString fp();

Q_SIGNALS:
    void nameFound(QString name);
    void stepFound(int step);
    void itemLoad(double percent);
    void endedLoad(bool failed);

private:
    QGraphicsScene *m_parent;
    QString m_fp;
};

#endif // NODESOPENER_H
