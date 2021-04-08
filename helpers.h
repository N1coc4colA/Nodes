#ifndef HELPERS_H
#define HELPERS_H

#include <QGraphicsItem>

template <class T>
inline bool isinstance(void *x)
{
    return ((x != nullptr) ? (dynamic_cast<T *>((QGraphicsItem *)x) != nullptr) : false);
}

class UIDC {
public:
    static UIDC *instance();
    static int uid();
    void less();
private:
    int generationPos = 0;
};

#endif // HELPERS_H
