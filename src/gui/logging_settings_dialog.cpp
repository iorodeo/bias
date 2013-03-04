#include "logging_settings_dialog.hpp"
#include "video_writer_ufmf.hpp"
#include "background_histogram_ufmf.hpp"
#include <iostream>
#include <QPointer>

namespace bias
{

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
        params_ = params;
        setInitialValues();
        setValidators();
        connectWidgets();

    }

    void LoggingSettingsDialog::setInitialValues()
    {
        bmpFrameSkipLineEditPtr_ -> setText(
                QString::number(params_.bmp.frameSkip)
                );

        bmpFrameSkipRangeLabelPtr_ -> setText(QString(" > 0 "));

        aviFrameSkipLineEditPtr_ -> setText(
                QString::number(params_.avi.frameSkip)
                );

        aviFrameSkipRangeLabelPtr_ -> setText(QString(" > 0 "));

        aviCodecComboBoxPtr_ -> addItem(params_.avi.codec);

        fmfFrameSkipLineEditPtr_ -> setText(QString::number(
                    params_.fmf.frameSkip)
                );

        fmfFrameSkipRangeLabelPtr_ -> setText(QString(" > 0 "));

        ufmfFrameSkipLineEditPtr_ -> setText(
                QString::number(params_.ufmf.frameSkip)
                );

        ufmfBackgroundThresholdLineEditPtr_ -> setText(
                QString::number(params_.ufmf.backgroundThreshold)
                );

        ufmfBoxLengthLineEditPtr_ -> setText(
                QString::number(params_.ufmf.boxLength)
                );

        ufmfMedianUpdateCountLineEditPtr_ -> setText(
                QString::number(params_.ufmf.medianUpdateCount)
                );

        ufmfCompressionThreadsLineEditPtr_ -> setText(
                QString::number(params_.ufmf.numberOfCompressors)
                );

        // Temporary
        aviCodecComboBoxPtr_ -> setEnabled(false);
        ufmfDilateCheckBoxPtr_ -> setEnabled(false);
        ufmfDilateLineEditPtr_ -> setEnabled(false);


    }

    void LoggingSettingsDialog::setValidators()
    {
        QPointer<QIntValidator> validatorPtr;

        // bmp tab
        validatorPtr = new QIntValidator(bmpFrameSkipLineEditPtr_);
        validatorPtr -> setBottom(0);
        bmpFrameSkipLineEditPtr_ -> setValidator(validatorPtr);

        // avi tab
        validatorPtr = new QIntValidator(aviFrameSkipLineEditPtr_);
        validatorPtr -> setBottom(0);
        aviFrameSkipLineEditPtr_ -> setValidator(validatorPtr);

        // fmf tab
        validatorPtr = new QIntValidator(fmfFrameSkipLineEditPtr_);
        validatorPtr -> setBottom(0);
        fmfFrameSkipLineEditPtr_ -> setValidator(validatorPtr);

        // ufmf tab
        validatorPtr = new QIntValidator(ufmfFrameSkipLineEditPtr_);
        validatorPtr -> setBottom(0);
        ufmfFrameSkipLineEditPtr_ -> setValidator(validatorPtr);

        validatorPtr = new QIntValidator(ufmfBackgroundThresholdLineEditPtr_);
        validatorPtr -> setRange(
                VideoWriter_ufmf::MIN_BACKGROUND_THRESHOLD,
                VideoWriter_ufmf::MAX_BACKGROUND_THRESHOLD
                );
        ufmfBackgroundThresholdLineEditPtr_ -> setValidator(validatorPtr);

        validatorPtr = new QIntValidator(ufmfBoxLengthLineEditPtr_);
        validatorPtr -> setRange(
                VideoWriter_ufmf::MIN_BOX_LENGTH,
                VideoWriter_ufmf::MAX_BOX_LENGTH
                );
        ufmfBoxLengthLineEditPtr_ -> setValidator(validatorPtr);


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
                aviFrameSkipLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(aviFrameSkip_EditingFinished())
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

    }


    void LoggingSettingsDialog::bmpFrameSkip_EditingFinished()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


    void LoggingSettingsDialog::aviFrameSkip_EditingFinished()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


    void LoggingSettingsDialog::fmfFrameSkip_EditingFinished()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


    void LoggingSettingsDialog::ufmfFrameSkip_EditingFinished()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


    void  LoggingSettingsDialog::ufmfBackgroundThreshold_EditingFinished()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


    void LoggingSettingsDialog::ufmfBoxLength_EditingFinished()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void LoggingSettingsDialog::ufmfMedianUpdateCount_EditingFinished()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


    void LoggingSettingsDialog::ufmfCompressionThreads_EditingFinished()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void LoggingSettingsDialog::ufmfDilateCheckBox_StateChanged(int state)
    {
        std::cout << __PRETTY_FUNCTION__ << ", state = " << state << std::endl;
    }


    void LoggingSettingsDialog::ufmfDilate_EditingFinished()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


} // namespace bias
