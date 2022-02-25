#include "loadingdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>


LoadingDialog::LoadingDialog(NodesOpener *opener)
{
    QVBoxLayout *lay = new QVBoxLayout;
    QLabel *file = new QLabel(opener->fp());
    QLabel *name = new QLabel;

    QHBoxLayout *loading = new QHBoxLayout;
    QProgressBar *bar = new QProgressBar;
    QLabel *step = new QLabel;

    QLabel *err = new QLabel;

    loading->addWidget(bar);
    loading->addWidget(step);
    lay->addWidget(file);
    lay->addLayout(loading);

    setLayout(lay);

    connect(opener, &NodesOpener::stepFound, this, [=](int v) {
        step->setText(QString::number(v+1) + "/5");
    });
    connect(opener, &NodesOpener::itemLoad, this, [=](double v) {
        bar->setValue((int)v);
    });
    connect(opener, &NodesOpener::nameFound, this, [=](QString v) {
        name->setText(tr("Author: ") + v);
    });
    connect(opener, &NodesOpener::endedLoad, this, [=](bool failed) {
        if (!failed) {
            this->done(0);
        } else {
            err->setText(tr("The file is corrupted."));
        }
    });
    this->setWindowModality(Qt::ApplicationModal);

    m_opener = opener;
    m_opener->setParent(this);
}

void LoadingDialog::run()
{
    //this->exec();
    m_opener->load();
}
