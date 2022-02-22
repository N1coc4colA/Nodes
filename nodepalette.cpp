#include "nodepalette.h"

#include "relayer.h"

NodePalette::NodePalette(QObject *p) : QObject(p) {}

NodePalette::~NodePalette() {}

QColor NodePalette::getInner()
{
	return m_data.inner;
}

QColor NodePalette::getOutter()
{
	return m_data.outter;
}

QColor NodePalette::getTitle()
{
	return m_data.title;
}

QColor NodePalette::getType()
{
	return m_data.type;
}

QColor NodePalette::getLeft()
{
	return m_data.left;
}

QColor NodePalette::getRight()
{
	return m_data.right;
}

NodePaletteData &NodePalette::getData()
{
	return m_data;
}

void NodePalette::setData(NodePaletteData d)
{
	m_data = d;
	Relayer::instance()->dispNodePaletteChange();
}

