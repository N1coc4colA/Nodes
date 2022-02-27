#include "dom.h"


QDataStream &operator<<(QDataStream &out, const ColorData &c)
{
	return out << c.r << c.g << c.b << c.a;
}

QDataStream &operator<<(QDataStream &out, const TypeData &t)
{
	out << t.ID << t.color << t.name;
	serializeList(out, t.inherits);
	return out;
}

QDataStream &operator<<(QDataStream &out, const ConnectionData &c)
{
	return out << c.ID << c.sourceID << c.targetID;
}

QDataStream &operator<<(QDataStream &out, const PortData &p)
{
	return out << p.ID << p.isOutput << p.type << p.name;
}

QDataStream &operator<<(QDataStream &out, const NodeData &n)
{
	out << n.ID << n.xpos << n.ypos << n.name << n.type;
	serializeList(out, n.ports);
	return out;
}

QDataStream &operator<<(QDataStream &out, const DateData &d)
{
	return out << d.year << d.month << d.day;
}

QDataStream &operator<<(QDataStream &out, const AdditionnalData &a)
{
	return out << a.authorName << a.date << a.has_types << a.has_nodes << a.has_ports << a.has_conns;
}

QDataStream &operator<<(QDataStream &out, const APIVersionData &v)
{
	return out << v.alpha << v.beta << v.gamma;
}

QDataStream &operator<<(QDataStream &out, const NodeDOM &dom)
{
	out << dom.versioning << dom.additional;
	if (dom.additional.has_types) {
		serializeList(out, dom.types);
	}
	if (dom.additional.has_nodes) {
		serializeList(out, dom.nodes);
	}
	if (dom.additional.has_ports) {
		serializeList(out, dom.ports);
	}
	if (dom.additional.has_conns) {
		serializeList(out, dom.conns);
	}
	return out << dom.sum;
}

