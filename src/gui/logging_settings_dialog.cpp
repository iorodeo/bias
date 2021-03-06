#include "logging_settings_dialog.hpp"
#include "video_writer_bmp.hpp"
#include "video_writer_jpg.hpp"
#include "video_writer_ufmf.hpp"
#include "video_writer_avi.hpp"
#include "background_histogram_ufmf.hpp"
#include "validators.hpp"
#include <iostream>
#include <QPointer>
#include <QStringList>
#include <QtDebug>

namespace bias
{

    // LoggingSettingsDialog methods
    // -----------------------------------------------------------------------------

    LoggingSettingsDialog::LoggingSettingsDialog(QWidget *parent)
        : QDialog(parent)
    {
        VideoWriterParams dummyParams;
        initialize(dummyParams);
    }


    LoggingSettingsDialog::LoggingSettingsDialog(VideoWriterParams params, QWidget *parent)
        : QDialog(parent)
    {
        initialize(params);
    }


    void LoggingSettingsDialog::initialize(VideoWriterParams params)
    {
        setupUi(this);
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        params_ = params;
        setInitialValues();
        setValidators();
        connectWidgets();

    }

    void LoggingSettingsDialog::setInitialValues()
    {
        QString tmpString;

        // bmp tab - frame skip
        tmpString = QString::number(params_.bmp.frameSkip);
        bmpFrameSkipLineEditPtr_ -> setText(tmpString);
        bmpFrameSkipRangeLabelPtr_ -> setText(QString(" >= 1 "));

        // jpg tab - frame skip
        tmpString = QString::number(params_.jpg.frameSkip);
        jpgFrameSkipLineEditPtr_ -> setText(tmpString);
        jpgFrameSkipRangeLabelPtr_ -> setText(QString(" >= 1"));

        // jpg tab - quality
        tmpString = QString::number(params_.jpg.quality);
        jpgQualityLineEditPtr_ -> setText(tmpString);
        tmpString = QString("(%1, %2)").arg(
                QString::number(VideoWriter_jpg::MIN_QUALITY),  
                QString::number(VideoWriter_jpg::MAX_QUALITY)
                );
        jpgQualityRangeLabelPtr_ -> setText(tmpString);

        // jpg compression threads
        tmpString = QString::number(params_.jpg.numberOfCompressors);
        jpgCompressionThreadsLineEditPtr_ -> setText(tmpString);
        jpgCompressionThreadsRangeLabelPtr_ -> setText(QString(" >= 1"));

        // jpg mjpgFlag 
        if (params_.jpg.mjpgFlag)
        {
            jpgMjpgFlagCheckBoxPtr_ -> setCheckState(Qt::Checked);
        }
        else
        {
            jpgMjpgFlagCheckBoxPtr_ -> setCheckState(Qt::Unchecked);
        }

        // jpg mjpgMaxFramePerFileFlag
        if (params_.jpg.mjpgMaxFramePerFileFlag)
        {
            jpgMjpgMaxFramePerFileCheckBoxPtr_ -> setCheckState(Qt::Checked);
        }
        else
        {
            jpgMjpgMaxFramePerFileCheckBoxPtr_ -> setCheckState(Qt::Unchecked);
        }

        // jpg mjpg - maximum number of frames per file
        tmpString = QString::number(params_.jpg.mjpgMaxFramePerFile);
        jpgMjpgMaxFramePerFileLineEditPtr_ -> setText(tmpString);
        tmpString = QString(" >= %1").arg(VideoWriter_jpg::MJPG_MINVAL_MAX_FRAME_PER_FILE);
        jpgMjpgMaxFramePerFileRangeLabelPtr_ -> setText(tmpString);

        // avi tab - frame skip
        tmpString = QString::number(params_.avi.frameSkip);
        aviFrameSkipLineEditPtr_ -> setText(tmpString);
        aviFrameSkipRangeLabelPtr_ -> setText(QString(" >= 1 "));

        // avi tabi - codec
        QStringList allowedCodecList = VideoWriter_avi::getListOfAllowedCodecs();
        for (unsigned int i=0; i<allowedCodecList.size(); i++)
        {
            QString codecString = allowedCodecList.at(i);
            aviCodecComboBoxPtr_ -> addItem(codecString);
            if (codecString == params_.avi.codec)
            {
                aviCodecComboBoxPtr_ -> setCurrentIndex(i);
            }
        }

        // fmf tab - frame skip
        tmpString = QString::number(params_.fmf.frameSkip);
        fmfFrameSkipLineEditPtr_ -> setText(tmpString);
        fmfFrameSkipRangeLabelPtr_ -> setText(QString(" >= 1 "));

        // ufmf tab - frame skip
        tmpString = QString::number(params_.ufmf.frameSkip);
        ufmfFrameSkipLineEditPtr_ -> setText(tmpString);
        ufmfFrameSkipRangeLabelPtr_ -> setText(QString(" >= 1"));

        // ufmf tab - background threshold
        tmpString = QString::number(params_.ufmf.backgroundThreshold);
        ufmfBackgroundThresholdLineEditPtr_ -> setText(tmpString);

        tmpString = QString("(%1, %2)").arg(
                    QString::number(VideoWriter_ufmf::MIN_BACKGROUND_THRESHOLD),
                    QString::number(VideoWriter_ufmf::MAX_BACKGROUND_THRESHOLD)
                    );
        ufmfBackgroundThresholdRangeLabelPtr_ -> setText(tmpString);

        // ufmf tab - box length
        tmpString = QString::number(params_.ufmf.boxLength);
        ufmfBoxLengthLineEditPtr_ -> setText(tmpString);

        tmpString = QString("(%1, %2)").arg(
                QString::number(VideoWriter_ufmf::MIN_BOX_LENGTH),  
                QString::number(VideoWriter_ufmf::MAX_BOX_LENGTH)
                );
        ufmfBoxLengthRangeLabelPtr_ -> setText(tmpString);

        // ufmf tab - median update count
        tmpString = QString::number(params_.ufmf.medianUpdateCount);
        ufmfMedianUpdateCountLineEditPtr_ -> setText(tmpString);
        tmpString = QString(" >= %1").arg(
                QString::number(BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_COUNT)
                );
        ufmfMedianUpdateCountRangeLabelPtr_ -> setText(tmpString);

        // ufmf tab - median update interval
        tmpString = QString::number(params_.ufmf.medianUpdateInterval);
        ufmfMedianUpdateIntervalLineEditPtr_ -> setText(tmpString);
        tmpString = QString(" >= %1").arg(
                QString::number(BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_INTERVAL)
                );
        ufmfMedianUpdateIntervalRangeLabelPtr_ -> setText(tmpString);

        // ufmf tab - number of compressor threads
        tmpString = QString::number(params_.ufmf.numberOfCompressors);
        ufmfCompressionThreadsLineEditPtr_ -> setText(tmpString);

        tmpString = QString(" >= %1").arg(
                QString::number(VideoWriter_ufmf::MIN_NUMBER_OF_COMPRESSORS)
                );
        ufmfCompressionThreadsRangeLabelPtr_ -> setText(tmpString);

        // ufmf tab - dilate 
        if (params_.ufmf.dilateState)
        {
            ufmfDilateCheckBoxPtr_ -> setCheckState(Qt::Checked);
        }
        else
        {
            ufmfDilateCheckBoxPtr_ -> setCheckState(Qt::Unchecked);
        }

        tmpString = QString::number(params_.ufmf.dilateWindowSize);
        ufmfDilateLineEditPtr_ -> setText(tmpString);

        tmpString = QString("(%1, %2)").arg(
                    QString::number(VideoWriter_ufmf::MIN_DILATE_WINDOW_SIZE),
                    QString::number(VideoWriter_ufmf::MAX_DILATE_WINDOW_SIZE)
                    );
        ufmfDilateRangeLabelPtr_ -> setText(tmpString);


        ufmfDilateCheckBoxPtr_ -> setEnabled(true); 
        ufmfDilateLineEditPtr_ -> setEnabled(true); 

    }


