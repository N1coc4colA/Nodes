#ifndef HELPERS_H
#define HELPERS_H

#include <QGraphicsItem>

template <class T>
inline bool isinstance(void *x)
{
    return ((x != nullptr) ? (dynamic_cast<T *>((QGraphicsItem *)x) != nullptr) : false);
}

void centerWidget(QWidget *p, QWidget *t);

void printTrace();

class UIDC {
public:
    static UIDC *instance();
	static qint64 uid();
    void less();
private:
	qint64 generationPos = 0;
};

#endif // HELPERS_H
