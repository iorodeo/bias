#include "pulse_device.hpp"
#include <QThread>
#include <QDebug>

namespace bias
{

    // Public methods
    // ----------------------------------------------------------------------------------
    PulseDevice::PulseDevice(QObject *parent) : QSerialPort(parent) 
    {
        initialize();
    };


    PulseDevice::PulseDevice(const QSerialPortInfo &portInfo, QObject *parent) 
        : QSerialPort(portInfo,parent) 
    {
        initialize();
    };


    bool PulseDevice::open(bool sleepForReset)
    {
        bool isOpen = QSerialPort::open(QIODevice::ReadWrite);
        if (isOpen && sleepForReset)
        {
            QThread::msleep(resetSleepDt_);
            readAll();
        }
        setBaudRate(DEFAULT_BAUDRATE); 
        setDataBits(DEFAULT_DATABITS); 
        setFlowControl(DEFAULT_FLOWCONTROL); 
        setParity(DEFAULT_PARITY); 
        setStopBits(DEFAULT_STOPBITS); 
        return isOpen;
    }


    bool PulseDevice::startPulse()
    {
        QByteArray cmd;
        cmd.append(QString("[%1,]\n").arg(CMD_ID_START_PULSE));
        return writeCmd(cmd);
    }


    bool PulseDevice::stopPulse()
    {
        QByteArray cmd;
        cmd.append(QString("[%1,]\n").arg(CMD_ID_STOP_PULSE));
        return writeCmd(cmd);
    }


    bool PulseDevice::setPulseLength(unsigned long pulseLength)
    {
        QByteArray cmd;
        cmd.append(QString("[%1,%2]\n").arg(CMD_ID_SET_PULSE_LENGTH).arg(pulseLength));
        return writeCmd(cmd);
    }


    unsigned long PulseDevice::getPulseLength(bool *ok)
    {
        unsigned long pulseLength = 0;
        QByteArray cmd;
        cmd.append(QString("[%1,]\n").arg(CMD_ID_GET_PULSE_LENGTH));

        QByteArray rsp;
        bool rspOk = writeCmdGetRsp(cmd,rsp);

        *ok = false;
        if ( rspOk && (rsp.size() > 0) )
        {
            rsp = rsp.trimmed();

            bool convOk;
            pulseLength = rsp.toULong(&convOk);
            if (convOk)
            {
                *ok = true;
            }
        }
        return pulseLength;
    }

    int PulseDevice::getOutputPin(bool *ok)
    {
        int pin = 0;
        QByteArray cmd;
        cmd.append(QString("[%1,]\n").arg(CMD_ID_GET_OUTPUT_PIN));

        QByteArray rsp;
        bool rspOk = writeCmdGetRsp(cmd,rsp);

        *ok = false;
        if ( rspOk && (rsp.size() > 0))
        {
            rsp = rsp.trimmed();

            bool convOk;
            pin = rsp.toInt(&convOk);
            if (convOk)
            {
                *ok = true;
            }
        }
        return pin;
    }

    // Prottected methods
    // ------------------------------------------------------------------------
    void PulseDevice::initialize()
    {
        waitForTimeout_ = DEFAULT_WAITFOR_TIMEOUT;
        resetSleepDt_ = DEFAULT_RESET_SLEEP_DT;
    }


    bool PulseDevice::writeCmd(QByteArray cmd)
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


    bool PulseDevice::writeCmdGetRsp(QByteArray cmd, QByteArray &rsp)
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

}
