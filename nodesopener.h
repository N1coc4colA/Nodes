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
    QMap<int, QGraphicsItem *> mapped;
    bool canRead(void *target, const char *tag);
    bool canRead(long val, const char *tag);
    char *readChar(std::ifstream &stream);
    NodeStruct readANode(std::ifstream &stream);
    PortStruct readAPort(std::ifstream &stream);
    ConnectionStruct readAConn(std::ifstream &stream);
    QList<PortStruct> readPorts(std::ifstream &stream);
    QList<ConnectionStruct> readConns(std::ifstream &stream);
    QList<NodeStruct> readNodes(std::ifstream &stream);

    void freeAllocated(QList<NodeStruct> &l);
    void freeAllocated(QList<PortStruct> &l);

    int itemsCount = 0;
    QGraphicsScene *m_parent;
    QString m_fp;
};

#endif // NODESOPENER_H
