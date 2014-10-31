#ifndef NANO_SSR_PULSE_HPP 
#define NANO_SSR_PULSE_HPP
#include <QSerialPort>

namespace bias
{

    class NanoSSRPulse : public QSerialPort
    {
        enum SerialCmdID 
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
            static const int MAX_WRITE_CNT;

            NanoSSRPulse(QObject *parent=Q_NULLPTR);
            NanoSSRPulse(const QSerialPortInfo &portInfo, QObject *parent=Q_NULLPTR);

            bool open(bool sleepForReset=true);
            bool isRunning(int chan);
            bool isRunning();

            bool start(int chan);
            bool stop(int chan);
            bool startAll();
            bool stopAll();

            bool setPeriod(int chan, int period);
            bool getPeriod(int chan, int &period);
            
            bool setNumPulse(int chan, int num);
            bool getNumPulse(int chan, int &num);

        protected:
            int waitForTimeout_;
            unsigned long resetSleepDt_;

            void initialize();
            bool writeCmd(QByteArray cmd);
            bool writeCmdGetRsp(QByteArray cmd, QByteArray &rsp);
            bool isChanInRange(int chan);
    };

}

#endif
