#include "nodewindow.h"
#include "sharedinstances.h"
#include "dom.h"

#include <QApplication>

#include <iostream>

int main(int argv, char *args[])
{
	setupMetatypes();

	QApplication app(argv, args);
	SharedInstances shared;
	NodeWindow w;
	w.show();

    return app.exec();
}
