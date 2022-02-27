#ifndef LINKSTYPE_H
#define LINKSTYPE_H

#include "operationalmutex.h"

#include <QObject>
#include <QMap>
#include <QColor>

class LinkType;

class LnTypeHolder : public QObject
{
	Q_OBJECT
public:
	explicit LnTypeHolder(LinkType *t = nullptr, QObject *p = nullptr);
	LnTypeHolder(const LnTypeHolder &holder);
	~LnTypeHolder();

	LinkType *type = nullptr;
	bool valid = false;

	LnTypeHolder &operator= (const LnTypeHolder &h);
	LnTypeHolder &operator =(LinkType *t);

	inline bool operator ==(const LnTypeHolder &h) {
		return type == h.type;
	}

	inline bool operator !=(const LnTypeHolder &h) {
		return type != h.type;
	}

	void release();

Q_SIGNALS:
	void destroyed();
	void edited();

protected:
	void setupValidity();
};

Q_DECLARE_METATYPE(LnTypeHolder);

class LinkType : public QObject
{
	Q_OBJECT
public:
	explicit LinkType(QObject *p = nullptr);
	~LinkType();

	QString name = "";
	qint64 uid = -1;
	QList<qint64> inherits = {};
	QColor color = Qt::gray;

	void notifyEdit();

	inline bool operator ==(const LinkType &t) {
		return (t.name == name && t.uid == uid && t.inherits == inherits);
	}

	inline bool operator !=(const LinkType &t) {
		return (t.name != name || t.uid != uid || t.inherits != inherits);
	}

	inline operator LnTypeHolder() {
		return LnTypeHolder(this);
	}

Q_SIGNALS:
	void destroyed();
	void edited();
};

class LinksTypeHolder
{
public:
	explicit LinksTypeHolder();

	QMap<qint64, int> uidMapped;
	QMap<QString, int> stringMapped;
	QList<LnTypeHolder> types;

	void addType(QString name, QList<qint64> inh, QColor c);
	LnTypeHolder getByName(QString name);
	LnTypeHolder getByUID(int uid);
	void resolveInherits_(LnTypeHolder h, QList<QString> &input);
	QList<QString> resolveInherits(QString);
	void resolveInherits_(LnTypeHolder h, QList<qint64> &input);
	QList<qint64> resolveInherits(qint64);
	QList<QString> typeNames();
	int generateUID();
	bool doesSInheritsE(qint64 e, qint64 s);

	void cleanup();

private:
	OperationalMutex mtx;
};

#endif // LINKSTYPE_H
