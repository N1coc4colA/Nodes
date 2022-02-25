#include "nodesopener.h"

#include "connectionitem.h"
#include "portitem.h"
#include "nodeitem.h"
#include "sharedinstances.h"

#include <fstream>
#include <iostream>

#include <QGraphicsItem>
#include <QMessageBox>

#define NEEDS_CLEARING 2
#define CAN_KEEP 1
#define CAN_CONTINUE 0

NodesOpener::NodesOpener(QString fp, QGraphicsScene *parent) : QObject(parent)
{
    m_fp = fp;
    m_parent = parent;
}

void NodesOpener::load()
{
	std::ifstream f;
	f.open(m_fp.remove("file://").toLocal8Bit(), std::ios::in | std::ios::binary);
	int state = CAN_CONTINUE;
	QString message;

	if (f.is_open()) {
		size_t summary = 0;

        //Check API version to know if it is compatible
		APIVersion versioning;
		versioning.read(f, summary);

		if (versioning.alpha == VERSION_ALPHA && versioning.beta == VERSION_BETA && versioning.gamma == VERSION_GAMMA) {
            QList<ConnectionStruct> connections_list;
            QList<PortStruct> ports_list;
            QList<NodeStruct> nodes_list;
			QList<TypeStruct> types_list;

			std::cout << "Removing in-app previous data..." << std::endl;
			//Now we can clear everything.
			m_parent->clear();
			SharedInstances::instance()->typesHolder()->cleanup();
			SharedInstances::instance()->holder()->release();

            //Get author and number of items to detect corruptions
			AdditionnalDataStruct additional;
			if (additional.read(SharedInstances::instance()->holder(), f, summary)) {
				std::cout << "Enabled: "
				<< (additional.has_types ? "types " : "")
				<< (additional.has_ports ? "ports " : "")
				<< (additional.has_conns ? "conns " : "")
				<< (additional.has_nodes ? "nodes " : "")
				<< std::endl;

				std::cout << "Reading needed fields..." << std::endl;
				//Get the types
				if (additional.has_types && !readTypes(SharedInstances::instance()->holder(), types_list, f, summary)) {
					message = tr("Unable to read Types' file part, file:\n%1").arg(m_fp);
					state = NEEDS_CLEARING;
				}

				//Check items count
				int itemsCount = 0;
				f.read((char*)&itemsCount, sizeof(itemsCount));
				if (f.fail()) {
					message = tr("Unable to read inventory, file:\n%1").arg(m_fp);
					state = CAN_KEEP;
				}

				//Get the nodes
				if (additional.has_nodes && !readNodes(SharedInstances::instance()->holder(), nodes_list, f, summary)) {
					message = tr("Unable to read Nodes' file part, file:\n%1").arg(m_fp);
					state = NEEDS_CLEARING;
				}
				//Get the ports
				if (additional.has_ports && !readPorts(SharedInstances::instance()->holder(), ports_list, f, summary)) {
					message = tr("Unable to read Ports' file part, file:\n%1").arg(m_fp);
					state = NEEDS_CLEARING;
				}
				//Get the connections
				if (additional.has_conns && !readConnections(connections_list, f, summary)) {
					message = tr("Unable to read Connections' file part, file:\n%1").arg(m_fp);
					state = NEEDS_CLEARING;
				}

				std::cout << "Checking fields and inventory..." << std::endl;
				 if (itemsCount != (ports_list.count() + nodes_list.count() + connections_list.count())) {
					 message = tr("Inventory is invalid, file:\n%1").arg(m_fp);
					 state = CAN_KEEP;
				 }

				 std::cout << "Checking summary..." << std::endl;
				 //Checkup summary and count.
				 if (readOffset(f, summary) && state != CAN_KEEP) {
					 message = tr("Summary invalid, file parts are missing. File:\n%1").arg(m_fp);
					 state = CAN_KEEP;
				 }

				 std::cout << "Generating types..." << std::endl;
				 //Add all types first, to have a graceful creation of other elements.
				 int i = 0;
				 while (i<types_list.length()) {
					 //std::cout << "Adding type: " << types_list[i].name.toStdString() << std::endl;
					 types_list[i].addConvertion();
				 }

				 std::cout << "Building tables..." << std::endl;
				 //Build tables
				 QMap<int, QGraphicsItem *> mapped;
				 QMap<int, PortStruct> port_table;

				 for (auto p : ports_list) {
					 port_table[p.ID] = p;
				 }

				 std::cout << "Generating nodes..." << std::endl;
				 //Add all nodes
				 i = 0;
				 while (i<nodes_list.length()) {
					 NodeItem *item = new NodeItem;
					 NodeStruct src = nodes_list[i];

					 item->setTitle(src.name);
					 item->setTextType(src.type);
					 item->setPos(src.xpos, src.ypos);

					 std::cout << "Generating node's ports..." << std::endl;
					 //Create the needed ports
					 int j = 0;
					 while (j<src.ports.length()) {
						 int p = src.ports[i];
						 //[TODO] Emit a warning here if the port is not registered.
						 if (port_table.contains(p)) {
							 PortStruct s = port_table[p];
							 mapped[s.ID] = item->addPorts(s.name, s.isOutput, SharedInstances::instance()->typesHolder()->getByUID(s.type));
						 } else if (state != CAN_KEEP) {
							 message = tr("Missing data about some ports, representation might be invalid. File:\n%1").arg(m_fp);
							 state = CAN_KEEP;
						 }
						 j++;
					}

					 item->build();
					 m_parent->addItem(item);
					 i++;
				 }

				 std::cout << "Generating conncetions..." << std::endl;
				 //Build connections
				 while (i<connections_list.length()) {
					 ConnectionStruct src = connections_list[i];

					 //Because we continue even if there is an issue with the ports, do not add any on error.
					 PortItem *tar = dynamic_cast<PortItem *>(mapped[src.targetID]);
					 PortItem *srt = dynamic_cast<PortItem *>(mapped[src.sourceID]);
					 if (tar && srt) {
						 ConnectionItem *item = new ConnectionItem;

						 item->setStartPort(srt);
						 item->setEndPort(tar);
						 m_parent->addItem(item);

						 item->updateStartEndPos();
						 item->setZValue(-1);
					 }
					 i++;
				 }
			 } else {
				 message = tr("Unable to read Additional data's file part, file:\n%1").arg(m_fp);
				 state = NEEDS_CLEARING;
			 }
		 } else {
			 message = tr("File is not supported due to its version. Conversions are not supported yet. File:\n%1").arg(m_fp);
			 state = NEEDS_CLEARING;
		 }
	 } else {
		 message = tr("Unable to open the file: \n%1").arg(m_fp);
	 }

	 Q_EMIT endedLoad(state != NEEDS_CLEARING);

	 if (state != CAN_CONTINUE) {
		 QMessageBox box(QMessageBox::Icon::Warning, tr("File error"), message + tr("\nThe file have not been loaded."), QMessageBox::StandardButton::Ok);
		 box.exec();
	 }
}

QString NodesOpener::fp()
{
    return m_fp;
}
