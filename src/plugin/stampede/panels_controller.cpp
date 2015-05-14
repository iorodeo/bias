#include "panels_controller.hpp"
#include <iostream>
#include <QThread>
#include <QDebug>
#include <QTime>
#include <QSerialPortInfo>

namespace bias
{
    // Static constants
    // ----------------------------------------------------------------------------------
    const unsigned int PanelsController::DEFAULT_BAUDRATE = 921600; 
    const QSerialPort::DataBits PanelsController::DEFAULT_DATABITS = QSerialPort::Data8;
    const QSerialPort::FlowControl PanelsController::DEFAULT_FLOWCONTROL = QSerialPort::NoFlowControl;
    const QSerialPort::Parity PanelsController::DEFAULT_PARITY = QSerialPort::NoParity;
    const QSerialPort::StopBits PanelsController::DEFAULT_STOPBITS = QSerialPort::OneStop;
    const int PanelsController::RESET_SLEEP_DT = 9000;
    const int PanelsController::DEFAULT_WAITFOR_TIMEOUT = 500;
    const int PanelsController::MAX_WRITE_CNT = 10;
    const uint8_t PanelsController::NUM_GRAY_LEVEL = 16;
    const uint8_t PanelsController::NUM_ADC_CHANNEL = 8;
    const uint8_t PanelsController::NUM_DIO_CHANNEL = 8;
    const uint8_t PanelsController::NUM_LOOP_MODE = 5;
    const uint8_t PanelsController::MIN_PATTERN_ID = 1;
    const uint8_t PanelsController::MAX_PATTERN_ID = 99;
    const uint8_t PanelsController::MIN_CONFIG_ID = 1;
    const uint8_t PanelsController::MAX_CONFIG_ID = 99;
    const uint8_t PanelsController::MAX_ADDRESS = 127;

    // Public methods
    // ----------------------------------------------------------------------------------
    PanelsController::PanelsController(QObject *parent) : QSerialPort(parent) 
    { 
        initialize();
    };

    PanelsController::PanelsController(const QSerialPortInfo &portInfo, QObject *parent) 
        : QSerialPort(portInfo,parent) 
    { 
        initialize();
    };

    bool PanelsController::open()
    {
        // Do we need a sleed here??
        bool isOpen = QSerialPort::open(QIODevice::ReadWrite);
        if (isOpen)
        {
            setBaudRate(DEFAULT_BAUDRATE); 
            setDataBits(DEFAULT_DATABITS); 
            setFlowControl(DEFAULT_FLOWCONTROL); 
            setParity(DEFAULT_PARITY); 
            setStopBits(DEFAULT_STOPBITS); 
            waitForTimeout_ = DEFAULT_WAITFOR_TIMEOUT;
            readAll();
        }
        return isOpen;
    }

