#include "nodewindow.h"

#include <QApplication>

int main(int argv, char *args[])
{
	QApplication app(argv, args);

	NodeWindow w;
	w.show();

    return app.exec();
}
