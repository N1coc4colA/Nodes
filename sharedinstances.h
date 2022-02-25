#ifndef SHAREDINSTANCES_H
#define SHAREDINSTANCES_H

#include <QObject>

class LinksTypeHolder;
class NodePalette;
class Relayer;
class StringsHolder;

class SharedInstances : public QObject
{
	Q_OBJECT
public:
	explicit SharedInstances();
	~SharedInstances();

	static SharedInstances *instance();
	static void setInstance(SharedInstances *);

	LinksTypeHolder *typesHolder();
	NodePalette *nodePalette();
	Relayer *relayer();
	StringsHolder *holder();

private:
	LinksTypeHolder *m_typesHolder;
	NodePalette *m_nodePalette;
	Relayer *m_relayer;
	StringsHolder *m_sh;

	inline static SharedInstances *inst = nullptr;
};

#endif // SHAREDINSTANCES_H
