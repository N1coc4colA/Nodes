#include "nodesdiscover.h"

#include "portitem.h"
#include "nodeitem.h"
#include "connectionitem.h"
#include "dom.h"

#include <fstream>

#include <QMessageBox>

void writeChar(char *c, std::ofstream &stream)
{
    long namesize = strlen(c) + 1;
    stream.write((char*)&namesize, sizeof (namesize));
    stream.write(c, namesize);
}

void writeANode(NodeStruct n, std::ofstream &stream)
{
    stream.write((char*)&n.ID, sizeof (n.ID));

    writeChar(n.name, stream);
    writeChar(n.type, stream);

    stream.write((char*)&n.xpos, sizeof (n.xpos));
    stream.write((char*)&n.ypos, sizeof (n.ypos));

    int i = 0;
    int len = n.ports.length();
    stream.write((char*)&len, sizeof (len));

    while (i<len) {
        stream.write((char*)&n.ports[i], sizeof (n.ports[i]));
        i++;
    }
}

void writeAPort(PortStruct n, std::ofstream &stream)
{
    stream.write((char*)&n.ID, sizeof (n.ID));
    stream.write((char*)&n.isOutput, sizeof (n.isOutput));

    writeChar(n.name, stream);
}

void writeAConn(ConnectionStruct n, std::ofstream &stream)
{
    stream.write((char*)&n.ID, sizeof (n.ID));
    stream.write((char*)&n.sourceID, sizeof (n.sourceID));
    stream.write((char*)&n.targetID, sizeof (n.targetID));
}

void writePorts(QList<PortStruct> ports, std::ofstream &stream)
{
    int len = ports.length();
    stream.write((char*)&len, sizeof (len));
    int i = 0;
    while (i<len) {
        writeAPort(ports[i], stream);
        i++;
    }
}

void writeNodes(QList<NodeStruct> nodes, std::ofstream &stream)
{
    int len = nodes.length();
    stream.write((char*)&len, sizeof (len));

    int i = 0;
    while (i<len) {
        writeANode(nodes[i], stream);
        i++;
    }
}

void writeConns(QList<ConnectionStruct> conns, std::ofstream &stream)
{
    int len = conns.length();
    stream.write((char*)&len, sizeof (len));

    int i = 0;
    while (i<len) {
        writeAConn(conns[i], stream);
        i++;
    }
}

NodesDiscover::NodesDiscover(QString authorName, QString fp, QGraphicsView *parent) : QObject(parent)
{
    std::ofstream f;
    f.open(fp.remove("file:/").toLocal8Bit(), std::ios::trunc | std::ios::out | std::ios::binary);

    if (f.is_open()) {
        int i = 0;
        QList<QGraphicsItem *> list = parent->items();
        QList<PortStruct> ports_list;
        QList<ConnectionStruct> connections_list;
        QList<NodeStruct> nodes_list;

        //Process all items that are in the scene
        while (i<list.length()) {
            if (PortItem *p = dynamic_cast<PortItem *>(list[i])) {
                PortStruct st;
                st.name = (char*)malloc(strlen(p->name().toLocal8Bit().data()) + 1);
                strcpy(st.name, p->name().toLocal8Bit().data());
                st.ID = p->UID();
                st.isOutput = p->isOutput();
                ports_list << st;
            } else if (ConnectionItem *p = dynamic_cast<ConnectionItem *>(list[i])) {
                ConnectionStruct st;
                st.ID = p->UID();
                st.sourceID = p->startPort()->UID();
                st.targetID = p->endPort()->UID();
                connections_list << st;
            } else if (NodeItem *p = dynamic_cast<NodeItem *>(list[i])) {
                NodeStruct st;
                st.ID = p->UID();
                st.name = (char*)malloc(strlen(p->title().toLocal8Bit().data()) + 1);
                strcpy(st.name, p->title().toLocal8Bit().data());
                st.type = (char*)malloc(strlen(p->textType().toLocal8Bit().data()) + 1);
                strcpy(st.type, p->textType().toLocal8Bit().data());

                st.xpos = p->scenePos().x();
                st.ypos = p->scenePos().y();

                int j = 0;
                QList<PortItem *> ports = p->ports();
                while (j<ports.length()) {
                    st.ports << ports[j]->UID();
                    j++;
                }
                nodes_list << st;
            }
            i++;
        }
        /*
         * We save data in a specific order:
         * -> At root: Our version to don't get confused between different versions
         * 0. Possible author
         * 1. The nodes
         * 2. The ports
         * 3. The connections
         *
         * Our separator is an int corresponding to the type being loaded.
         * Then a second int that is the size of the stuct being loaded
         */

        //Tell the version we use
        APIVersion versioning;
        f.write((char*)&versioning, sizeof (versioning));

        int step = 0;

        //Put the author's name
        f.write((char*)&step, sizeof (step));
        writeChar(authorName.toLocal8Bit().data(), f);

        //Item count is used to check possible file corruption.
        //It would be sad to miss a node and have connections and ports in a ghosty node...
        int itemsCount = ports_list.count() + connections_list.count() + nodes_list.count();
        f.write((char*)&itemsCount, sizeof (itemsCount));

        //Write the nodes
        step = 1;
        f.write((char*)&step, sizeof (step));
        writeNodes(nodes_list, f);

        //Then write ports
        step = 2;
        f.write((char*)&step, sizeof (step));
        writePorts(ports_list, f);

        //Here are the conns
        step = 3;
        f.write((char*)&step, sizeof (step));
        writeConns(connections_list, f);

        //Ensure all is written on the file and close that
        f.flush();
        f.close();

        //Free mallocated data
        i = 0;
        while (i<ports_list.length()) {
            free(ports_list[i].name);
            i++;
        }
        i = 0;
        while (i<nodes_list.length()) {
            free(nodes_list[i].name);
            free(nodes_list[i].type);
            i++;
        }
    } else {
        QMessageBox box(QMessageBox::Icon::Warning, tr("File error"), tr("Unable to write on the file %1").arg(fp), QMessageBox::StandardButton::Ok, parent);
        box.exec();
    }
}
