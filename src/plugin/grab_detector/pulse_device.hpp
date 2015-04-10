#ifndef PULSE_DEVICE_HPP
#define PULSE_DEIVE_HPP
#include <QSerialPort>

namespace bias
{

    class PulseDevice : public QSerialPort
    {
        public:

            
            static const QSerialPort::BaudRate DEFAULT_BAUDRATE = QSerialPort::Baud9600; 
            static const QSerialPort::DataBits DEFAULT_DATABITS = QSerialPort::Data8;
            static const QSerialPort::FlowControl DEFAULT_FLOWCONTROL = QSerialPort::NoFlowControl;
            static const QSerialPort::Parity DEFAULT_PARITY = QSerialPort::NoParity;
            static const QSerialPort::StopBits DEFAULT_STOPBITS = QSerialPort::OneStop;
            static const unsigned long DEFAULT_RESET_SLEEP_DT = 2500;
            static const int DEFAULT_WAITFOR_TIMEOUT = 500;
            static const int RSP_BUFFER_SIZE = 100;
            static const int MAX_WRITE_CNT = 10;

            static const int CMD_ID_START_PULSE = 1;
            static const int CMD_ID_STOP_PULSE = 2;
            static const int CMD_ID_SET_PULSE_LENGTH = 3;
            static const int CMD_ID_GET_PULSE_LENGTH = 4;

            PulseDevice(QObject *parent=Q_NULLPTR);
            PulseDevice(const QSerialPortInfo &portInfo, QObject *parent=Q_NULLPTR);

            bool open(bool sleepForReset=true);
            bool startPulse();
            bool stopPulse();
            bool setPulseLength(unsigned long pulseLength);
            bool getPulseLength(unsigned long &pulseLength);

        protected:

            int waitForTimeout_;
            unsigned long resetSleepDt_;

            void initialize();
            bool writeCmd(QByteArray cmd);
            bool writeCmdGetRsp(QByteArray cmd, QByteArray &rsp);
    };

}

#endif
