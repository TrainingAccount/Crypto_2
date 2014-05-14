#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define BMP_HEADER_SIZE 54
#define MAX_SIZE 32

#include <QMainWindow>
#include <stdio.h>
#include <cmath>
#include <string.h>
#include <iostream>
//#include <conio.h>
#include <fstream>

using namespace std;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int lsbEncrypt();
    int lsbDecrypt();
    int desEncrypt();
    int desDecrypt();
    int caezarEncrypt();
    int caezarDecrypt();
    int vidjEncrypt();
    int vidjDecrypt();

private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_radioButton_8_clicked();
    void on_radioButton_7_clicked();
    void on_radioButton_10_clicked();
    void on_radioButton_9_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_clicked();
    void on_pushButton_clicked();
    void on_radioButton_5_clicked(bool checked);
    void on_radioButton_6_clicked(bool checked);

};

#endif // MAINWINDOW_H
