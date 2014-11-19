#include <iostream>
#include <QThread>
#include <QDebug>
#include <QList>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QCoreApplication>
#include "panels_controller.hpp"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Get list of all serial ports
    QList<QSerialPortInfo> serialInfoList = QSerialPortInfo::availablePorts();
    qDebug();
    if (!serialInfoList.size() > 0)
    {
        qDebug() << "no serial ports found";
        return app.exec();
    }
    else
    {
        qDebug() << "List all serial ports";
        qDebug() << "---------------------";
        for (int i=0; i<serialInfoList.size(); i++)
        {
            QSerialPortInfo serialInfo = serialInfoList.at(i);
            qDebug() << "port: " << i;
            qDebug() << "  name:          " << serialInfo.portName();
            qDebug() << "  description:   " << serialInfo.description();
            qDebug() << "  manufacturer:  " << serialInfo.manufacturer();
            //qDebug() << "  serial Number: " << serialInfo.serialNumber();
            if (serialInfo.hasVendorIdentifier())
            {
                qDebug() << "  vendorId:     " << serialInfo.vendorIdentifier();
            }
            if (serialInfo.hasProductIdentifier())
            {
                qDebug() << "  producId:     " << serialInfo.productIdentifier();
            }
            qDebug();
        }
    }

    qDebug() << "opening panels controller";
    QSerialPortInfo serialInfo = serialInfoList.at(0);
    bias::PanelsController pcontrol(serialInfo);
    bool isOpen = pcontrol.open();
    if (!isOpen)
    {
        qDebug() << "  unable to open device";
        return app.exec();
    }
    qDebug() << "  device opened";

     if (true)
    {
        qDebug() << "  blink led";
        pcontrol.blinkLED();
        QThread::msleep(4000);
    }

    // Test allOn, allOff 
     if (true)
    {
        for (int i=0; i<5; i++)
        {
            qDebug() << "  all on";
            pcontrol.allOn();
            QThread::msleep(500);
            qDebug() << "  all off";
            pcontrol.allOff();
            QThread::msleep(500);
        }
    }

    // Test setToGrayLevel
     if (true)
    {
        for (int i=0; i<2; i++)
        {
            for (int j=1; j<bias::PanelsController::NUM_GRAY_LEVEL; j++)
            {
                qDebug() << "  set to gray level = " << j;
                pcontrol.setToGrayLevel(j);
                QThread::msleep(100);
            }
            for (int j=bias::PanelsController::NUM_GRAY_LEVEL-1; j>=0; j--) 
            {
                qDebug() << "  set to gray level = " << j;
                pcontrol.setToGrayLevel(j);
                QThread::msleep(100);
            }
        }
    }

    // Test setConfigID
     if (true)
    {
        int id = 1;
        qDebug() << "  set config id = " << id;
        pcontrol.setConfigID(id);
        QThread::msleep(1000);
    }


    // Test setPatternID 
    if (true)
    {
        int id = 1;
        qDebug() << "  set pattern id = " << id;
        pcontrol.setPatternID(id);
        QThread::msleep(1000);
    }

    // Test start/stop
     if (true)
    {
        qDebug() << "  start";
        pcontrol.start();
        QThread::msleep(5000);

        qDebug() << "  stop";
        pcontrol.stop();
        QThread::msleep(1000);
    }

    // Test controller reset
     if (true)
    {
        qDebug() << "  reset controller";
        pcontrol.reset(true);

        qDebug() << "  allOn";
        pcontrol.allOn();
        QThread::msleep(1000);
        qDebug() << "  allOff";
        pcontrol.allOff();
        QThread::msleep(1000);
    }

    // Test show bus number
     if (true)
    {
        qDebug() << "  show bus number";
        pcontrol.showBusNumber();
        QThread::msleep(1000);
    }


    // Test set position (not sure how this works??)
     if (false)
    {
        qDebug() << "  test set position";
        pcontrol.stop();
        for (int i=0;i<100;i++)
        {
            qDebug() << "  i = " << i;
            pcontrol.setPosition(i,0);
            pcontrol.start();
            QThread::msleep(500);
            pcontrol.stop();
        }
        pcontrol.stop();
    }

    // Test set gainAndOffset
     if (true)
    {
        qDebug() << "  test set gain and offset";
        pcontrol.setPatternID(1);
        pcontrol.start();
        pcontrol.setGainAndBias(0,0,0,0);

        qDebug() << "  test x gain";
        QThread::msleep(2000);
        pcontrol.setGainAndBias(10,0,0,0);
        QThread::msleep(2000);
        pcontrol.setGainAndBias(-10,0,0,0);
        QThread::msleep(2000);
        pcontrol.setGainAndBias(0,0,0,0);
        QThread::msleep(500);

        qDebug() << "  test y gain";
        pcontrol.setGainAndBias(0,0,10,0);
        QThread::msleep(2000);
        pcontrol.setGainAndBias(0,0,-10,0);
        QThread::msleep(2000);
        pcontrol.setGainAndBias(0,0,0,0);
        QThread::msleep(500);

        qDebug() << "  test x offset";
        pcontrol.setGainAndBias(0,10,0,0);
        QThread::msleep(2000);
        pcontrol.setGainAndBias(0,-10,0,0);
        QThread::msleep(2000);
        pcontrol.setGainAndBias(0,0,0,0);
        QThread::msleep(500);

        qDebug() << "  test y offset";
        pcontrol.setGainAndBias(0,0,0,10);
        QThread::msleep(2000);
        pcontrol.setGainAndBias(0,0,0,-10);
        QThread::msleep(2000);
        pcontrol.setGainAndBias(0,0,0,0);
        QThread::msleep(500);

        pcontrol.stop();
    }

    // Test set offsets again
    if (true)
    {
        pcontrol.setPatternID(1);
        qDebug() << "  testing y offset";
        int8_t n = 30;
        for (int8_t i=0; i<n; i++)
        {
            pcontrol.stop();
            pcontrol.setGainAndBias(0,0,0,i);
            pcontrol.start();
            QThread::msleep(200);
        }
        for (int8_t i=n; i>-n; i--)
        {
            pcontrol.stop();
            pcontrol.setGainAndBias(0,0,0,i);
            pcontrol.start();
            QThread::msleep(200);
        }
        for (int8_t i=-n; i<1; i++)
        {
            pcontrol.stop();
            pcontrol.setGainAndBias(0,0,0,i);
            pcontrol.start();
            QThread::msleep(200);
        }
        pcontrol.stop();
    }

    pcontrol.close();
    qDebug() << "Press Ctl-C to exit";
    return app.exec();
}
