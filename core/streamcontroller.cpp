/*
 * This file is part of DLM.
 *
 * DLM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DLM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DLM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "streamcontroller.h"
#include "ui_general.h"
#include "ui_encoder.h"
#include "graphics/graphicscontroller.h"
#include "audiocontroller.h"
#include "mediaprocessor.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QFile>
#include <QDir>
#include <QDebug>

const int kResolution_16_9_Num = 9;
const QSize kResolution_16_9[kResolution_16_9_Num] = {
    QSize(704, 396),
    QSize(768, 432),
    QSize(832, 468),
    QSize(896, 504),
    QSize(960, 540),
    QSize(1024, 576),
    QSize(1280, 720),
    QSize(1600, 900),
    QSize(1920, 1080)
};

const int kResolution_4_3_Num = 7;
const QSize kResolution_4_3[kResolution_4_3_Num] = {
    QSize(704, 528),
    QSize(768, 576),
    QSize(832, 624),
    QSize(896, 672),
    QSize(960, 720),
    QSize(1024, 768),
    QSize(1280, 960)
};

StreamController::StreamController(GraphicsController *graphics, AudioController *audio,
                                   QWidget *general_setting, QWidget *encoder_setting, QObject *parent):
    QObject(parent),
    /* ui related members */
    general_ui(new Ui::GeneralSetting),
    encoder_ui(new Ui::EncoderSetting),
    /* main members */
    graphics_(graphics),
    audio_(audio),
    processor_(new MediaProcessor),
    /* misc */
    is_active_(0),
    target_frame_period_(0),
    frame_period_(0),
    image_size_(QSize(0, 0)),
    rendered_(QImage(image_size_, QImage::Format_RGB888))
{
    // init ui
    general_ui->setupUi(general_setting);
    connect(general_ui->open_dest_file_, SIGNAL(clicked()), this, SLOT(onOpenSaveFile()));

    encoder_ui->setupUi(encoder_setting);
    connect(encoder_ui->v_size_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSizeChanged()));
    connect(encoder_ui->v_framerate_, SIGNAL(valueChanged(int)), this, SLOT(setFramerate(int)));
    for(int i = 0; i < kResolution_16_9_Num; ++i) {
        const QSize &size = kResolution_16_9[i];
        QString resolution =
                QString::number(size.width()) + "x" +
                QString::number(size.height()) + " (16:9)";
        encoder_ui->v_size_->addItem(resolution);
    }
    for(int i = 0; i < kResolution_4_3_Num; ++i) {
        const QSize &size = kResolution_4_3[i];
        QString resolution =
                QString::number(size.width()) + "x" +
                QString::number(size.height()) + " (4:3)";
        encoder_ui->v_size_->addItem(resolution);
    }

    audio_->setupUi(encoder_ui->a_device_);

    // media processing
    connect(this, SIGNAL(requestProcessAudioData(int,QByteArray)),
            processor_, SLOT(processAudioData(int,QByteArray)));
    connect(this, SIGNAL(requestProcessVideoData(int,QImage)),
            processor_, SLOT(processVideoData(int,QImage)));
    connect(this, SIGNAL(requestStop()),
            processor_, SLOT(stop()));
    processor_->moveToThread(&media_process_thread_);
    media_process_thread_.start(QThread::HighPriority);

    // misc
    connect(&update_timer_, SIGNAL(timeout()), this, SLOT(process()));

    setFramerate(encoder_ui->v_framerate_->value());
    general_ui->save_to_file_->setChecked(true);
    general_ui->dest_file_->setText(QDir::homePath() + "/live.flv");
    encoder_ui->v_size_->setCurrentIndex(0);
    encoder_ui->v_preset_->setCurrentIndex(5); // Medium
}

StreamController::~StreamController()
{
    stop();
    media_process_thread_.quit();
    media_process_thread_.wait();

    delete processor_;
    delete general_ui;
    delete encoder_ui;
}

bool StreamController::start()
{
    if(is_active_) return false;
    is_active_ = true;

    // wait upto 100ms for media processor to finish if active
    for(int i = 0; i < 10; ++i) {
        if(!processor_->isActive()) break;
        QThread::msleep(10);
    }
    if(processor_->isActive()) return false;

    // setup media processor
    processor_->setupAudioEncoder(44100, encoder_ui->a_bitrate_->value());
    processor_->setupVideoEncoder(image_size_,
                                  encoder_ui->v_framerate_->value(),
                                  encoder_ui->v_bitrate_->value(),
                                  encoder_ui->v_preset_->currentIndex());

    if(general_ui->save_to_file_->isChecked()) {
        processor_->EnableSaveToLocalFile(general_ui->dest_file_->text());
    } else {
        processor_->DisableSaveToLocalFile();
    }

    if(general_ui->stream_to_server_->isChecked()) {
        QString url =
                general_ui->rtmp_url_->text() + QString("/") +
                general_ui->rtmp_stream_->text();
        processor_->EnableRTMPStreaming(url);
    } else {
        processor_->DisableRTMPStreaming();
    }

    processor_->start();

    // start audioinput device
    audio_->setSampleRate(44100);
    audio_->start();

    time_.start();

    return true;
}

void StreamController::stop()
{
    if(!is_active_) return;
    is_active_ = false;

    audio_->stop();
    emit requestStop();
    setFramePeriod(target_frame_period_);
}

bool StreamController::isActive()
{
    return is_active_;
}

int StreamController::elapsed()
{
    return time_.elapsed();
}

void StreamController::process()
{
    graphics_->update();
    if(!is_active_) return;

    QPainter p(&rendered_);
    graphics_->render(&p);
    emit requestProcessVideoData(time_.elapsed(), rendered_);

    emit requestProcessAudioData(time_.elapsed(), audio_->data());

    // framerate control
    int since_last_frame = time_.elapsed() - processor_->lastTimestamp();
    qDebug() << frame_period_ << target_frame_period_ << since_last_frame;
    if(since_last_frame < frame_period_ * 2) {
        // do nothing
        if(frame_period_ == target_frame_period_) {
            qDebug() << "ok";
        } else {
            int period = frame_period_ - (frame_period_ >> 3);
            setFramePeriod(period > target_frame_period_ ?
                               period : target_frame_period_);
            qDebug() << "decrease period";
        }
    } else {
        setFramePeriod(frame_period_ + (frame_period_ >> 2));
            qDebug() << "increase period";
    }
}

void StreamController::onOpenSaveFile()
{
    QString dest = QFileDialog::getSaveFileName(
                0, QString("Destination File"), QDir::homePath());
    if(dest != QString()) {
        general_ui->dest_file_->setText(dest);
    }
}

void StreamController::onSizeChanged()
{
    int index = encoder_ui->v_size_->currentIndex();
    if(index < kResolution_16_9_Num) {
        setSize(kResolution_16_9[index]);
    } else if (index - kResolution_16_9_Num < kResolution_4_3_Num) {
        setSize(kResolution_4_3[index - kResolution_16_9_Num]);
    } else {
        qDebug() << "unknown index of size";
    }
}

void StreamController::setFramerate(int framerate)
{
    if(is_active_) return;
    target_frame_period_ = 1000 / framerate;
    setFramePeriod(target_frame_period_);
}

void StreamController::setSize(const QSize &size)
{
    if(is_active_) return;
    image_size_ = size;
    rendered_ = QImage(image_size_, QImage::Format_RGB888);
    graphics_->setSize(image_size_);
}

void StreamController::setFramePeriod(int period)
{
    frame_period_ = period;
    update_timer_.stop();
    update_timer_.start(period);
}