    void LoggingSettingsDialog::setValidators()
    {
        QPointer<QIntValidator> validatorPtr;

        // bmp tab - frame skip
        validatorPtr = new IntValidatorWithFixup(bmpFrameSkipLineEditPtr_);
        validatorPtr -> setBottom(1);
        bmpFrameSkipLineEditPtr_ -> setValidator(validatorPtr);

        // jpg tab - frame skip
        validatorPtr = new IntValidatorWithFixup(jpgFrameSkipLineEditPtr_);
        validatorPtr -> setBottom(1);
        jpgFrameSkipLineEditPtr_ -> setValidator(validatorPtr);

        // jpg tab - quality
        validatorPtr = new IntValidatorWithFixup(jpgQualityLineEditPtr_);
        validatorPtr -> setRange(
                VideoWriter_jpg::MIN_QUALITY,
                VideoWriter_jpg::MAX_QUALITY
                );
        jpgQualityLineEditPtr_ -> setValidator(validatorPtr);

        // jpg tab - compression threads
        validatorPtr = new IntValidatorWithFixup(jpgCompressionThreadsLineEditPtr_);
        validatorPtr -> setBottom(1);
        jpgCompressionThreadsLineEditPtr_ -> setValidator(validatorPtr);

        // jpg tab - mjpg max frame per file
        validatorPtr = new IntValidatorWithFixup(jpgMjpgMaxFramePerFileLineEditPtr_);
        validatorPtr -> setBottom(VideoWriter_jpg::MJPG_MINVAL_MAX_FRAME_PER_FILE);
        jpgMjpgMaxFramePerFileLineEditPtr_ -> setValidator(validatorPtr);

        // avi tab - frame skip
        validatorPtr = new IntValidatorWithFixup(aviFrameSkipLineEditPtr_);
        validatorPtr -> setBottom(1);
        aviFrameSkipLineEditPtr_ -> setValidator(validatorPtr);

        // fmf tab - frame skip
        validatorPtr = new IntValidatorWithFixup(fmfFrameSkipLineEditPtr_);
        validatorPtr -> setBottom(1);
        fmfFrameSkipLineEditPtr_ -> setValidator(validatorPtr);

        // ufmf tab - frame skip
        validatorPtr = new IntValidatorWithFixup(ufmfFrameSkipLineEditPtr_);
        validatorPtr -> setBottom(1);
        ufmfFrameSkipLineEditPtr_ -> setValidator(validatorPtr);

        // ufmf tab - background threshold
        validatorPtr = new IntValidatorWithFixup(ufmfBackgroundThresholdLineEditPtr_);
        validatorPtr -> setRange(
                VideoWriter_ufmf::MIN_BACKGROUND_THRESHOLD,
                VideoWriter_ufmf::MAX_BACKGROUND_THRESHOLD
                );
        ufmfBackgroundThresholdLineEditPtr_ -> setValidator(validatorPtr);

        // ufmf tab - box length
        validatorPtr = new IntValidatorWithFixup(ufmfBoxLengthLineEditPtr_);
        validatorPtr -> setRange(
                VideoWriter_ufmf::MIN_BOX_LENGTH,
                VideoWriter_ufmf::MAX_BOX_LENGTH
                );
        ufmfBoxLengthLineEditPtr_ -> setValidator(validatorPtr);

        // ufmf tab - median update count
        validatorPtr = new IntValidatorWithFixup(ufmfMedianUpdateCountLineEditPtr_);
        validatorPtr -> setBottom(BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_COUNT);
        ufmfMedianUpdateCountLineEditPtr_ -> setValidator(validatorPtr);

        // ufmf tab - median update interval
        validatorPtr = new IntValidatorWithFixup(ufmfMedianUpdateIntervalLineEditPtr_);
        validatorPtr -> setBottom(BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_INTERVAL);
        ufmfMedianUpdateIntervalLineEditPtr_ -> setValidator(validatorPtr);

        // ufmf tab - number of compression threads
        validatorPtr = new IntValidatorWithFixup(ufmfCompressionThreadsLineEditPtr_);
        validatorPtr -> setBottom(VideoWriter_ufmf::MIN_NUMBER_OF_COMPRESSORS);
        ufmfCompressionThreadsLineEditPtr_ -> setValidator(validatorPtr);

        // ufmf tab - dilate window size
        validatorPtr = new IntValidatorWithFixup(ufmfDilateLineEditPtr_);
        validatorPtr -> setRange(
                VideoWriter_ufmf::MIN_DILATE_WINDOW_SIZE,
                VideoWriter_ufmf::MAX_DILATE_WINDOW_SIZE
                );
        ufmfDilateLineEditPtr_ -> setValidator(validatorPtr);
    }


