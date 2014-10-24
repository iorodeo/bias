#include "nano_ssr_pulse.hpp"
#include <iostream>
#include <QThread>
#include <QDebug>
#include <QTime>
#include <QSerialPortInfo>

namespace bias
{
    // Static constants
    // ----------------------------------------------------------------------------------
    const QSerialPort::BaudRate NanoSSRPulse::DEFAULT_BAUDRATE = QSerialPort::Baud9600; 
    const QSerialPort::DataBits NanoSSRPulse::DEFAULT_DATABITS = QSerialPort::Data8;
    const QSerialPort::FlowControl NanoSSRPulse::DEFAULT_FLOWCONTROL = QSerialPort::NoFlowControl;
    const QSerialPort::Parity NanoSSRPulse::DEFAULT_PARITY = QSerialPort::NoParity;
    const QSerialPort::StopBits NanoSSRPulse::DEFAULT_STOPBITS = QSerialPort::OneStop;
    const unsigned long NanoSSRPulse::DEFAULT_RESET_SLEEP_DT = 2500;
    const int NanoSSRPulse::DEFAULT_WAITFOR_TIMEOUT = 500;
    const int NanoSSRPulse::NUM_CHANNELS = 8;
    const int NanoSSRPulse::RSP_BUFFER_SIZE = 100;
    const int NanoSSRPulse::MAX_WRITE_CNT = 10;

    // Public methods
    // ----------------------------------------------------------------------------------
    NanoSSRPulse::NanoSSRPulse(QObject *parent) : QSerialPort(parent) 
    {
        initialize();
    };

    NanoSSRPulse::NanoSSRPulse(const QSerialPortInfo &portInfo, QObject *parent) 
        : QSerialPort(portInfo,parent) 
    {
        initialize();
    };

    bool NanoSSRPulse::open(bool sleepForReset)
    {
        bool isOpen = QSerialPort::open(QIODevice::ReadWrite);
        if (isOpen && sleepForReset)
        {
            QThread::msleep(resetSleepDt_);
        }
        readAll();
        return isOpen;
    }

    bool NanoSSRPulse::isRunning(int chan)
    {
        bool isRunningVal = false;
        if (isChanInRange(chan)) 
        { 
            QByteArray cmd;
            QByteArray rsp;

            cmd.append(QString("[%1,%2]\n").arg(CMD_ID_GET_RUNNING).arg(chan));
            bool ok = writeCmdGetRsp(cmd, rsp);
            rsp = rsp.trimmed();
            if (rsp.size() > 0)
            {
                bool ok;
                int rspInt = rsp.toInt(&ok);
                if (ok)
                {
                    isRunningVal = bool(rspInt);
                }
            }
        }
        return isRunningVal;
    }

    bool NanoSSRPulse::isRunning()
    {
        bool isRunningVal = false;
        for (int i=0; i<NUM_CHANNELS; i++)
        {
            isRunningVal |= isRunning(i);
        }
        return isRunningVal;
    }

    bool NanoSSRPulse::start(int chan)
    {
        if (!isChanInRange(chan)) 
        { 
            return false; 
        }
        QByteArray cmd;
        cmd.append(QString("[%1,%2]\n").arg(CMD_ID_START).arg(chan));
        return writeCmd(cmd);
    }


    bool NanoSSRPulse::startAll()
    {
        QByteArray cmd;  
        cmd.append(QString("[%1]\n").arg(CMD_ID_START_ALL));
        return writeCmd(cmd);
    }

    bool NanoSSRPulse::stopAll()
    {
        QByteArray cmd;
        cmd.append(QString("[%1]\n").arg(CMD_ID_STOP_ALL));
        return writeCmd(cmd);
    }

    bool NanoSSRPulse::setPeriod(int chan,int period)
    {
        if (!isChanInRange(chan))
        {
            return false;
        }
        if (period < 0)
        {
            return false; 
        }
        QByteArray cmd;
        cmd.append(QString("[%1,%2,%3]\n").arg(CMD_ID_SET_PERIOD).arg(chan).arg(period));
        return writeCmd(cmd);
    } 

    bool NanoSSRPulse::getPeriod(int chan, int &period)
    {
        bool ok = false;
        if (isChanInRange(chan))
        {
            QByteArray cmd;
            QByteArray rsp;
            cmd.append(QString("[%1,%2]\n").arg(CMD_ID_GET_PERIOD).arg(chan));
            ok = writeCmdGetRsp(cmd,rsp);
            rsp = rsp.trimmed();
            if (rsp.size() > 0)
            {
                period = rsp.toInt(&ok);
            }
        }
        return ok;
    }

    bool NanoSSRPulse::setNumPulse(int chan, int numPulse)
    {
        if (!isChanInRange(chan))
        {
            return false;
        }
        if (numPulse < 0)
        {
            return false; 
        }
        QByteArray cmd;
        cmd.append(QString("[%1,%2,%3]\n").arg(CMD_ID_SET_NUM_PULSE).arg(chan).arg(numPulse));
        return writeCmd(cmd);
    }

    bool NanoSSRPulse::getNumPulse(int chan, int &numPulse)
    {
        bool ok = false;
        if (isChanInRange(chan))
        {
            QByteArray cmd;
            QByteArray rsp;
            cmd.append(QString("[%1,%2]\n").arg(CMD_ID_GET_NUM_PULSE).arg(chan));
            ok = writeCmdGetRsp(cmd,rsp);
            rsp = rsp.trimmed();
            if (rsp.size() > 0)
            {
                numPulse = rsp.toInt(&ok);
            }
        }
        return ok;
    }

    // Protected methods
    // ----------------------------------------------------------------------------------
    void NanoSSRPulse::initialize()
    { 
        setBaudRate(DEFAULT_BAUDRATE); 
        setDataBits(DEFAULT_DATABITS); 
        setFlowControl(DEFAULT_FLOWCONTROL); 
        setParity(DEFAULT_PARITY); 
        setStopBits(DEFAULT_STOPBITS); 
        waitForTimeout_ = DEFAULT_WAITFOR_TIMEOUT;
        resetSleepDt_ = DEFAULT_RESET_SLEEP_DT;
    }

    bool NanoSSRPulse::writeCmd(QByteArray cmd)
    {
        bool ok = false;
        int cnt = 0;
        while ( (!ok) && (cnt < MAX_WRITE_CNT)) 
        {
            write(cmd);
            ok = waitForBytesWritten(waitForTimeout_);
            cnt++;
        }
        return ok;
    }

    bool NanoSSRPulse::writeCmdGetRsp(QByteArray cmd, QByteArray &rsp)
    {
        bool ok = false;
        if (writeCmd(cmd))
        {
            if (waitForReadyRead(waitForTimeout_))
            {
                char buffer[RSP_BUFFER_SIZE];
                qint64 numBytes = readLine(buffer,RSP_BUFFER_SIZE);
                if (numBytes > 0)
                {
                    rsp.append(buffer);
                    ok = true;
                }
            }
        }
        return ok;
    }

    bool NanoSSRPulse::isChanInRange(int chan)
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
