#ifndef FLY_SORTER_WINDOW_HPP
#define FLY_SORTER_WINDOW_HPP

#include "ui_fly_sorter_window.h"
#include "parameters.hpp"
#include "image_grabber.hpp"
#include "blob_finder.hpp"
#include "identity_tracker.hpp"
#include "fly_segmenter.hpp"
#include "hog_position_fitter.hpp"
#include "gender_sorter.hpp"
#include "rtn_status.hpp"
#include <memory>
#include <QCloseEvent>
#include <QMainWindow>
#include <QPointer>
#include <QMap>
#include <QList>
#include <QDir>
#include <QVariantMap>
#include <opencv2/core/core.hpp>

// Debug
// ------------------------------
#include <fstream>
// ------------------------------

class QThreadPool;
class ImageGrabber;
class QTimer;
class QNetworkAccessManager;
class QNetworkReply;
class QVarianMap;
class QByteArray;
class ExtCtlHttpServer;


enum TrainingDataMode 
{
    TRAINING_DATA_MODE_SINGLE=0,
    TRAINING_DATA_MODE_BATCH
};



class FlySorterWindow : public QMainWindow, private Ui::FlySorterWindow
{
    Q_OBJECT

    public:

        FlySorterWindow(QWidget *parent=0);
        bool isRunning();
        RtnStatus startRunning();
        RtnStatus stopRunning();
        RtnStatus getStatus(QVariantMap &statusMap);

    signals:

        void stopCapture();
        void dumpCameraProperties();

    protected:

        void resizeEvent(QResizeEvent *event);
        void closeEvent(QCloseEvent *event);
        void showEvent(QShowEvent *event);

    private slots:

        void startPushButtonClicked();
        void reloadPushButtonClicked();
        void httpOutputCheckBoxChanged(int state);
        void trainingDataCheckBoxChanged(int state);
        void newImage(ImageData imageData);
        void updateDisplayOnTimer(); 
        void networkAccessManagerFinished(QNetworkReply *reply);
        void cameraSetupError(QString errorMsg);
        void imageGrabberFileReadError(QString errorMsg);
        void OnImageCaptureStopped();
        void actionDumpCameraPropsTriggered();

    private:

        bool running_;
        bool stopRunningFlag_;

        FlySorterParam param_;
        QVariantMap paramMap_;

        float displayFreq_;
        QPointer<QThreadPool> threadPoolPtr_;
        QPointer<ImageGrabber> imageGrabberPtr_;
        QPointer<QTimer> displayTimerPtr_;
        QPointer<QNetworkAccessManager> networkAccessManagerPtr_;
        QPixmap previewPixmapOrig_;
        QPixmap thresholdPixmapOrig_;
        ImageData imageData_;
        unsigned int httpRequestErrorCount_;
        QString parameterFileName_;

        BlobFinder blobFinder_;
        BlobFinderData blobFinderData_;

        IdentityTracker identityTracker_;

        FlySegmenter flySegmenter_;
        FlySegmenterData flySegmenterData_;

        HogPositionFitter hogPositionFitter_;
        HogPositionFitterData hogPositionFitterData_;

        GenderSorter genderSorter_;
        GenderSorterData genderSorterData_;

        QStringList batchVideoFileList_;
        int batchVideoFileIndex_;

        QStringList batchVideoDirList_;
        int batchVideoDirIndex_;

        QPointer<ExtCtlHttpServer> httpServerPtr_;
        unsigned int httpServerPort_;

        void connectWidgets();
        void initialize();
        void startImageCapture();
        void stopImageCapture();
        void updateAllImageLabels();
        void updateImageLabel(QLabel *labelPtr, QPixmap &pixmap); 
        void resizeAllImageLabels();
        void resizeImageLabel(QLabel *labelPtr, QPixmap &pixmap);
        void setupImageLabels();
        void setupDisplayTimer();
        void setupNetworkAccessManager();
        void sendDataViaHttpRequest();
        void startHttpServer();
        QVariantMap dataToMap();
        QByteArray dataToJson();
        void loadParamFromFile();
        void updateParamText();
        void updateWidgetsOnLoad();
        void setupTrainingDataWrite(QString name);
        void setupBatchDataWrite();
        void setupDebugImagesWrite();
        TrainingDataMode getTrainingDataMode();

        bool isTrainingDataModeSingle();
        bool isTrainingDataModeBatch();
        bool isCaptureModeFile();
        bool isCaptureModeDir();
        bool updateBatchVideoFileList();
        bool updateBatchVideoDirList();
        bool createTrainingData();
        bool createDebugImages();
        bool createDebugLog();


        // Debug 
        std::ofstream debugDataLogStream_;
        QDir debugImagesDir_;

};

#endif // #ifndef FLY_SORTER_WINDOW_HPP
