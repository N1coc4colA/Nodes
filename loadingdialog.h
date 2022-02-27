#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include "nodesloader.h"

#include <QDialog>

class LoadingDialog : public QDialog
{
public:
	explicit LoadingDialog(NodesLoader *opener);
    void run();

private:
	NodesLoader *m_opener;
};

#endif // LOADINGDIALOG_H
