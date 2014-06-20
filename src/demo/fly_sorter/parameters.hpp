#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP
#include <string>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QFile>
#include <vector>
#include "blob_finder_param.hpp"
#include "rtn_status.hpp"
#include <opencv2/core/core.hpp>

extern const double PixelScaleFactor;

using namespace bias;

class StumpData 
{
    public:
        unsigned int channel;
        float threshold;
        float value;
        StumpData();
        StumpData(unsigned int chan, float thresh, float val);
};


class ClassifierParam
{
    public:
        std::vector<StumpData> stumpVector;
        float offset;
        QString fileName;

        ClassifierParam();
        RtnStatus loadFromFile(QString dirName=QString(""));
        QString toString();
        std::string toStdString();
        void print();
};


class FlySegmenterParam 
{
    public:
        ClassifierParam classifier;
        static const QString DEFAULT_CLASSIFIER_FILENAME;
        FlySegmenterParam();
};


class BinParam
{
    public:
        unsigned int numX;
        unsigned int numY;
        BinParam();
        BinParam(unsigned int numX_, unsigned int numY_);
};

class PixelFeatureVectorParam
{
    public:

        unsigned int gradNormRadius;
        static const unsigned int DEFAULT_GRAD_NORM_RADIUS;

        double gradNormConst;
        static const double DEFAULT_GRAD_NORM_CONST;

        unsigned int fillBndryErodeRadius;
        static const unsigned int DEFAULT_FILL_BNDRY_ERODE_RADIUS;

        std::vector<cv::Scalar> colorEdgeVector;
        static const std::vector<cv::Scalar> DEFAULT_COLOR_EDGE_VECTOR;
        static std::vector<cv::Scalar> createDefaultColorEdgeVector();

        std::vector<float> gradMagEdgeVector;     
        static const std::vector<float> DEFAULT_GRAD_MAG_EDGE_VECTOR;
        static std::vector<float> createDefaultGradMagEdgeVector();

        std::vector<float> gradOriCentVector;
        static const std::vector<float> DEFAULT_GRAD_ORI_CENT_VECTOR;
        static std::vector<float> createDefaultGradOriCentVector();

        std::vector<BinParam> binParam;
        static const std::vector<BinParam> DEFAULT_BIN_PARAM;
        static std::vector<BinParam> createDefaultBinParam();

        PixelFeatureVectorParam();
};


class HogPositionFitterParam 
{
    public:

        unsigned int closeRadius;
        static const unsigned int DEFAULT_CLOSE_RADIUS;

        unsigned int openArea;
        static const unsigned int DEFAULT_OPEN_AREA;

        unsigned int maxBodyArea;
        static const unsigned int DEFAULT_MAX_BODY_AREA;

        cv::Scalar fillValuesLUV;
        static const cv::Scalar DEFAULT_FILL_VALUES_LUV;

        double padBorder;
        static const double DEFAULT_PAD_BORDER;

        ClassifierParam orientClassifier;
        //static const ClassifierParam DEFAULT_ORIENT_CLASSIFIER_PARAM;
        //static ClassifierParam createDefaultOrientClassifierParam();
        static const QString DEFAULT_ORIENT_CLASSIFIER_FILENAME;

        PixelFeatureVectorParam pixelFeatureVector;

        HogPositionFitterParam();
};


class GenderSorterParam
{
    public:

        ClassifierParam genderClassifier;
        //static const ClassifierParam DEFAULT_GENDER_CLASSIFIER_PARAM;
        //static ClassifierParam createDefaultGenderClassifierParam();
        static const QString DEFAULT_GENDER_CLASSIFIER_FILENAME;

        double minConfidence;
        static const double DEFAULT_MIN_CONFIDENCE;

        GenderSorterParam();
};


class ImageGrabberParam
{
    public:
        float frameRate;
        static const float DEFAULT_FRAMERATE;
        static const float MINIMUM_FRAMERATE;
        static const float MAXIMUM_FRAMERATE;

        float gain;
        static const float DEFAULT_GAIN;
        static const float MINIMUM_GAIN;
        static const float MAXIMUM_GAIN;

