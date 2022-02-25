#include "dom.h"

#include <fstream>
#include <iostream>

void writeChar(char *c, std::ofstream &stream, size_t &offset)
{
	long namesize = strlen(c) + 1;
	stream.write((char*)&namesize, sizeof(namesize));
	stream.write(c, namesize);

	offset += sizeof(namesize) + namesize;
}

void writeString(QString string, std::ofstream &stream, size_t &offset)
{
	std::string v = string.toStdString();
	long s = v.length();
	stream.write((char*)&s, sizeof(s));
	stream.write(v.c_str(), s);

	offset += sizeof(s) + s;
}

void ColorStruct::write(std::ofstream &stream, size_t &offset)
{
	stream.write((char *)this, sizeof(this));
	offset += sizeof(this);
}

void TypeStruct::write(std::ofstream &stream, size_t &offset)
{
	stream.write((char*)&ID, sizeof(ID));
	writeString(name, stream, offset);

	color.write(stream, offset);

	int i = 0;
	int len = inherits.length();
	stream.write((char*)&len, sizeof(len));

	while (i<len) {
		stream.write((char*)&inherits[i], sizeof(inherits[i]));
		offset += sizeof(inherits[i]);
		i++;
	}


	offset += sizeof(ID)+sizeof(len);
}

void NodeStruct::write(std::ofstream &stream, size_t &offset)
{
	stream.write((char*)&ID, sizeof(ID));

	writeString(name, stream, offset);
	writeString(type, stream, offset);

	stream.write((char*)&xpos, sizeof(xpos));
	stream.write((char*)&ypos, sizeof(ypos));

	int i = 0;
	int len = ports.length();
	stream.write((char*)&len, sizeof(len));

	offset += sizeof(len)+sizeof(xpos)+sizeof(ypos)+sizeof (ID);

	while (i<len) {
		stream.write((char*)&ports[i], sizeof(ports[i]));
		offset += sizeof(ports[i]);
		i++;
	}
}

void PortStruct::write(std::ofstream &stream, size_t &offset)
{
	stream.write((char*)&ID, sizeof(ID));
	stream.write((char*)&isOutput, sizeof(isOutput));
	stream.write((char*)&type, sizeof(type));

	writeString(name, stream, offset);

	offset += sizeof(ID)+sizeof(isOutput)+sizeof(type);
}

void ConnectionStruct::write(std::ofstream &stream, size_t &offset)
{
	stream.write((char*)&ID, sizeof(ID));
	stream.write((char*)&sourceID, sizeof(sourceID));
	stream.write((char*)&targetID, sizeof(targetID));

	offset += sizeof(targetID)+sizeof(ID)+sizeof(sourceID);
}

void DateStruct::write(std::ofstream &stream, size_t &offset)
{
	stream.write((char*)this, sizeof(this));
	offset += sizeof(this);
}

void AdditionnalDataStruct::write(std::ofstream &stream, size_t &offset)
{
	writeString(authorName, stream, offset);
	date.write(stream, offset);

	stream.write((char*)&has_types, sizeof(has_types));
	stream.write((char*)&has_nodes, sizeof(has_nodes));
	stream.write((char*)&has_ports, sizeof(has_ports));
	stream.write((char*)&has_conns, sizeof(has_conns));
}

void APIVersion::write(std::ofstream &stream, size_t &offset)
{
	stream.write((char*)this, sizeof(this));
	offset += sizeof(this);
}

void writeTypes(QList<TypeStruct> types, std::ofstream &stream, size_t &offset)
{
	int len = types.length();
	stream.write((char*)&len, sizeof(len));
	offset += sizeof(len);

	int i = 0;
	while (i<len) {
		types[i].write(stream, offset);
		i++;
	}
	std::cout << "Total number of types: " << len << ", written: " << i << std::endl;
}

void writePorts(QList<PortStruct> ports, std::ofstream &stream, size_t &offset)
{
	int len = ports.length();
	stream.write((char*)&len, sizeof(len));
	offset += sizeof(len);

	int i = 0;
	while (i<len) {
		ports[i].write(stream, offset);
		i++;
	}
}

void writeNodes(QList<NodeStruct> nodes, std::ofstream &stream, size_t &offset)
{
	int len = nodes.length();
	stream.write((char*)&len, sizeof(len));
	offset += sizeof(len);

	int i = 0;
	while (i<len) {
		nodes[i].write(stream, offset);
		i++;
	}
}

void writeConnections(QList<ConnectionStruct> conns, std::ofstream &stream, size_t &offset)
{
	int len = conns.length();
	stream.write((char*)&len, sizeof(len));
	offset += sizeof(len);

	int i = 0;
	while (i<len) {
		conns[i].write(stream, offset);
		i++;
	}
}

void writeOffset(std::ofstream &stream, size_t &current)
{
	current += sizeof(current);
	stream.write((char*)&current, sizeof(current));
}
