#include "linkstype.h"

#include "relayer.h"

#include <algorithm>
#include <iostream>

template<typename t>
void l_merge(QList<t> &first, const QList<t> &second)
{
	for (auto s : second) {
		if (!first.contains(s)) {
			first << s;
		}
	}
}

LinkType::LinkType(QObject *p) : QObject(p) {}

LinkType::~LinkType()
{
	Q_EMIT destroyed();
}

void LinkType::notifyEdit()
{
	Q_EMIT edited();
}

LnTypeHolder::LnTypeHolder(LinkType *t, QObject *p)
	: QObject(p), type(t)
{
	valid = type != nullptr;
	setupValidity();
}

LnTypeHolder::LnTypeHolder(const LnTypeHolder &holder)
	: QObject(holder.parent()), type(holder.type)
{
	valid = type != nullptr;
	setupValidity();
}

LnTypeHolder::~LnTypeHolder() {}

LnTypeHolder &LnTypeHolder::operator =(const LnTypeHolder &h)
{
	if (this !=	&h) {
		type = h.type;
		valid = type != nullptr;
		setupValidity();
	}
	return *this;
}
LnTypeHolder &LnTypeHolder::operator =(LinkType *t)
{
	type = t;
	valid = type != nullptr;
	setupValidity();
	return *this;
}

void LnTypeHolder::setupValidity()
{
	if (valid) {
		connect(type, &LinkType::edited, this, &LnTypeHolder::edited);
		connect(type, &LinkType::destroyed, this, [this]() {
			type = nullptr;
			valid = false;
			Q_EMIT destroyed();
		});
	}
}

void LnTypeHolder::release()
{
	if (type) {
		delete type;
	}
	valid = false;
}

LinksTypeHolder::LinksTypeHolder()
{
	addType("?", {}, Qt::black);
	uidMapped.first() = 0;
	stringMapped[types.first().type->name] = 0;
	types.first().type->uid = 0;
}

void LinksTypeHolder::addType(QString n, QList<int> l, QColor c)
{
	LinkType *empty = new LinkType;
	empty->name = n;
	empty->inherits = l;
	empty->color = c;
	empty->uid = generateUID();
	types.append(LnTypeHolder(empty));
	stringMapped[empty->name] = empty->uid;
	uidMapped[empty->uid] = types.indexOf(LnTypeHolder(empty));

	Relayer::instance()->updateLinksList();
}

LnTypeHolder LinksTypeHolder::getByName(QString name)
{
	std::cout << "Getting by name" << std::endl;
	int i = 0;
	for (auto t : types) {
		std::cout << "Looking at: " << i << std::endl;
		if (t.valid) {
			std::cout << t.type->name.toStdString() << std::endl;
			if (t.type->name == name) {
				std::cout << "Found required type" << std::endl;
				return types[i];
			}
		} else {
			std::cout << "Invalid type!" << std::endl;
		}
		i++;
	}
	std::cout << "Required type not found" << std::endl;
	return LnTypeHolder(nullptr);
}

LnTypeHolder LinksTypeHolder::getByUID(int uid)
{
	return types[uidMapped[uid]];
}

void LinksTypeHolder::resolveInherits_(LnTypeHolder type, QList<QString> &input)
{
	if (type.valid) {
		if (!input.contains(type.type->name)) {
			input.append(type.type->name);
			for (auto t : type.type->inherits) {
				resolveInherits_(LinksTypeHolder::instance()->getByUID(t), input);
			}
		}
	}
}

QList<QString> LinksTypeHolder::resolveInherits(QString n)
{
	 QList<QString> l;
	 resolveInherits_(getByName(n), l);
	 l.removeAll(n);
	 return l;
}

void LinksTypeHolder::resolveInherits_(LnTypeHolder type, QList<int> &input)
{
	if (type.valid) {
		input << type.type->uid;
		if (!input.contains(type.type->uid)) {
			for (auto t : type.type->inherits) {
				resolveInherits_(LinksTypeHolder::instance()->getByUID(t), input);
			}
		}
	}
}

QList<int> LinksTypeHolder::resolveInherits(int t)
{
	QList<int> l;
	resolveInherits_(getByUID(t), l);
	l.removeAll(t);
	return l;
}

int LinksTypeHolder::generateUID()
{
	QList<int> allocated;

	std::sort(allocated.begin(), allocated.end());
	int i = 1;
	while (i < allocated.length()) {
		if (allocated[i] != i) {
			return i;
		}
		i++;
	}
	return i;
}

QList<QString> LinksTypeHolder::typeNames()
{
	return stringMapped.keys();
}

/*
 * TyA +-> TyB
 *	   |-> TyC +-> TyE
 *     |       |-> TyF
 *     |       +-> TyG
 *     +-> TyD
 *
 * Input to Output:
 * Input type is TyC
 * Means Output can be: TyC, TyE, TyF and TyG.
 *
 * Output to Input:
 * Output type is TyA
 * Means Input can be: TyA
 *
 */

bool LinksTypeHolder::doesSInheritsE(int s, int e)
{
	QList<int> l = resolveInherits(s);
	std::cout << "Input: " << getByUID(s).type->name.toStdString() << ", Output: " << getByUID(s).type->name.toStdString() << std::endl;
	if (s == e) {
		return true;
	}
	return l.contains(e);
}

void LinksTypeHolder::cleanup()
{
	stringMapped.clear();
	uidMapped.clear();

	for (auto e : types) {
		e.release();
	}

	types.clear();
}
