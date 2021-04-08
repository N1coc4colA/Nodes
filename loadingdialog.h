#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include "nodesopener.h"

#include <QDialog>

class LoadingDialog : public QDialog
{
public:
    explicit LoadingDialog(NodesOpener *opener);
    void run();

private:
    NodesOpener *m_opener;
};

#endif // LOADINGDIALOG_H
