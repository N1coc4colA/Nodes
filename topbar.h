#ifndef TOPBAR_H
#define TOPBAR_H

#include <QLineEdit>
#include <QDoubleSpinBox>

class TopBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopBar(QWidget *parent = nullptr);
    QString author();
    void setAuthor(QString name);

Q_SIGNALS:
    void runSave();
    void runLoad();
    void generateNode();
    void setScaling(double scale);

public Q_SLOTS:
    void updateScaling(double scale);

private:
    QLineEdit *m_author;
    QDoubleSpinBox *scale;
};

#endif // TOPBAR_H
