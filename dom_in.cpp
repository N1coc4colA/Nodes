#include "dom.h"


QDataStream &operator>>(QDataStream &in, ColorData &c)
{
	return in >> c.r >> c.g >> c.b >> c.a;
}

QDataStream &operator>>(QDataStream &in, TypeData &t)
{
	in >> t.ID >> t.color >> t.name;
	deserializeList(in, t.inherits);
	return in;
}

QDataStream &operator>>(QDataStream &in, ConnectionData &c)
{
	return in >> c.ID >> c.sourceID >> c.targetID;
}

QDataStream &operator>>(QDataStream &in, PortData &p)
{
	in >> p.ID >> p.isOutput >> p.type >> p.name;
	return in;
}

QDataStream &operator>>(QDataStream &in, NodeData &n)
{
	in >> n.ID >> n.xpos >> n.ypos >> n.name >> n.type;
	deserializeList(in, n.ports);
	return in;
}

QDataStream &operator>>(QDataStream &in, DateData &d)
{
	return in >> d.year >> d.month >> d.day;
}

QDataStream &operator>>(QDataStream &in, AdditionnalData &a)
{
	return in >> a.authorName >> a.date >> a.has_types >> a.has_nodes >> a.has_ports >> a.has_conns;
}

QDataStream &operator>>(QDataStream &in, APIVersionData &v)
{
	return in >> v.alpha >> v.beta >> v.gamma;
}

QDataStream &operator>>(QDataStream &in, NodeDOM &dom)
{
	in >> dom.versioning >> dom.additional;
	if (dom.additional.has_types) {
		deserializeList(in, dom.types);
	}
	if (dom.additional.has_nodes) {
		deserializeList(in, dom.nodes);
	}
	if (dom.additional.has_ports) {
		deserializeList(in, dom.ports);
	}
	if (dom.additional.has_conns) {
		deserializeList(in, dom.conns);
	}

	in >> dom.sum;
	dom.validity = dom.summup() == dom.sum;

	return in;
}
