#ifndef DOM_H
#define DOM_H

#include <string>
#include <QList>

#define VERSION_ALPHA 0
#define VERSION_BETA 0
#define VERSION_OMEGA 0

struct ConnectionStruct {
    int ID = -1;
    int targetID = -1;
    int sourceID = -1;
};

struct PortStruct {
    int ID = -1;
    bool isOutput;
    char *name;
};

struct NodeStruct {
    int ID = -1;
    double xpos = 0;
    double ypos = 0;
    char *name;
    char *type;
    QList<int> ports;
};

struct APIVersion {
    int alpha = VERSION_ALPHA;
    int beta = VERSION_BETA;
    int omega = VERSION_OMEGA;
};

#endif // DOM_H