    bool PanelsController::blinkLED()
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0x50));
        return writeCmd(cmd);
    }

    bool PanelsController::allOn()
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0xff));
        return writeCmd(cmd);
    }

    bool PanelsController::allOff()
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0x00));
        return writeCmd(cmd);
    }

    bool PanelsController::setToGrayLevel(uint8_t level)
    {
        bool ok = false;
        if (level < NUM_GRAY_LEVEL)
        {
            QByteArray cmd;
            cmd.append(char(0x01));
            cmd.append(char(0x90 + level));
            ok = writeCmd(cmd);
        }
        return ok;
    }

    bool PanelsController::setPatternId(uint8_t id)
    {
        bool ok = false;
        if ((id >= MIN_PATTERN_ID) && (id <= MAX_PATTERN_ID))
        {
            QByteArray cmd;
            cmd.append(char(0x02));
            cmd.append(char(0x03));
            cmd.append(char(id));
            ok = writeCmd(cmd);
        }
        return ok; 
    }

    bool PanelsController::setConfigId(uint8_t id)
    {
        bool ok = false;
        if ((id >= MIN_CONFIG_ID) && (id <= MAX_CONFIG_ID))
        {
            QByteArray cmd;
            cmd.append(char(0x02));
            cmd.append(char(0x09));
            cmd.append(char(id));
            ok = writeCmd(cmd);
        }
        return ok; 
    }

    bool PanelsController::start()
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0x20));
        return writeCmd(cmd);
    }

    bool PanelsController::stop()
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0x30));
        return writeCmd(cmd);
    }

    bool PanelsController::startWithTrig()
    {
        // What does this do ... ?
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0x25));
        return writeCmd(cmd);
    }

    bool PanelsController::stopWithTrig()
    {
        // What does this do ... ?
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0x35));
        return writeCmd(cmd);
    }

    bool PanelsController::clearFlash()
    {
        // What does this do ... ?
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0xf0));
        return writeCmd(cmd);
    }

    bool PanelsController::reset(bool sleep)
    {
        // Resets the controller
        QByteArray cmd;
        cmd.append(char(0x1));
        cmd.append(char(0x60));
        bool ok =  writeCmd(cmd);
        if (sleep)
        {
            QThread::msleep(RESET_SLEEP_DT);
        }
        return ok;
    }

    bool PanelsController::resetPanel(uint8_t addr)
    {
        // Resets panels with given address (I think) 
        bool ok = false;
        if (addr<=MAX_ADDRESS)
        {
            QByteArray cmd;
            cmd.append(char(0x02));
            cmd.append(char(0x01));
            cmd.append(char(addr));
            ok = writeCmd(cmd);
        }
        return ok;
    }

    bool PanelsController::setLaserTrig(bool value)
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        if (value)
        {
            cmd.append(char(0x10));
        }
        else
        {
            cmd.append(char(0x11));
        }
        return writeCmd(cmd);
    }

    bool PanelsController::setIdentCompress(bool value)
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        if (value)
        {
            cmd.append(char(0x12));
        }
        else
        {
            cmd.append(char(0x13));
        }
        return writeCmd(cmd);
    } 

    bool PanelsController::setQuietMode(bool value)
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        if (value)
        {
            cmd.append(char(0x17));
        }
        else
        {
            cmd.append(char(0x18));
        }
        return writeCmd(cmd);
    }

    bool PanelsController::setPanelAddress(uint8_t addrCur, uint8_t addrNew)
    {
        bool ok = false;
        if ((addrCur <= MAX_ADDRESS)) 
        {
            if ((addrNew <= MAX_ADDRESS))
            {
                QByteArray cmd;
                cmd.append(char(0x3));
                cmd.append(char(0xff));
                cmd.append(char(addrCur));
                cmd.append(char(addrNew));
                ok = writeCmd(cmd);
            }
        }
        return ok;
    }

    bool PanelsController::setToControllerMode()
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0x21));
        return writeCmd(cmd);
    }

    bool PanelsController::setToPCDumpingMode()
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0x22));
        return writeCmd(cmd);
    }

    bool PanelsController::setTriggerRate(uint8_t rate)
    { 
        QByteArray cmd;
        cmd.append(char(0x02));
        cmd.append(char(0x06));
        cmd.append(char(rate));
        return writeCmd(cmd);
    }

    bool PanelsController::setLoopMode(uint8_t modeX, uint8_t modeY)
    {
        bool ok = false;
        if ((modeX < NUM_LOOP_MODE) && (modeY < NUM_LOOP_MODE))
        {
            QByteArray cmd;
            cmd.append(char(0x03));
            cmd.append(char(0x10));
            cmd.append(char(modeX));
            cmd.append(char(modeY));
            ok = writeCmd(cmd);
        }
        return ok;
    }

    bool PanelsController::setPosition(uint16_t x, uint16_t y)
    {
        uint8_t xUpper = uint8_t((x>>8)&0xff);
        uint8_t xLower = uint8_t(x&0xff0);
        uint8_t yUpper = uint8_t((y>>8)&0xff);
        uint8_t yLower = uint8_t(x&0xff);
        QByteArray cmd;
        cmd.append(char(0x05));
        cmd.append(char(0x70));
        cmd.append(char(xUpper));
        cmd.append(char(xLower));
        cmd.append(char(yUpper));
        cmd.append(char(yLower));
        return writeCmd(cmd);
    }

    bool PanelsController::setGainAndBias(int8_t gainX, int8_t offsetX, int8_t gainY, int8_t offsetY)
    {
        QByteArray cmd;
        cmd.append(char(0x05));
        cmd.append(char(0x71));
        cmd.append(char(gainX));
        cmd.append(char(offsetX));
        cmd.append(char(gainY));
        cmd.append(char(offsetY));
        return writeCmd(cmd);
    }

    bool PanelsController::showBusNumber()
    {
        QByteArray cmd;
        cmd.append(char(0x01));
        cmd.append(char(0x16));
        return writeCmd(cmd);
    }

    bool PanelsController::display(uint8_t addr)
    {
        bool ok = false;
        if (addr <= MAX_ADDRESS)
        {
            QByteArray cmd;
            cmd.append(char(0x02));
            cmd.append(char(0x02));
            ok = writeCmd(cmd);
        }
        return ok;
    } 
    
    bool PanelsController::runADCTest(uint8_t chan)
    {
        bool ok = false;
        if (chan < NUM_ADC_CHANNEL)
        {
            QByteArray cmd;
            cmd.append(char(0x02));
            cmd.append(char(0x04));
            cmd.append(char(chan));
            ok = writeCmd(cmd);
        }
        return ok;

    }

    bool PanelsController::runDIOTest(uint8_t chan)
    {
        bool ok = false;
        if (chan < NUM_DIO_CHANNEL)
        {
            QByteArray cmd;
            cmd.append(char(0x02));
            cmd.append(char(0x05));
            cmd.append(char(chan));
            ok = writeCmd(cmd);
        }
        return ok;
    }


    // Protected methods
    // ----------------------------------------------------------------------------------
    void PanelsController::initialize()
    { 
    }

    bool PanelsController::writeCmd(QByteArray cmd)
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
}
