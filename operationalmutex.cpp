#include "operationalmutex.h"

#include <iostream>

OperationalMutex::OperationalMutex() {}

void OperationalMutex::lock()
{
#ifdef DEBUG_MUTEXES
	std::cout << "Locking operations..." << std::endl;
#endif
	opMtx.lock();
	opMtx.unlock();
#ifdef DEBUG_MUTEXES
	std::cout << "Operations locked" << std::endl;
#endif
	mtx.lock();
}

void OperationalMutex::unlock()
{
#ifdef DEBUG_MUTEXES
	std::cout << "Operations unlocked" << std::endl;
#endif
	mtx.unlock();
}

void OperationalMutex::startOperation()
{
#ifdef DEBUG_MUTEXES
	std::cout << "Starting new operation..." << std::endl;
#endif
	mtx.lock();
	mtx.unlock();
#ifdef DEBUG_MUTEXES
	std::cout << "Operation started" << std::endl;
#endif

	sum(+1);
	if (get() == 1) {
#ifdef DEBUG_MUTEXES
		std::cout << "Operation's mutex locked." << std::endl;
#endif
		opMtx.lock();
	}
}

void OperationalMutex::endOperation()
{
#ifdef DEBUG_MUTEXES
	std::cout << "An operation ended" << std::endl;
#endif
	sum(-1);
	if (null()) {
#ifdef DEBUG_MUTEXES
		std::cout << "Operation's mutex unlocked" << std::endl;
#endif
		opMtx.unlock();
	}
}

void OperationalMutex::sum(int x)
{
	cMtx.lock();
	count += x;
	cMtx.unlock();
}

bool OperationalMutex::null()
{
	cMtx.lock();
	bool o = count == 0;
	cMtx.unlock();
	return o;
}

int OperationalMutex::get()
{
	cMtx.lock();
	int o = count;
	cMtx.unlock();
	return o;
}

OMHolder::OMHolder(OperationalMutex *mtx, QString n) : mutex(mtx)
{
#ifdef DEBUG_MUTEXES
	name = n;
	std::cout << "New operations holder created" << std::endl;
#else
	Q_UNUSED(n);
#endif
}

OperationalMutex *OMHolder::getMutex() const
{
	return mutex;
}

bool OMHolder::isStopped()
{
	return stopped;
}

void OMHolder::stop()
{
	stopped = true;
}

OMLocker::OMLocker(OperationalMutex *mtx, QString n) : OMHolder(mtx, n)
{
	if (mutex) {
#ifdef DEBUG_MUTEXES
		std::cout << name.toStdString() << ": Locker locking mutex" << std::endl;
#endif
		mutex->lock();
	}
}

OMLocker::~OMLocker()
{
	if (mutex && !stopped) {
#ifdef DEBUG_MUTEXES
		std::cout << name.toStdString() << ": Locker unlocking mutex" << std::endl;
#endif
		mutex->unlock();
	}
}

void OMLocker::stop()
{
	if (mutex) {
#ifdef DEBUG_MUTEXES
		std::cout << name.toStdString() << ": Locker unlocking mutex" << std::endl;
#endif
		mutex->unlock();
	}
	return OMHolder::stop();
}

OMOperator::OMOperator(OperationalMutex *mtx, QString n) : OMHolder(mtx, n)
{
	if (mutex && !stopped) {
#ifdef DEBUG_MUTEXES
		std::cout << name.toStdString() << ": Registering new operation" << std::endl;
#endif
		mutex->startOperation();
	}
}

OMOperator::~OMOperator()
{
	if (mutex) {
#ifdef DEBUG_MUTEXES
		std::cout << name.toStdString() << ": Unregistering an operation" << std::endl;
#endif
		mutex->endOperation();
	}
}

void OMOperator::stop()
{
	if (mutex) {
#ifdef DEBUG_MUTEXES
		std::cout << name.toStdString() << ": Unregistering an operation" << std::endl;
#endif
		mutex->endOperation();
	}
	return OMHolder::stop();
}
