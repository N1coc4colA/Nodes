#include "relayer.h"

Relayer::Relayer(QObject *parent) : QObject(parent) {}

void Relayer::editNode(NodeItem *e)
{
	Q_EMIT reqEditNode(e);
}

void Relayer::addNode()
{
	Q_EMIT reqAddNode();
}

void Relayer::rmElement()
{
	Q_EMIT reqRmElement();
}

void Relayer::runSave()
{
	Q_EMIT reqSave();
}

void Relayer::runLoad()
{
	Q_EMIT reqLoad();
}

void Relayer::setScaling(double s)
{
	Q_EMIT reqSetScaling(s);
}

void Relayer::dispScaled(double s)
{
	Q_EMIT scaled(s);
}

void Relayer::enable()
{
	Q_EMIT reqEnable();
}

void Relayer::disable()
{
	Q_EMIT reqDisable();
}

void Relayer::showLinksList()
{
	Q_EMIT reqLinksList();
}

void Relayer::updateLinksList()
{
	Q_EMIT reqLinksListUpd();
}

void Relayer::addLinkType(LnTypeHolder &t)
{
	Q_EMIT reqAddLink(t);
}

void Relayer::editCurrent()
{
	Q_EMIT reqEditCurrent();
}

void Relayer::linkEditor(QWidget *p)
{
	Q_EMIT reqLnEditor(p);
}

void Relayer::linkEditorFrom(QWidget *p, LnTypeHolder h)
{
	Q_EMIT reqLnEditorFrom(p, h);
}

void Relayer::dispNodePaletteChange()
{
	Q_EMIT nodePaletteChanged();
}
