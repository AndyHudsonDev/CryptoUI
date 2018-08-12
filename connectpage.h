#ifndef CONNECTPAGE_H
#define CONNECTPAGE_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLayout>
#include <QGridLayout>
#include <QPlainTextEdit>

class ConnectPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectPage(QWidget *parent = nullptr);
    ~ConnectPage();
    void ShowPage();

signals:

public slots:
};

#endif // CONNECTPAGE_H
