#ifndef DOM_H
#define DOM_H

#include <QList>
#include <QColor>
#include <QDataStream>
#include <QMetaType>

#define VERSION_ALPHA 0
#define VERSION_BETA 0
#define VERSION_GAMMA 1

class LnTypeHolder;
class PortItem;
class NodeItem;
class QDate;

class ColorData {
public:
	quint8 r;
	quint8 g;
	quint8 b;
	quint8 a;

	QColor convert();
	void from(QColor c);
};

class TypeData {
public:
	qint64 ID = -1;
	ColorData color;
	QString name;
	QList<qint64> inherits;

	void addConvertion();
	void from(LnTypeHolder holder);
};

class ConnectionData {
public:
	qint64 ID = -1;
	qint64 sourceID = -1;
	qint64 targetID = -1;
};

class PortData {
public:
	qint64 ID = -1;
	bool isOutput = false;
	qint64 type = 0;
	QString name;

	void from(PortItem *);
};

class NodeData {
public:
	qint64 ID = -1;
    double xpos = 0;
	double ypos = 0;
	QString name;
	QString type;
	QList<qint64> ports;

	void from(NodeItem *it);
};

class DateData {
public:
	qint8 day = 0;
	qint8 month = 0;
	qint16 year = 0;

	void from(QDate d);
};

class AdditionnalData {
public:
	QString authorName;
	DateData date;

	bool has_types = false;
	bool has_nodes = false;
	bool has_ports = false;
	bool has_conns = false;
};

class APIVersionData {
public:
	quint8 alpha = 0;
	quint8 beta = 0;
	quint8 gamma = 0;
};

class NodeDOM {
public:
	APIVersionData versioning;
	AdditionnalData additional;
	QList<TypeData> types;
	QList<NodeData> nodes;
	QList<PortData> ports;
	QList<ConnectionData> conns;
	quint64 sum;
	bool validity = false;

	void build(QString name);
	quint64 summup();
};

Q_DECLARE_METATYPE(ColorData);
Q_DECLARE_METATYPE(TypeData);
Q_DECLARE_METATYPE(ConnectionData);
Q_DECLARE_METATYPE(PortData);
Q_DECLARE_METATYPE(NodeData);
Q_DECLARE_METATYPE(DateData);
Q_DECLARE_METATYPE(AdditionnalData);
Q_DECLARE_METATYPE(APIVersionData);
Q_DECLARE_METATYPE(NodeDOM);

void setupMetatypes();


template <typename T>
inline void serializeList(QDataStream &out, const QList<T> &src)
{
	qint64 s = src.length();
	out << s;
	int i = 0;
	while (i < s) {
		out << src[i];
		i++;
	}
}

template <typename T>
inline void deserializeList(QDataStream &in, QList<T> &src)
{
	qint64 s;
	in >> s;
	int i = 0;
	while (i < s) {
		T ti;
		in >> ti;
		src << ti;
		i++;
	}
}

template <typename T>
void deserializeList(QDataStream &out, QList<T> &src);

QDataStream &operator<<(QDataStream &out, const ColorData &);
QDataStream &operator<<(QDataStream &out, const TypeData &);
QDataStream &operator<<(QDataStream &out, const ConnectionData &);
QDataStream &operator<<(QDataStream &out, const PortData &);
QDataStream &operator<<(QDataStream &out, const NodeData &);
QDataStream &operator<<(QDataStream &out, const DateData &);
QDataStream &operator<<(QDataStream &out, const AdditionnalData &);
QDataStream &operator<<(QDataStream &out, const APIVersionData &);
QDataStream &operator<<(QDataStream &out, const NodeDOM &);

QDataStream &operator>>(QDataStream &in, ColorData &);
QDataStream &operator>>(QDataStream &in, TypeData &);
QDataStream &operator>>(QDataStream &in, ConnectionData &);
QDataStream &operator>>(QDataStream &in, PortData &);
QDataStream &operator>>(QDataStream &in, NodeData &);
QDataStream &operator>>(QDataStream &in, DateData &);
QDataStream &operator>>(QDataStream &in, AdditionnalData &);
QDataStream &operator>>(QDataStream &in, APIVersionData &);
QDataStream &operator>>(QDataStream &in, NodeDOM &);

#endif // DOM_H