    void LoggingSettingsDialog::connectWidgets()
    {
        connect(
                bmpFrameSkipLineEditPtr_, 
                SIGNAL(editingFinished()),
                this,
                SLOT(bmpFrameSkip_EditingFinished())
               );

        connect(
                jpgFrameSkipLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(jpgFrameSkip_EditingFinished())
               );

        connect(
                jpgQualityLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(jpgQuality_EditingFinished())
               );

        connect(
                jpgCompressionThreadsLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(jpgCompressionThreads_EditingFinished())
               );


        connect(
                jpgMjpgFlagCheckBoxPtr_,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(jpgMjpgFlagCheckBox_StateChanged(int))
               );

        connect(
                jpgMjpgMaxFramePerFileCheckBoxPtr_,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(jpgMjpgMaxFramePerFileFlagCheckBox_StateChanged(int))
               );

        connect(
                jpgMjpgMaxFramePerFileLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(jpgMjpgMaxFramePerFile_EditingFinished())
               );

        connect(
                aviFrameSkipLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(aviFrameSkip_EditingFinished())
               );

        connect(
                aviCodecComboBoxPtr_,
                SIGNAL(currentIndexChanged(QString)),
                this,
                SLOT(aviCodecComboBox_CurrentIndexChanged(QString))
               );

        connect(
                fmfFrameSkipLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(fmfFrameSkip_EditingFinished())
               );

        connect(
                ufmfFrameSkipLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(ufmfFrameSkip_EditingFinished())
               );

        connect(
                ufmfBackgroundThresholdLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(ufmfBackgroundThreshold_EditingFinished())
               );

        connect(
                ufmfBoxLengthLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(ufmfBoxLength_EditingFinished())
               );

        connect( 
                ufmfMedianUpdateCountLineEditPtr_, 
                SIGNAL(editingFinished()), 
                this, 
                SLOT(ufmfMedianUpdateCount_EditingFinished())
                );

        connect(
                ufmfMedianUpdateIntervalLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(ufmfMedianUpdateInterval_EditingFinished())
               );

        connect(
                ufmfCompressionThreadsLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(ufmfCompressionThreads_EditingFinished())
               );

        connect(
                ufmfDilateCheckBoxPtr_,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(ufmfDilateCheckBox_StateChanged(int))
               );

        connect(
                ufmfDilateLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(ufmfDilate_EditingFinished())
               );

        connect(
                parent(),
                SIGNAL(imageCaptureStarted(bool)),
                this,
                SLOT(imageCaptureStarted(bool))
               );

        connect(
                parent(),
                SIGNAL(imageCaptureStopped()),
                this,
                SLOT(imageCaptureStopped())
               );
    }


