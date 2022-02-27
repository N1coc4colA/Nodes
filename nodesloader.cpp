#include "nodesloader.h"

#include "connectionitem.h"
#include "portitem.h"
#include "nodeitem.h"
#include "sharedinstances.h"

#include <iostream>

#include <QFile>
#include <QDataStream>
#include <QGraphicsItem>
#include <QMessageBox>


NodesLoader::NodesLoader(QString fp, QGraphicsScene *parent) : QObject(parent)
{
    m_fp = fp;
    m_parent = parent;
}

void NodesLoader::load()
{
	Q_EMIT stepFound(0);
	QFile f(m_fp.remove("file:/"));
	QString message;
	Q_EMIT itemLoad(5);

	if (f.open(QIODevice::OpenModeFlag::ReadOnly)) {
		Q_EMIT itemLoad(10);
		QDataStream stream(&f);

		NodeDOM dom;
		Q_EMIT itemLoad(25);
		stream >> dom;
		Q_EMIT itemLoad(95);

		if (dom.validity) {
			Q_EMIT itemLoad(99);
			if (dom.versioning.alpha == VERSION_ALPHA && dom.versioning.beta == VERSION_BETA && dom.versioning.gamma == VERSION_GAMMA) {
				Q_EMIT itemLoad(100);
				Q_EMIT nameFound(dom.additional.authorName);

				Q_EMIT stepFound(1);
				int current = 0;
				std::cout << "Removing in-app previous data..." << std::endl;

				//Now we can clear everything.
				m_parent->clear();
				SharedInstances::instance()->typesHolder()->cleanup();

				current += 10;
				Q_EMIT itemLoad(current);

				std::cout << "Enabled fields: "
				<< (dom.additional.has_types ? "types " : "")
				<< (dom.additional.has_ports ? "ports " : "")
				<< (dom.additional.has_conns ? "conns " : "")
				<< (dom.additional.has_nodes ? "nodes " : "")
				<< std::endl;

				//Add all types first, to have a graceful creation of other elements.
				if (dom.additional.has_types) {
					std::cout << "Generating types..." << std::endl;
					int s = 1/dom.types.length()/40;
					for (auto t : dom.types) {
						current += s;
						t.addConvertion();
						Q_EMIT itemLoad(current);
					}
				} else {
					current += 40;
					Q_EMIT itemLoad(current);
				}

				//Mapping tables
				QMap<qint64, QGraphicsItem *> mapped;
				QMap<qint64, PortData> port_table;
				current += 10;
				Q_EMIT itemLoad(current);

				if (dom.additional.has_ports) {
					std::cout << "Building tables..." << std::endl;
					//Build tables
					int s = 1/dom.types.length()/40;
					for (auto p : dom.ports) {
						current += s;
						port_table[p.ID] = p;
						Q_EMIT itemLoad(current);
					}
				} else {
					current += 40;
					Q_EMIT itemLoad(current);
				}

				Q_EMIT stepFound(2);
				current = 0;
				if (dom.additional.has_nodes) {
					std::cout << "Generating nodes..." << std::endl;
					int s = 1/dom.types.length()/60;
					for (auto n : dom.nodes) {
						current += s;
						NodeItem *item = new NodeItem;
						item->setTitle(n.name);
						item->setTextType(n.type);
						item->setPos(n.xpos, n.ypos);

						if (dom.additional.has_ports) {
							std::cout << "Generating node's ports..." << std::endl;
							for (auto p : n.ports) {
								if (port_table.contains(p)) {
									PortData s = port_table[p];
									LnTypeHolder hold = SharedInstances::instance()->typesHolder()->getByUID(s.type);
									if (!hold.valid) {
										std::cout << "Port type is invalid!" << std::endl;
									}
									mapped[s.ID] = item->addPort(s.name, s.isOutput, hold);
								}
							}
						}
						item->build();
						m_parent->addItem(item);
						Q_EMIT itemLoad(current);
					}
				} else {
					current += 60;
					Q_EMIT itemLoad(current);
				}

				if (dom.additional.has_conns && dom.additional.has_ports) {
					std::cout << "Generating conncetions..." << std::endl;
					int s = 1/dom.conns.length()/60;
					for (auto c : dom.conns) {
						current += s;
						//Because we continue even if there is an issue with the ports, do not add any on error.
						PortItem *tar = dynamic_cast<PortItem *>(mapped[c.targetID]);
						PortItem *srt = dynamic_cast<PortItem *>(mapped[c.sourceID]);
						if (tar && srt) {
							ConnectionItem *item = new ConnectionItem;

							item->setStartPort(srt);
							item->setEndPort(tar);
							m_parent->addItem(item);

							item->updateStartEndPos();
							item->setZValue(-1);
						}
						Q_EMIT itemLoad(current);
					}
				} else {
					current += 40;
					Q_EMIT itemLoad(current);
				}
			} else {
				message = tr("Opening cross version files is not supported yet, try with a compatible file.");
			}
		} else {
			message = tr("The file content is corrupted.");
		}
	} else {
		message = f.errorString();
	}
	Q_EMIT itemLoad(100);

	if (!message.isEmpty()) {
		Q_EMIT nameFound("");

		QMessageBox box(QMessageBox::Icon::Warning, tr("File error"), message + tr("\nThe file has not been loaded."), QMessageBox::StandardButton::Ok);
		box.exec();

		Q_EMIT endedLoad(true);
	} else {
		Q_EMIT endedLoad(false);
	}
}

QString NodesLoader::fp()
{
    return m_fp;
}
