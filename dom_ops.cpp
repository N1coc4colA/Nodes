#include "dom.h"

#include "linkstype.h"
#include "portitem.h"
#include "nodeitem.h"
#include "sharedinstances.h"

#include <QDate>

#include <iostream>

void StringsHolder::add(char *trg)
{
	if (!strings.contains(trg)) {
		strings << trg;
	}
}

void StringsHolder::release(char *trg)
{
	if (strings.contains(trg)) {
		strings.removeOne(trg);
		delete [] trg;
	}
}

void StringsHolder::release()
{
	for (auto v : strings) {
		delete [] v;
	}
	strings.clear();
}

QColor ColorStruct::convert()
{
	return QColor(r, g, b, a);
}

void ColorStruct::from(QColor c)
{
	r = c.red();
	g = c.green();
	b = c.blue();
	a = c.alpha();
}

TypeStruct::TypeStruct() {name = ""; inherits = {}; color = {0, 0, 0, 0}; ID = -1;}

TypeStruct::TypeStruct(int id, ColorStruct c, QString n, QList<int> inh)
	: ID(id), color(c), name(n), inherits(inh)
{}

TypeStruct::TypeStruct(const TypeStruct &src)
{
	if (&src != this) {
		ID = src.ID;
		color = src.color;
		inherits = src.inherits;
	}
}

void TypeStruct::addConvertion()
{
	SharedInstances::instance()->typesHolder()->addType(QString::fromStdString(name.toStdString()),
														inherits,
														color.convert());
}

void TypeStruct::from(LnTypeHolder holder)
{
	if (holder.valid) {
		inherits = holder.type->inherits;
		color.from(holder.type->color);
		ID = holder.type->uid;
		name = holder.type->name;
	}
}

TypeStruct &TypeStruct::operator=(const TypeStruct &src)
{
	ID = src.ID;
	color = src.color;
	name = src.name;
	inherits = src.inherits;
	return *this;
}

void PortStruct::from(PortItem *p)
{
	name = p->name();
	ID = p->UID();
	isOutput = p->isOutput();
}

void NodeStruct::from(NodeItem *p)
{
	ID = p->UID();
	name = p->title();
	type = p->textType();
	xpos = p->scenePos().x();
	ypos = p->scenePos().y();
}

void DateStruct::from(QDate date)
{
	day = date.day();
	month = date.month();
	year = date.year();
}
