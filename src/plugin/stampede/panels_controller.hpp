#ifndef PANELS_CONTROLLER_HPP 
#define PANELS_CONTROLLER_HPP
#include <QSerialPort>

namespace bias
{

    class PanelsController : public QSerialPort
    {
        public:

            static const unsigned int DEFAULT_BAUDRATE;
            static const QSerialPort::DataBits DEFAULT_DATABITS;
            static const QSerialPort::FlowControl DEFAULT_FLOWCONTROL;
            static const QSerialPort::Parity DEFAULT_PARITY;
            static const QSerialPort::StopBits DEFAULT_STOPBITS;
            static const int DEFAULT_WAITFOR_TIMEOUT;
            static const int MAX_WRITE_CNT;
            static const int RESET_SLEEP_DT;
            static const uint8_t NUM_GRAY_LEVEL;
            static const uint8_t NUM_ADC_CHANNEL;
            static const uint8_t NUM_DIO_CHANNEL;
            static const uint8_t NUM_LOOP_MODE;
            static const uint8_t MIN_PATTERN_ID;
            static const uint8_t MAX_PATTERN_ID;
            static const uint8_t MIN_CONFIG_ID;
            static const uint8_t MAX_CONFIG_ID;
            static const uint8_t MAX_ADDRESS;

            PanelsController(QObject *parent=Q_NULLPTR);
            PanelsController(const QSerialPortInfo &portInfo, QObject *parent=Q_NULLPTR);

            bool open();

            bool start();
            bool stop();
            bool startWithTrig();
            bool stopWithTrig();

            bool clearFlash();
            bool reset(bool sleep=false); 
            bool resetPanel(uint8_t addr);

            bool blinkLED();
            bool allOn();
            bool allOff();
            bool setToGrayLevel(uint8_t level);
            bool setPatternID(uint8_t id);
            bool setConfigID(uint8_t id);
            bool setLaserTrig(bool value);
            bool setIdentCompress(bool value);
            bool setQuietMode(bool value);
            bool setToControllerMode();
            bool setToPCDumpingMode();
            bool setPanelAddress(uint8_t addrCur, uint8_t addrNew);
            bool setTriggerRate(uint8_t rate);
            bool setLoopMode(uint8_t modeX, uint8_t modeY);
            bool setPosition(uint16_t x, uint16_t y);
            bool setGainAndBias(int8_t gainX, int8_t offsetX, int8_t gainY, int8_t offsetY);

            bool showBusNumber();
            bool display(uint8_t addr);

            bool runADCTest(uint8_t chan);
            bool runDIOTest(uint8_t chan);



        protected:

            int waitForTimeout_;

            void initialize();
            bool writeCmd(QByteArray cmd);
    };

}

#endif
