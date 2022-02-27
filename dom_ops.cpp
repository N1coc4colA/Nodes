#include "dom.h"

#include "linkstype.h"
#include "portitem.h"
#include "nodeitem.h"
#include "sharedinstances.h"

#include <QDate>


void setupMetatypes()
{
	qRegisterMetaTypeStreamOperators<ColorData>("ColorData");
	qRegisterMetaTypeStreamOperators<TypeData>("TypeData");
	qRegisterMetaTypeStreamOperators<ConnectionData>("ConnectionData");
	qRegisterMetaTypeStreamOperators<PortData>("PortData");
	qRegisterMetaTypeStreamOperators<NodeData>("NodeData");
	qRegisterMetaTypeStreamOperators<DateData>("DateData");
	qRegisterMetaTypeStreamOperators<AdditionnalData>("AddtionnalData");
	qRegisterMetaTypeStreamOperators<APIVersionData>("APIVersionData");
	qRegisterMetaTypeStreamOperators<NodeDOM>("NodeDOM");
}

QColor ColorData::convert()
{
	return QColor(r, g, b, a);
}

void ColorData::from(QColor c)
{
	r = c.red();
	g = c.green();
	b = c.blue();
	if (c.alpha() < 0) {
		a = 255;
	} else {
		a = c.alpha();
	}
}

void TypeData::addConvertion()
{
	SharedInstances::instance()->typesHolder()->addType(name, inherits, color.convert());
}

void TypeData::from(LnTypeHolder holder)
{
	if (holder.valid) {
		inherits = holder.type->inherits;
		color.from(holder.type->color);
		ID = holder.type->uid;
		name = holder.type->name;
	}
}

void PortData::from(PortItem *p)
{
	name = p->name();
	ID = p->UID();
	isOutput = p->isOutput();
}

void NodeData::from(NodeItem *p)
{
	ID = p->UID();
	name = p->title();
	type = p->textType();
	xpos = p->scenePos().x();
	ypos = p->scenePos().y();

	QList<PortItem *> pl = p->ports();
	for (auto p : pl) {
		ports << p->UID();
	}
}

void DateData::from(QDate date)
{
	day = date.day();
	month = date.month();
	year = date.year();
}

void NodeDOM::build(QString name)
{
	versioning = {VERSION_ALPHA, VERSION_BETA, VERSION_GAMMA};

	additional.authorName = name;
	additional.date.from(QDate::currentDate());
	additional.has_types = !types.isEmpty();
	additional.has_nodes = !nodes.isEmpty();
	additional.has_ports = !ports.isEmpty();
	additional.has_conns = !conns.isEmpty();
	sum = summup();
}

quint64 NodeDOM::summup()
{
	quint64 out = 0;
	out += additional.authorName.length();
	out += additional.date.year - additional.date.month - additional.date.day;
	out += types.length()+nodes.length()+ports.length()+conns.length();
	out *= (((additional.has_types+additional.has_nodes+additional.has_ports+additional.has_conns)%2)
			? -1
			: 1);
	return out;
}
