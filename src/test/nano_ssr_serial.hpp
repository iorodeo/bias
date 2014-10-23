#ifndef NANO_SSR_SERIAL_HPP 
#define NANO_SSR_SERIAL_HPP
#include <QSerialPort>

namespace bias
{

    class NanoSSRSerial : public QSerialPort
    {
        enum SerialCmdId 
        {
            CMD_ID_STOP=0,           
            CMD_ID_START, 
            CMD_ID_START_ALL,
            CMD_ID_STOP_ALL, 
            CMD_ID_SET_PERIOD,
            CMD_ID_GET_PERIOD,
            CMD_ID_SET_NUM_PULSE,
            CMD_ID_GET_NUM_PULSE,
            CMD_ID_GET_RUNNING
        };

        public:
            static const QSerialPort::BaudRate DEFAULT_BAUDRATE;
            static const QSerialPort::DataBits DEFAULT_DATABITS;
            static const QSerialPort::FlowControl DEFAULT_FLOWCONTROL;
            static const QSerialPort::Parity DEFAULT_PARITY;
            static const QSerialPort::StopBits DEFAULT_STOPBITS;
            static const unsigned long DEFAULT_RESET_SLEEP_DT;
            static const int DEFAULT_WAITFOR_TIMEOUT;
            static const int NUM_CHANNELS;
            static const int RSP_BUFFER_SIZE;

            NanoSSRSerial(QObject *parent=Q_NULLPTR);
            NanoSSRSerial(const QSerialPortInfo &portInfo, QObject *parent=Q_NULLPTR);

            bool open(bool sleepForReset=true);
            bool isRunning(int chan);

            void start(int chan);
            void stop(int chan);
            void startAll();
            void stopAll();

            void setPeriod(int chan, int period);
            int getPeriod();
            
            void setNumPulse(int chan, int period);
            int getNumPulse(int chan);

        protected:
            int waitForTimeout_;
            unsigned long resetSleepDt_;

            void initialize();
            void writeCmd(QByteArray cmd);
            QByteArray writeCmdGetRsp(QByteArray rsp);
            bool isChanInRange(int chan);
    };

}

#endif
