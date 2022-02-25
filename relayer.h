#ifndef RELAYER_H
#define RELAYER_H

#include "linkstype.h"

class NodeItem;
class LnTypeHolder;

class Relayer : public QObject
{
	Q_OBJECT
public:
	explicit Relayer(QObject *parent = nullptr);

	void editNode(NodeItem *);
	void addNode();
	void rmElement();
	void runSave();
	void runLoad();
	void setScaling(double);
	void dispScaled(double);
	void enable();
	void disable();
	void showLinksList();
	void updateLinksList();
	void addLinkType(LnTypeHolder &t);
	void editCurrent();
	void linkEditor(QWidget *p);
	void linkEditorFrom(QWidget *p, LnTypeHolder h);
	void dispNodePaletteChange();

Q_SIGNALS:
	void reqEditNode(NodeItem *);
	void reqAddNode();
	void reqRmElement();
	void reqLoad();
	void reqSave();
	void reqSetScaling(double);
	void scaled(double);
	void reqEnable();
	void reqDisable();
	void reqLinksList();
	void reqLinksListUpd();
	void reqAddLink(LnTypeHolder t);
	void reqEditCurrent();
	void reqLnEditor(QWidget *p);
	void reqLnEditorFrom(QWidget *p, LnTypeHolder h);
	void nodePaletteChanged();
};

#endif // RELAYER_H