    void LoggingSettingsDialog::bmpFrameSkip_EditingFinished()
    {
        QString frameSkipString = bmpFrameSkipLineEditPtr_ -> text();
        unsigned int frameSkip = frameSkipString.toUInt();
        params_.bmp.frameSkip = frameSkip;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::jpgFrameSkip_EditingFinished()
    {
        QString frameSkipString = jpgFrameSkipLineEditPtr_ -> text();
        unsigned int frameSkip = frameSkipString.toUInt();
        params_.jpg.frameSkip = frameSkip;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::jpgQuality_EditingFinished()
    {
        QString qualityString = jpgQualityLineEditPtr_ -> text();
        unsigned int quality = qualityString.toUInt();
        params_.jpg.quality = quality;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::jpgCompressionThreads_EditingFinished()
    {
        QString lineEditString = jpgCompressionThreadsLineEditPtr_ -> text();
        unsigned int numberOfCompressors =  lineEditString.toUInt();
        params_.jpg.numberOfCompressors = numberOfCompressors;
        emit parametersChanged(params_);
    }

    
    void LoggingSettingsDialog::jpgMjpgFlagCheckBox_StateChanged(int state)
    {
        params_.jpg.mjpgFlag = bool(state);
        emit parametersChanged(params_);
    }

    void LoggingSettingsDialog::jpgMjpgMaxFramePerFileFlagCheckBox_StateChanged(int state)
    {
        params_.jpg.mjpgMaxFramePerFileFlag = bool(state);
        emit parametersChanged(params_);
    }

    void LoggingSettingsDialog::jpgMjpgMaxFramePerFile_EditingFinished()
    {
        QString lineEditString = jpgMjpgMaxFramePerFileLineEditPtr_ -> text();
        unsigned long maxFramePerFile = lineEditString.toULong();
        params_.jpg.mjpgMaxFramePerFile = maxFramePerFile;
        emit parametersChanged(params_);
    }

    void LoggingSettingsDialog::aviFrameSkip_EditingFinished()
    {
        QString frameSkipString = aviFrameSkipLineEditPtr_ -> text();
        unsigned int frameSkip = frameSkipString.toUInt();
        params_.avi.frameSkip = frameSkip;
        emit parametersChanged(params_);
    }

    void LoggingSettingsDialog::aviCodecComboBox_CurrentIndexChanged(QString text)
    {
        params_.avi.codec = text;
        emit parametersChanged(params_);
    }

    void LoggingSettingsDialog::fmfFrameSkip_EditingFinished()
    {
        QString frameSkipString = fmfFrameSkipLineEditPtr_ -> text();
        unsigned int frameSkip = frameSkipString.toUInt();
        params_.fmf.frameSkip = frameSkip;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::ufmfFrameSkip_EditingFinished()
    {
        QString frameSkipString = ufmfFrameSkipLineEditPtr_ -> text();
        unsigned int frameSkip = frameSkipString.toUInt();
        params_.ufmf.frameSkip = frameSkip;
        emit parametersChanged(params_);
    }


    void  LoggingSettingsDialog::ufmfBackgroundThreshold_EditingFinished()
    {
        QString thresholdString = ufmfBackgroundThresholdLineEditPtr_ -> text();
        unsigned int threshold = thresholdString.toUInt();
        params_.ufmf.backgroundThreshold = threshold;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::ufmfBoxLength_EditingFinished()
    {
        QString boxLengthString = ufmfBoxLengthLineEditPtr_ -> text();
        unsigned int boxLength = boxLengthString.toUInt();
        params_.ufmf.boxLength = boxLength;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::ufmfMedianUpdateCount_EditingFinished()
    {
        QString updateCountString = ufmfMedianUpdateCountLineEditPtr_ -> text();
        unsigned int updateCount = updateCountString.toUInt();
        params_.ufmf.medianUpdateCount = updateCount;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::ufmfMedianUpdateInterval_EditingFinished()
    {
        QString updateIntervalString = ufmfMedianUpdateIntervalLineEditPtr_ -> text();
        unsigned int updateInterval = updateIntervalString.toUInt();
        params_.ufmf.medianUpdateInterval = updateInterval;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::ufmfCompressionThreads_EditingFinished()
    {
        QString numberString = ufmfCompressionThreadsLineEditPtr_ -> text();
        unsigned int number = numberString.toUInt();
        params_.ufmf.numberOfCompressors = number;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::ufmfDilateCheckBox_StateChanged(int state)
    {
        params_.ufmf.dilateState = bool(state);
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::ufmfDilate_EditingFinished()
    {
        QString sizeString = ufmfDilateLineEditPtr_ -> text();
        unsigned int size = sizeString.toUInt(); 
        params_.ufmf.dilateWindowSize = size;
        emit parametersChanged(params_);
    }


    void LoggingSettingsDialog::imageCaptureStarted(bool logging)
    {
        setEnabled(false);
    }

    
    void LoggingSettingsDialog::imageCaptureStopped()
    {
        setEnabled(true);
    }


} // namespace bias
