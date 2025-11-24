#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "BoardWidget.h"
#include "GameWidget.h"
#include "GameController.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QThread gameContrThread;
};
#endif // MAINWINDOW_H
