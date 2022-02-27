#include "sharedinstances.h"

#include "relayer.h"
#include "nodepalette.h"
#include "linkstype.h"
#include "dom.h"

SharedInstances::SharedInstances() : QObject(nullptr)
{
	if (!inst) {
		setInstance(this);
	}
	m_relayer = new Relayer(this);
	m_nodePalette = new NodePalette(this);
	m_typesHolder = new LinksTypeHolder;
}

SharedInstances::~SharedInstances()
{
	delete m_typesHolder;
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
	inst = i;
}
