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




    //// Connect to serial port
    //int portNum = 0;
    //qDebug() << "Connecting to port number: " << portNum;
    //QSerialPort serial; 

    //serial.setPort(serialInfoList.at(0));
    //serial.setBaudRate(QSerialPort::Baud115200);
    //serial.setDataBits(QSerialPort::Data8);
    //serial.setFlowControl(QSerialPort::NoFlowControl);
    //serial.setParity(QSerialPort::NoParity);
    //serial.setStopBits(QSerialPort::OneStop);

    //bool isOpen = serial.open(QIODevice::ReadWrite);
    //if (!isOpen)
    //{
    //    qDebug() << "  unable to open serial port";
    //    return app.exec();

    //}
    //qDebug() << "  serial port opened";
    //qDebug() << "  sleeping for rest";
    //QThread::sleep(3);
    //qDebug() << "  read/write data";

    //for (int i=0; i<100; i++)
    //{
    //    QByteArray msg;  
    //    msg.append(QString("hello # %1").arg(i));
    //    qDebug() << "  msg: " << msg;
    //    msg.append('\n');
    //    serial.write(msg);
    //    serial.waitForBytesWritten(500);

    //    QByteArray buffer(100,'0');
    //    serial.waitForReadyRead(500);
    //    qint64 numBytes = serial.readLine(buffer.data(),buffer.size());
    //    qDebug() << "  rsp: " << buffer;
    //    qDebug();
    //}

    //serial.close();
    return app.exec();

}
