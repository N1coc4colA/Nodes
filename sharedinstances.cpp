#include "sharedinstances.h"

#include "relayer.h"
#include "nodepalette.h"
#include "linkstype.h"
#include "dom.h"

#include <iostream>

SharedInstances::SharedInstances() : QObject(nullptr)
{
	if (!inst) {
		inst = this;
	}
	m_sh = new StringsHolder;
	m_relayer = new Relayer(this);
	m_nodePalette = new NodePalette(this);
	m_typesHolder = new LinksTypeHolder;
}

SharedInstances::~SharedInstances()
{
	delete m_typesHolder;
	m_sh->release();
	delete m_sh;
}

SharedInstances *SharedInstances::instance()
{
	return inst;
}

Relayer *SharedInstances::relayer()
{
	return m_relayer;
}

NodePalette *SharedInstances::nodePalette()
{
	return m_nodePalette;
}

LinksTypeHolder *SharedInstances::typesHolder()
{
	return m_typesHolder;
}

void SharedInstances::setInstance(SharedInstances *i)
{
	std::cout << "Has set the instance." << std::endl;
	inst = i;
}

StringsHolder *SharedInstances::holder()
{
	return m_sh;
}
