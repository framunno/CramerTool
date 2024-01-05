#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <array>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_CLEARbutton_clicked();

    void on_SOLVEbutton_clicked();

private:
    std::array<std::array<double, 3>, 3> matrix;
    double Anum, Bnum, Cnum;
    double det, detX, detY, detZ, Xnum, Ynum,Znum;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
