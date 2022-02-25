#include "nodewindow.h"
#include "sharedinstances.h"
#include "linkstype.h"

#include <QApplication>

int main(int argv, char *args[])
{
	QApplication app(argv, args);
	SharedInstances shared;

	NodeWindow w;
	w.show();

    return app.exec();
}
