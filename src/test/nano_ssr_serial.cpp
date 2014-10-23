#include "nano_ssr_serial.hpp"
#include <iostream>
#include <QThread>
#include <QDebug>
#include <QSerialPortInfo>

namespace bias
{
    // Static constants
    // ----------------------------------------------------------------------------------
    const QSerialPort::BaudRate NanoSSRSerial::DEFAULT_BAUDRATE = QSerialPort::Baud9600; 
    const QSerialPort::DataBits NanoSSRSerial::DEFAULT_DATABITS = QSerialPort::Data8;
    const QSerialPort::FlowControl NanoSSRSerial::DEFAULT_FLOWCONTROL = QSerialPort::NoFlowControl;
    const QSerialPort::Parity NanoSSRSerial::DEFAULT_PARITY = QSerialPort::NoParity;
    const QSerialPort::StopBits NanoSSRSerial::DEFAULT_STOPBITS = QSerialPort::OneStop;
    const unsigned long NanoSSRSerial::DEFAULT_RESET_SLEEP_DT = 2500;
    const int NanoSSRSerial::DEFAULT_WAITFOR_TIMEOUT = 500;
    const int NanoSSRSerial::NUM_CHANNELS = 8;
    const int NanoSSRSerial::RSP_BUFFER_SIZE = 100;

    // Public methods
    // ----------------------------------------------------------------------------------
    NanoSSRSerial::NanoSSRSerial(QObject *parent) : QSerialPort(parent) 
    {
        initialize();
    };

    NanoSSRSerial::NanoSSRSerial(const QSerialPortInfo &portInfo, QObject *parent) 
        : QSerialPort(portInfo,parent) 
    {
        initialize();
    };

    bool NanoSSRSerial::open(bool sleepForReset)
    {
        bool isOpen = QSerialPort::open(QIODevice::ReadWrite);
        if (isOpen && sleepForReset)
        {
            QThread::msleep(resetSleepDt_);
        }
        return isOpen;
    }

    bool NanoSSRSerial::isRunning(int chan)
    {
        bool isRunningVal = false;
        if (isChanInRange(chan)) 
        { 
            QByteArray cmd;
            cmd.append(QString("[%1,%2]\n").arg(CMD_ID_GET_RUNNING).arg(chan));
            QByteArray rsp = writeCmdGetRsp(cmd).trimmed();
            if (rsp.size() > 0)
            {
                bool ok;
                int rspInt = rsp.toInt(&ok);
                if (ok)
                {
                    isRunningVal = bool(rspInt);
                }
            }
            qDebug() << rsp << rsp.size();
        }
        return isRunningVal;
    }

    void NanoSSRSerial::start(int chan)
    {
        if (!isChanInRange(chan)) 
        { 
            return; 
        }
        QByteArray cmd;
        cmd.append(QString("[%1,%2]\n").arg(CMD_ID_START).arg(chan));
        writeCmd(cmd);
    }


    void NanoSSRSerial::startAll()
    {
        QByteArray cmd;  
        cmd.append(QString("[%1]\n").arg(CMD_ID_START_ALL));
        writeCmd(cmd);
    }

    void NanoSSRSerial::stopAll()
    {
        QByteArray cmd;
        cmd.append(QString("[%1]\n").arg(CMD_ID_STOP_ALL));
        writeCmd(cmd);
    }


    // Protected methods
    // ----------------------------------------------------------------------------------
    void NanoSSRSerial::initialize()
    { 
        setBaudRate(DEFAULT_BAUDRATE); 
        setDataBits(DEFAULT_DATABITS); 
        setFlowControl(DEFAULT_FLOWCONTROL); 
        setParity(DEFAULT_PARITY); 
        setStopBits(DEFAULT_STOPBITS); 
        waitForTimeout_ = DEFAULT_WAITFOR_TIMEOUT;
        resetSleepDt_ = DEFAULT_RESET_SLEEP_DT;
    }

    void NanoSSRSerial::writeCmd(QByteArray cmd)
    {
        write(cmd);
        waitForBytesWritten(waitForTimeout_);
    }

    QByteArray NanoSSRSerial::writeCmdGetRsp(QByteArray cmd)
    {
        QByteArray rsp;
        char buffer[RSP_BUFFER_SIZE];
        writeCmd(cmd);
        waitForReadyRead(waitForTimeout_);
        qint64 numBytes = -1;

        for (int i=0; i<100; i++)
        {
            qDebug() << "i: " << i << ", bytes: " << bytesAvailable();
            if (canReadLine())
            {
                //numBytes = readLine(buffer,RSP_BUFFER_SIZE);
                break;
            }
        }

        if (numBytes > 0)
        {
            rsp.append(buffer);
        }
        else
        {
            qDebug() << "readAll: " << readAll();
        }
        return rsp;
    }

    bool NanoSSRSerial::isChanInRange(int chan)
    {
        if ((chan < 0) || (chan >= NUM_CHANNELS))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

};
