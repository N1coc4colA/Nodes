#include "dom.h"

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

char *readChar(std::ifstream &stream, size_t &offset)
{
	long size = 0;
	stream.read((char*)&size, sizeof(size));
	if (stream.fail()) {
		return nullptr;
	}
	offset += sizeof(size);

	char *allocated = new char [size];
	if (allocated) {
		stream.read(allocated, size);
		if (stream.fail()) {
			delete [] allocated;
			return nullptr;
		}
		offset += size;
	}

	return allocated;
}

QString readString(StringsHolder *holder, std::ifstream &stream, size_t &offset)
{
	long size = 0;
	stream.read((char*)&size, sizeof(size));
	if (stream.fail()) {
		return nullptr;
	}
	offset += sizeof(size);

	char *allocated = new char [size];
	if (allocated) {
		stream.read(allocated, size);
		if (stream.fail()) {
			delete [] allocated;
			return QString();
		}
		offset += size;
	}

	holder->add(allocated);

	QString v;
	long i = 0;
	while (i < size) {
		v += allocated[i];
		i++;
	}

	return v;
}

bool ColorStruct::read(std::ifstream &stream, size_t &offset)
{
	stream.read((char *)this, sizeof(this));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(this);
	return true;
}

bool TypeStruct::read(StringsHolder *holder, std::ifstream &stream, size_t &offset)
{
	stream.read((char*)&ID, sizeof(ID));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(ID);

	//memcpy((char *)&name, (char *)&tmp, sizeof(name));
	name = readString(holder, stream, offset);
	std::cout << "Found type: " << name.toStdString() << std::endl;
	color.read(stream, offset);

	int i = 0, len = 0;
	stream.read((char*)&len, sizeof(len));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(len);

	while (i<len) {
		int t = -1;
		stream.read((char*)&t, sizeof(int));
		if (!stream.fail()) {
			inherits << t;
			offset += sizeof(int);
		} else {
			return false;
		}
		i++;
	}
	return true;
}

bool NodeStruct::read(StringsHolder *holder, std::ifstream &stream, size_t &offset)
{
	stream.read((char*)&ID, sizeof(ID));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(ID);

	name = readString(holder, stream, offset);
	type = readString(holder, stream, offset);

	stream.read((char*)&xpos, sizeof(xpos));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(xpos);
	stream.read((char*)&ypos, sizeof(ypos));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(ypos);

	int i = 0, len = 0;
	stream.read((char*)&len, sizeof(len));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(len);

	while (i<len) {
		int p = -1;
		stream.read((char*)&p, sizeof(int));
		if (stream.fail()) {
			return false;
		}
		ports << p;
		offset += sizeof(int);
		i++;
	}
	return true;
}

bool PortStruct::read(StringsHolder *holder, std::ifstream &stream, size_t &offset)
{
	stream.read((char*)&ID, sizeof(ID));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(ID);

	stream.read((char*)&isOutput, sizeof(isOutput));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(isOutput);

	stream.read((char*)&type, sizeof(type));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(type);

	name = readString(holder, stream, offset);
	return true;
}

bool ConnectionStruct::read(std::ifstream &stream, size_t &offset)
{
	stream.read((char*)&ID, sizeof(ID));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(ID);

	stream.read((char*)&sourceID, sizeof(sourceID));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(sourceID);

	stream.read((char*)&targetID, sizeof(targetID));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(targetID);
	return true;
}

bool DateStruct::read(std::ifstream &stream, size_t &offset)
{
	stream.read((char*)this, sizeof(this));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(this);
	return true;
}

bool AdditionnalDataStruct::read(StringsHolder *holder, std::ifstream &stream, size_t &offset)
{
	authorName = readString(holder, stream, offset);
	if (!date.read(stream, offset)) {
		return false;
	}

	stream.read((char*)&has_types, sizeof(has_types));
	if (stream.fail()) {
		return false;
	}
	stream.read((char*)&has_nodes, sizeof(has_nodes));
	if (stream.fail()) {
		return false;
	}
	stream.read((char*)&has_ports, sizeof(has_ports));
	if (stream.fail()) {
		return false;
	}
	stream.read((char*)&has_conns, sizeof(has_conns));
	if (stream.fail()) {
		return false;
	}

	return true;
}

bool APIVersion::read(std::ifstream &stream, size_t &offset)
{
	stream.read((char*)this, sizeof(this));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(this);
	return true;
}

bool readTypes(StringsHolder *holder, QList<TypeStruct> &types, std::ifstream &stream, size_t &offset)
{
	int len = 0;
	stream.read((char*)&len, sizeof(len));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(len);

	int i = 0;
	while (i<len) {
		TypeStruct s;
		if (!s.read(holder, stream, offset)) {
			std::cout << "Aborted due to read error!" << std::endl;
			return false;
		}
		types << s;
		i++;
	}
	return true;
}

bool readPorts(StringsHolder *holder, QList<PortStruct> &ports, std::ifstream &stream, size_t &offset)
{
	int len = 0;
	stream.read((char*)&len, sizeof(len));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(len);

	int i = 0;
	while (i<len) {
		PortStruct s;
		if (!s.read(holder, stream, offset)) {
			return false;
		}
		ports << s;
		i++;
	}
	return true;
}

bool readNodes(StringsHolder *holder, QList<NodeStruct> &nodes, std::ifstream &stream, size_t &offset)
{
	int len = 0;
	stream.read((char*)&len, sizeof(len));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(len);

	int i = 0;
	while (i<len) {
		NodeStruct s;
		if (!s.read(holder, stream, offset)) {
			return false;
		}
		nodes << s;
		i++;
	}
	return true;
}

bool readConnections(QList<ConnectionStruct> &conns, std::ifstream &stream, size_t &offset)
{
	int len = 0;
	stream.read((char*)&len, sizeof(len));
	if (stream.fail()) {
		return false;
	}
	offset += sizeof(len);

	int i = 0;
	while (i<len) {
		ConnectionStruct s;
		if (s.read(stream, offset)) {
			return false;
		}
		conns << s;
		i++;
	}
	return true;
}

bool readOffset(std::ifstream &stream, size_t &current)
{
	size_t stored = 0;
	stream.read((char*)&stored, sizeof(stored));
	if (stream.fail()) {
		return false;
	}
	current += sizeof(stored);
	return stored == current;
}
