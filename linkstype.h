#ifndef LINKSTYPE_H
#define LINKSTYPE_H

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
	int uid = -1;
	QList<int> inherits = {};
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
	inline static LinksTypeHolder *instance() {
		static LinksTypeHolder *inst = new LinksTypeHolder;
		return inst;
	}

	QMap<int, int> uidMapped;
	QMap<QString, int> stringMapped;
	QList<LnTypeHolder> types;

	void addType(QString name, QList<int> inh, QColor c);
	LnTypeHolder getByName(QString name);
	LnTypeHolder getByUID(int uid);
	void resolveInherits_(LnTypeHolder h, QList<QString> &input);
	QList<QString> resolveInherits(QString);
	void resolveInherits_(LnTypeHolder h, QList<int> &input);
	QList<int> resolveInherits(int);
	QList<QString> typeNames();
	int generateUID();
	bool doesSInheritsE(int e, int s);

	void cleanup();
};

#endif // LINKSTYPE_H
