#include "nodesopener.h"

#include "connectionitem.h"
#include "portitem.h"
#include "nodeitem.h"

#include <fstream>

#include <QGraphicsItem>
#include <QMessageBox>

char *NodesOpener::readChar(std::ifstream &stream)
{
    long size = 0;
    stream.read((char*)&size, sizeof (size));
    char *allocated = (char*)malloc((size_t)size);
    stream.read(allocated, size);
    return allocated;
}

NodeStruct NodesOpener::readANode(std::ifstream &stream)
{
    NodeStruct st;
    stream.read((char*)&st.ID, sizeof (st.ID));
    st.name = readChar(stream);
    st.type = readChar(stream);

    stream.read((char*)&st.xpos, sizeof (st.xpos));
    stream.read((char*)&st.ypos, sizeof (st.ypos));

    int i = 0;
    int len = 0;
    stream.read((char*)&len, sizeof (len));
    while (i<len) {
        int val = 0;
            stream.read((char*)&val, sizeof (val));
            st.ports << val;
        i++;
    }
    return st;
}

PortStruct NodesOpener::readAPort(std::ifstream &stream)
{
    PortStruct st;
    stream.read((char*)&st.ID, sizeof (st.ID));
    stream.read((char*)&st.isOutput, sizeof (st.isOutput));
    st.name = readChar(stream);
    return st;
}

ConnectionStruct NodesOpener::readAConn(std::ifstream &stream)
{
    ConnectionStruct st;
    stream.read((char*)&st.ID, sizeof (st.ID));
    stream.read((char*)&st.sourceID, sizeof (st.sourceID));
    stream.read((char*)&st.targetID, sizeof (st.targetID));
    return st;
}

QList<PortStruct> NodesOpener::readPorts(std::ifstream &stream)
{
    QList<PortStruct> list;
    int len = 0;
    int i = 0;
    stream.read((char*)&len, sizeof (len));

    while (i<len) {
        list << readAPort(stream);
        Q_EMIT itemLoad((i+1)/len);
        i++;
    }
    return list;
}

QList<ConnectionStruct> NodesOpener::readConns(std::ifstream &stream)
{
    QList<ConnectionStruct> list;
    int len = 0;
    int i = 0;
    stream.read((char*)&len, sizeof (len));

    while (i<len) {
        list << readAConn(stream);
        Q_EMIT itemLoad((i+1)/len);
        i++;
    }
    return list;
}

QList<NodeStruct> NodesOpener::readNodes(std::ifstream &stream)
{
    QList<NodeStruct> list;
    int len = 0;
    int i = 0;
    stream.read((char*)&len, sizeof (len));

    while (i<len) {
        list << readANode(stream);
        Q_EMIT itemLoad((i+1)/len);
        i++;
    }
    return list;
}

NodesOpener::NodesOpener(QString fp, QGraphicsScene *parent) : QObject(parent)
{
    m_fp = fp;
    m_parent = parent;
}

#include <iostream>

