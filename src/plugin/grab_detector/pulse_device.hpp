#ifndef PULSE_DEVICE_HPP
#define PULSE_DEIVE_HPP
#include <QSerialPort>
#include <QVector>

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
            static const int MAX_READ_CNT = 50;

            static const int CMD_ID_START_PULSE = 1;
            static const int CMD_ID_STOP_PULSE = 2;
            static const int CMD_ID_SET_PULSE_LENGTH = 3;
            static const int CMD_ID_GET_PULSE_LENGTH = 4;
            static const int CMD_ID_SET_OUTPUT_PIN = 5;
            static const int CMD_ID_GET_OUTPUT_PIN = 6;
            static const int CMD_ID_GET_ALLOWED_OUTPUT_PIN = 7;

            PulseDevice(QObject *parent=Q_NULLPTR);
            PulseDevice(const QSerialPortInfo &portInfo, QObject *parent=Q_NULLPTR);

            bool open(bool sleepForReset=true);
            bool startPulse();
            bool stopPulse();
            bool setPulseLength(unsigned long pulseLength);
            unsigned long getPulseLength(bool *ok);
            int getOutputPin(bool *ok);
            bool setOutputPin(int outputPin);
            QVector<int> getAllowedOutputPin(bool *ok);


        protected:

            int waitForTimeout_;
            unsigned long resetSleepDt_;

            void initialize();
            bool writeCmd(QByteArray cmd);
            bool writeCmdGetRsp(QByteArray cmd, QByteArray &rsp);
    };

}

#endif
