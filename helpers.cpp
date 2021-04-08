#include "helpers.h"

UIDC *UIDC::instance()
{
    static UIDC *inst = new UIDC;
    return inst;
}

void UIDC::less()
{
    this->generationPos+=1;
}

int UIDC::uid()
{
    int v = UIDC::instance()->generationPos;
    UIDC::instance()->less();
    return v;
}
