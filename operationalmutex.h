#ifndef OPERATIONALMUTEX_H
#define OPERATIONALMUTEX_H

#include <QString>
#include <QMutex>

class OperationalMutex
{
public:
	OperationalMutex();

	void lock();
	void unlock();
	void startOperation();
	void endOperation();

protected:
	void sum(int x);
	bool null();
	int get();

private:
	QMutex mtx;
	QMutex opMtx;
	QMutex cMtx;
	int count = 0;
};

class OMHolder
{
public:
	explicit OMHolder(OperationalMutex *mutex, QString n = "");

	OperationalMutex *getMutex() const;
	bool isStopped();
	virtual void stop();

protected:
	OperationalMutex *mutex;
#ifdef DEBUG_MUTEXES
	QString name;
#endif
	bool stopped = false;
};

class OMLocker : public OMHolder
{
public:
	explicit OMLocker(OperationalMutex *mutex, QString n = "");
	~OMLocker();

	void stop() override;
};

class OMOperator : public OMHolder
{
public:
	explicit OMOperator(OperationalMutex *mutex, QString n = "");
	~OMOperator();

	void stop() override;
};

#endif // OPERATIONALMUTEX_H
