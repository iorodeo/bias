#include <iostream>
#include <QThread>
#include <QDebug>
#include <QList>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QCoreApplication>
#include "nano_ssr_pulse.hpp"


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
            qDebug() << "  serial Number: " << serialInfo.serialNumber();
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

    qDebug() << "opening nano SSR device";
    QSerialPortInfo serialInfo = serialInfoList.at(0);
    bias::NanoSSRPulse nanoSSR(serialInfo);
    bool isOpen = nanoSSR.open();
    if (!isOpen)
    {
        qDebug() << "  unable to open device";
        return app.exec();
    }
    qDebug() << "  device opened";

    // Period
    int period = -1;
    nanoSSR.getPeriod(0,period);
    qDebug() << "period: " << period;
    nanoSSR.setPeriod(0,100);
    nanoSSR.getPeriod(0,period);
    qDebug() << "period: " << period;

    // numPulse
    int numPulse = -1;
    nanoSSR.getNumPulse(0,numPulse);
    qDebug() << "numPulse: " << numPulse;
    nanoSSR.setNumPulse(0,20);
    nanoSSR.getNumPulse(0,numPulse);
    qDebug() << "numPulse: " << numPulse;


    nanoSSR.start(0);
    while (nanoSSR.isRunning(0))
    {
        qDebug() << "running";
        QThread::msleep(200);
    }

    nanoSSR.close();
    qDebug() << "Press Ctl-C to exit";
    return app.exec();

}
