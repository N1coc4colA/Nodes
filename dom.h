#ifndef DOM_H
#define DOM_H

#include <string>
#include <QList>
#include <QColor>

#define VERSION_ALPHA 0
#define VERSION_BETA 0
#define VERSION_GAMMA 1

class LnTypeHolder;
class PortItem;
class NodeItem;
class QDate;

struct StringsHolder {
	QList<char *> strings;
	void add(char *target);
	void release(char *target);

	void release();
};

struct ColorStruct {
	unsigned int r : 1;
	unsigned int g : 1;
	unsigned int b : 1;
	unsigned int a : 1;

	void write(std::ofstream &stream, size_t &offset);
	bool read(std::ifstream &stream, size_t &sum);

	QColor convert();
	void from(QColor c);
};

struct TypeStruct {
	explicit TypeStruct();
	explicit TypeStruct(int id, ColorStruct c, QString n, QList<int> inh);
	TypeStruct(const TypeStruct &src);

	int ID = -1;
	ColorStruct color;
	QString name;
	QList<int> inherits;

	void write(std::ofstream &stream, size_t &offset);
	bool read(StringsHolder *holder, std::ifstream &stream, size_t &sum);

	void addConvertion();
	void from(LnTypeHolder holder);

	//TypeStruct(const TypeStruct &src);
	//TypeStruct &operator = (TypeStruct src);
	TypeStruct &operator = (const TypeStruct &src);
};

struct ConnectionStruct {
    int ID = -1;
    int targetID = -1;
    int sourceID = -1;

	void write(std::ofstream &stream, size_t &offset);
	bool read(std::ifstream &stream, size_t &sum);
};

struct PortStruct {
    int ID = -1;
	bool isOutput = false;
	int type = 0;
	QString name;

	void write(std::ofstream &stream, size_t &offset);
	bool read(StringsHolder *holder, std::ifstream &stream, size_t &sum);

	void from(PortItem *);
};

struct NodeStruct {
    int ID = -1;
    double xpos = 0;
	double ypos = 0;
	QString name;
	QString type;
    QList<int> ports;

	void write(std::ofstream &stream, size_t &offset);
	bool read(StringsHolder *holder, std::ifstream &stream, size_t &sum);

	void from(NodeItem *it);
};

struct DateStruct {
	int day : 1;
	int month : 1;
	int year : 2;

	void write(std::ofstream &stream, size_t &offset);
	bool read(std::ifstream &stream, size_t &offset);

	void from(QDate d);
};

struct AdditionnalDataStruct {
	QString authorName;
	DateStruct date;

	bool has_types = false;
	bool has_nodes = false;
	bool has_ports = false;
	bool has_conns = false;

	void write(std::ofstream &stream, size_t &offset);
	bool read(StringsHolder *holder, std::ifstream &stream, size_t &offset);
};

struct APIVersion {
	unsigned int alpha : 1;
	unsigned int beta : 1;
	unsigned int gamma : 1;

	void write(std::ofstream &stream, size_t &offset);
	bool read(std::ifstream &stream, size_t &offset);
};

void writeTypes(QList<TypeStruct> l, std::ofstream &stream, size_t &offset);
void writePorts(QList<PortStruct> l, std::ofstream &stream, size_t &offset);
void writeNodes(QList<NodeStruct> l, std::ofstream &stream, size_t &offset);
void writeConnections(QList<ConnectionStruct> l, std::ofstream &stream, size_t &offset);
void writeOffset(std::ofstream &stream, size_t &current);

bool readTypes(StringsHolder *holder, QList<TypeStruct> &l, std::ifstream &stream, size_t &offset);
bool readPorts(StringsHolder *holder, QList<PortStruct> &l, std::ifstream &stream, size_t &offset);
bool readNodes(StringsHolder *holder, QList<NodeStruct> &l, std::ifstream &stream, size_t &offset);
bool readConnections(QList<ConnectionStruct> &l, std::ifstream &stream, size_t &offset);
bool readOffset(std::ifstream &stream, size_t &current);

#endif // DOM_H
