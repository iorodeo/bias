#ifndef FLY_SORTER_WINDOW_HPP
#define FLY_SORTER_WINDOW_HPP

#include "ui_fly_sorter_window.h"
#include <memory>
#include <QMainWindow>
#include <QPointer>


class FlySorterWindow : public QMainWindow, private Ui::FlySorterWindow
{
    Q_OBJECT

    public:

        FlySorterWindow(QWidget *parent=0);

    private slots:

        void startPushButtonClicked();
        void httpOutputCheckBoxChanged(int state);

    private:

        bool connected_;
        void connectWidgets();

};

#endif // #ifndef FLY_SORTER_WINDOW_HPP
