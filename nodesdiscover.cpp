#include "nodesdiscover.h"

#include "portitem.h"
#include "nodeitem.h"
#include "connectionitem.h"
#include "dom.h"
#include "sharedinstances.h"

#include <fstream>

#include <QMessageBox>
#include <QDateTime>
#include <QDate>

NodesDiscover::NodesDiscover(QString authorName, QString fp, QGraphicsView *parent) : QObject(parent)
{
    std::ofstream f;
    f.open(fp.remove("file:/").toLocal8Bit(), std::ios::trunc | std::ios::out | std::ios::binary);

	if (f.is_open()) {
        QList<PortStruct> ports_list;
        QList<ConnectionStruct> connections_list;
        QList<NodeStruct> nodes_list;
		QList<TypeStruct> types_list;

        //Process all items that are in the scene
		QList<QGraphicsItem *> list = parent->items();
		int i = 0;
        while (i<list.length()) {
            if (PortItem *p = dynamic_cast<PortItem *>(list[i])) {
				PortStruct st;
				st.from(p);
                ports_list << st;
            } else if (ConnectionItem *p = dynamic_cast<ConnectionItem *>(list[i])) {
				connections_list << ConnectionStruct {p->UID(), p->endPort()->UID(), p->startPort()->UID()};
            } else if (NodeItem *p = dynamic_cast<NodeItem *>(list[i])) {
                NodeStruct st;

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

		//Process all existing types.
		i = 0;
		QList<LnTypeHolder> types_holders = SharedInstances::instance()->typesHolder()->types;
		while (i < types_holders.length()) {
			TypeStruct s;
			s.from(types_holders[i]);
			types_list << s;
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

		size_t offset = 0;

        //Tell the version we use
		APIVersion versioning {VERSION_ALPHA, VERSION_BETA, VERSION_GAMMA};
		versioning.write(f, offset);

		//Put the author's name and date
		AdditionnalDataStruct additional;
		additional.authorName = authorName;
		additional.date.from(QDate::currentDate());
		additional.has_types = !SharedInstances::instance()->typesHolder()->types.isEmpty();
		additional.has_nodes = !nodes_list.isEmpty();
		additional.has_ports = !ports_list.isEmpty();
		additional.has_conns = !connections_list.isEmpty();

		additional.write(f, offset);

		//Put the types table.
		if (!types_list.isEmpty()) {
			writeTypes(types_list, f, offset);
		}

        //Item count is used to check possible file corruption.
        //It would be sad to miss a node and have connections and ports in a ghosty node...
        int itemsCount = ports_list.count() + connections_list.count() + nodes_list.count();
		f.write((char*)&itemsCount, sizeof(itemsCount));
		offset += sizeof(itemsCount);

		if (!nodes_list.isEmpty()) {
			//Write the nodes
			writeNodes(nodes_list, f, offset);
		}

		if (!ports_list.isEmpty()) {
			//Then write ports
			writePorts(ports_list, f, offset);
		}

		if (!connections_list.isEmpty()) {
			//Here are the conns
			writeConnections(connections_list, f, offset);
		}

		//Put the sum
		writeOffset(f, offset);

        //Ensure all is written on the file and close that
        f.flush();
        f.close();
    } else {
        QMessageBox box(QMessageBox::Icon::Warning, tr("File error"), tr("Unable to write on the file %1").arg(fp), QMessageBox::StandardButton::Ok, parent);
        box.exec();
    }
}