        float shutter;
        static const float DEFAULT_SHUTTER;
        static const float MINIMUM_SHUTTER;
        static const float MAXIMUM_SHUTTER;

        unsigned int brightness;
        static const unsigned int DEFAULT_BRIGHTNESS;
        static const unsigned int MAXIMUM_BRIGHTNESS;

        QString captureMode;
        static const QStringList  ALLOWED_CAPTURE_MODES;

        QString captureInputFile;
        static const QString DEFAULT_CAPTURE_INPUT_FILE;

        QString captureInputDir;
        static const QString DEFAULT_CAPTURE_INPUT_DIR;

        // New 
        // -----------------------------------------------------------
        float gamma;
        static const float DEFAULT_GAMMA;
        static const float MINIMUM_GAMMA;
        static const float MAXIMUM_GAMMA;

        float saturation;
        static const float DEFAULT_SATURATION;
        static const float MINIMUM_SATURATION;
        static const float MAXIMUM_SATURATION;

        unsigned int whiteBalanceRed;
        unsigned int whiteBalanceBlue;
        static const unsigned int DEFAULT_WHITE_BALANCE_RED;
        static const unsigned int DEFAULT_WHITE_BALANCE_BLUE;
        static const unsigned int MINIMUM_WHITE_BALANCE;
        static const unsigned int MAXIMUM_WHITE_BALANCE;
        // -----------------------------------------------------------

        ImageGrabberParam();
        QVariantMap toMap();
        RtnStatus fromMap(QVariantMap paramMap);
};


class ServerParam
{ 
    public:

        bool enabled;
        static const bool DEFAULT_ENABLED;

        QString address;
        static const QString DEFAULT_ADDRESS;

        unsigned int port;
        static const unsigned int DEFAULT_PORT;

        ServerParam();
        QVariantMap toMap();
        RtnStatus fromMap(QVariantMap paramMap);
};


enum MotionDirection 
{
    MOTION_DIRECTION_X = 0,
    MOTION_DIRECTION_Y = 1
};
QString motionDirectionToString(MotionDirection motionDir);
MotionDirection motionDirectionFromString(QString motionDirString);

class IdentityTrackerParam
{
    public:

        float meanDx;
        static const float DEFAULT_MEAN_DX;

        float meanDy;
        static const float DEFAULT_MEAN_DY;

        float meanWidth;
        static const float DEFAULT_MEAN_WIDTH;

        float meanHeight;
        static const float DEFAULT_MEAN_HEIGHT;

        float stdDx;
        static const float DEFAULT_STD_DX;

        float stdDy;
        static const float DEFAULT_STD_DY;

        float stdWidth;
        static const float DEFAULT_STD_WIDTH;

        float stdHeight;
        static const float DEFAULT_STD_HEIGHT;

        float maxCost;
        static const float DEFAULT_MAX_COST;

        MotionDirection motionDirection;
        static const MotionDirection DEFAULT_MOTION_DIRECTION;

        IdentityTrackerParam();
        QVariantMap toMap();
        RtnStatus fromMap(QVariantMap paramMap);
};


class FlySorterParam
{
    public:
        
        ServerParam server;
        ImageGrabberParam imageGrabber;
        BlobFinderParam blobFinder;
        IdentityTrackerParam identityTracker;
        FlySegmenterParam flySegmenter;
        HogPositionFitterParam hogPositionFitter;
        GenderSorterParam genderSorter;


        FlySorterParam();
        QVariantMap toMap();
        QByteArray toJson();

        RtnStatus fromJson(QByteArray paramJson);
        RtnStatus fromMap(QVariantMap paramMap);
};



RtnStatus getFloatFromQVariantMap(QString key, QString mapName, QVariantMap map, float &value);
RtnStatus getStringFromQVariantMap(QString key, QString mapName, QVariantMap map, QString &value);
RtnStatus getMotionDirectionFromQVariantMap(QString key, QString mapName, QVariantMap map, MotionDirection &value);

#endif // #ifndef PARAMETERS_HPP