void NodesOpener::load()
{
    std::ifstream f;
    f.open(m_fp.remove("file://").toLocal8Bit(), std::ios::in | std::ios::binary);

    if (f.is_open()) {

        //Check API version to know if it is compatible
        APIVersion versioning;
        int step;
        long size = 0;
        f.read((char*)&versioning, sizeof (versioning));

        if (versioning.alpha == VERSION_ALPHA && versioning.beta == VERSION_BETA && versioning.omega == VERSION_OMEGA) {
            QList<ConnectionStruct> connections_list;
            QList<PortStruct> ports_list;
            QList<NodeStruct> nodes_list;

            //Get author and number of items to detect corruptions
            f.read((char*)&step, sizeof (step));
            Q_EMIT stepFound(step);
            Q_EMIT itemLoad(0);

            f.read((char*)&size, sizeof (size));
            char *author = (char *)malloc((size_t)size);
            f.read(author, size);
            Q_EMIT nameFound(author);
            free(author);

            int itemsCount = 0;
            f.read((char*)&itemsCount, sizeof (itemsCount));
            Q_EMIT itemLoad(100);

            //Get the nodes
            f.read((char*)&step, sizeof (step));
            Q_EMIT stepFound(step);
            Q_EMIT itemLoad(0);
            nodes_list = readNodes(f);
            Q_EMIT itemLoad(100);

            //Get the ports
            f.read((char*)&step, sizeof (step));
            Q_EMIT stepFound(step);
            Q_EMIT itemLoad(0);
            ports_list = readPorts(f);
            Q_EMIT itemLoad(100);

            //Get the connections
            f.read((char*)&step, sizeof (step));
            Q_EMIT stepFound(step);
            Q_EMIT itemLoad(0);
            connections_list = readConns(f);
            Q_EMIT itemLoad(100);

            if (itemsCount == (nodes_list.count() + connections_list.count() + ports_list.count())) {
                m_parent->clear();

                int i = 0;
                Q_EMIT itemLoad(0);

                i = 0;
                Q_EMIT itemLoad(100);

                //Create and setup the nodes
                Q_EMIT stepFound(step+3);
                while (i<nodes_list.length()) {
                    Q_EMIT itemLoad((i+1)/nodes_list.length());
                    NodeItem *item = new NodeItem;
                    NodeStruct src = nodes_list[i];

                    //m_parent->addItem(item);

                    item->setTitle(src.name);
                    item->setTextType(src.type);
                    item->setPos(src.xpos, src.ypos);

                    int j = 0;
                    //Create the needed ports
                    while (j<src.ports.length()) {
                        /*item->appendPort(dynamic_cast<PortItem *>(mapped[src.ports[j]]));
                        dynamic_cast<PortItem *>(mapped[src.ports[j]])->setNode(item);*/

                        //In the case we build it twice!
                        int ID = src.ports[j];
                        if (mapped[ID] == nullptr) {
                            int k = 0;
                            bool found = false;
                            PortStruct st;
                            while ((found == false) && (k<ports_list.length())) {
                                if (ports_list[k].ID == ID) {
                                    st = ports_list[k];
                                    found = true;
                                }
                                k++;
                            }

                            if (found) {
                                PortItem *prt = item->addPorts(st.name, st.isOutput);
                                prt->makeUpdateOnPaint();
                                prt->setName(ports_list[i].name);
                                mapped[ID] = prt;
                            }
                        }

                        j++;
                    }

                    item->build();
                    m_parent->addItem(item);
                    mapped[src.ID] = item;
                    i++;
                }

                Q_EMIT itemLoad(100);
                i = 0;

                //Create and setup the connections
                Q_EMIT stepFound(step+2);
                while (i<connections_list.length()) {
                    Q_EMIT itemLoad((i+1)/connections_list.length());
                    ConnectionItem *item = new ConnectionItem;
                    ConnectionStruct src = connections_list[i];
                    PortItem *tar = dynamic_cast<PortItem *>(mapped[src.targetID]);
                    item->setEndPort(tar);
                    PortItem *srt = dynamic_cast<PortItem *>(mapped[src.sourceID]);
                    item->setStartPort(srt);
                    tar->setConnection(item);
                    srt->setConnection(item);

                    item->setStartPos(srt->scenePos());
                    item->setEndPos(tar->scenePos());
                    m_parent->addItem(item);

                    mapped[src.ID] = item;
                    i++;
                }

                i = 0;
                Q_EMIT itemLoad(100);

                //We edit a second time the connections. The first time their nodes wasn't created.
                Q_EMIT stepFound(step+4);
                while (i<connections_list.length()) {
                    Q_EMIT itemLoad((i+1)/connections_list.length());
                    ConnectionItem *con = dynamic_cast<ConnectionItem *>(mapped[connections_list[i].ID]);
                    con->updateStartEndPos();
                    con->setZValue(-1);
                    i++;
                }

                i = 0;
                Q_EMIT itemLoad(100);

                //We must set if it is output after we created the parent node!
                Q_EMIT stepFound(step+4);

                //Free allocated data for the char* members
                freeAllocated(nodes_list);
                freeAllocated(ports_list);
                Q_EMIT itemLoad(100);

                Q_EMIT endedLoad(false);
            } else {
                Q_EMIT endedLoad(true);
            }
        }
    } else {
        QMessageBox box(QMessageBox::Icon::Warning, tr("File error"), tr("Unable to read the file %1").arg(m_fp), QMessageBox::StandardButton::Ok);
        box.exec();
    }
}

void NodesOpener::freeAllocated(QList<NodeStruct> &l)
{
    int i = 0;
    while (i<l.length()) {
        free(l[i].name);
        free(l[i].type);
        i++;
    }
}

void NodesOpener::freeAllocated(QList<PortStruct> &l)
{
    int i = 0;
    while (i<l.length()) {
        free(l[i].name);
        i++;
    }
}

QString NodesOpener::fp()
{
    return m_fp;
}
