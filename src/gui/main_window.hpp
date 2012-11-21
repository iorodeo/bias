#ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
#define BIAS_TEST_GUI_MAINWINDOW_HPP

#include "ui_main_window.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);
       
};


#endif // #ifndef BIAS_TEST_GUI_MAINWINDOW_HPP
