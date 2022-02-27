#include "nodessaver.h"

#include "portitem.h"
#include "nodeitem.h"
#include "connectionitem.h"
#include "dom.h"
#include "sharedinstances.h"

#include <QFile>
#include <QDataStream>
#include <QMessageBox>

NodesSaver::NodesSaver(QString authorName, QString fp, QGraphicsView *parent) : QObject(parent)
{
	QFile f(fp.remove("file:/"));

	if (f.open(QIODevice::OpenModeFlag::Truncate | QIODevice::OpenModeFlag::WriteOnly)) {
		QDataStream stream(&f);
		NodeDOM dom;

        //Process all items that are in the scene
		QList<QGraphicsItem *> list = parent->items();
		int i = 0;
        while (i<list.length()) {
            if (PortItem *p = dynamic_cast<PortItem *>(list[i])) {
				PortData st;
				st.from(p);
				dom.ports << st;
            } else if (ConnectionItem *p = dynamic_cast<ConnectionItem *>(list[i])) {
				dom.conns << ConnectionData{p->UID(), p->startPort()->UID(), p->endPort()->UID()};
            } else if (NodeItem *p = dynamic_cast<NodeItem *>(list[i])) {
				NodeData st;
				st.from(p);
				dom.nodes << st;
            }
            i++;
        }

		//Process all existing types.
		i = 0;
		QList<LnTypeHolder> types_holders = SharedInstances::instance()->typesHolder()->types;
		while (i < types_holders.length()) {
			TypeData s;
			s.from(types_holders[i]);
			dom.types << s;
			i++;
		}

        /*
         * We save data in a specific order:
         * -> At root: Our version to don't get confused between different versions
		 * 0. Possible data; author, date...
		 * 1. The types
		 * 2. The nodes
		 * 3. The ports
		 * 4. The connections
         *
		 * All follows the NodeDOM structure when performing IO OPs.
		 * All the IO funcs are in dom_*.cpp
		 * The only needed step is performing calculations and optimisations by building the DOM.
         */

		dom.build(authorName);
		stream << dom;

        //Ensure all is written on the file and close that
        f.flush();
		f.close();
    } else {
		QMessageBox box(QMessageBox::Icon::Warning, tr("File error"), f.errorString(), QMessageBox::StandardButton::Ok, parent);
        box.exec();
    }
}
