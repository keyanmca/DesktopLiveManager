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
#include <QDebug>

StreamController::StreamController(GraphicsController *graphics, AudioController *audio,
                                   QWidget *general_tab, QWidget *encoder_tab, QObject *parent):
    QObject(parent),
    /* ui related members */
    general_tab_(general_tab),
    encoder_tab_(encoder_tab),
    general_ui(new Ui::GeneralTab),
    encoder_ui(new Ui::EncoderTab),
    /* main members */
    graphics_(graphics),
    audio_(audio),
    processor_(new MediaProcessor),
    /* misc */
    is_active_(0),
    image_size_(QSize(0, 0)),
    rendered_(QImage(image_size_, QImage::Format_RGB888)),
    audio_timestamp_(0)
{
    // init ui
    general_ui->setupUi(general_tab_);

    general_ui->save_to_file_->setChecked(true);
    general_ui->dest_file_->setText(QString("/home/sk/test.flv"));
    connect(general_ui->open_dest_file_, SIGNAL(clicked()), this, SLOT(onOpenSaveFile()));

    encoder_ui->setupUi(encoder_tab_);

    connect(encoder_ui->v_size_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSizeChanged()));
    connect(encoder_ui->v_framerate_, SIGNAL(valueChanged(int)), this, SLOT(onFPSChanged()));

    audio_->setupUi(encoder_ui->a_device_);

    // media processing
    connect(this, SIGNAL(requestProcessAudioData(int,QByteArray)),
            processor_, SLOT(processAudioData(int,QByteArray)));
    connect(this, SIGNAL(requestProcessVideoData(int,QImage)),
            processor_, SLOT(processVideoData(int,QImage)));

    // misc
    connect(&update_timer_, SIGNAL(timeout()), this, SLOT(process()));

    setSize(QSize(640, 480));
    setFPS(encoder_ui->v_framerate_->value());
}

StreamController::~StreamController()
{
    delete general_ui;
    delete encoder_ui;
}

bool StreamController::start()
{
    if(is_active_) return false;
    is_active_ = true;

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
    processor_->moveToThread(&media_process_thread_);
    media_process_thread_.start();

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

    media_process_thread_.quit();
    media_process_thread_.wait();
    processor_->stop();
    //processor_->moveToThread(QThread::currentThread());

    audio_->stop();
}

bool StreamController::isActive()
{
    return is_active_;
}

int StreamController::elapsed()
{
    return time_.elapsed();
}

void StreamController::setSize(const QSize &size)
{
    if(is_active_) return;
    image_size_ = size;
    rendered_ = QImage(image_size_, QImage::Format_RGB888);
    graphics_->setSize(image_size_);
}

void StreamController::setFPS(int fps)
{
    if(is_active_) return;

    update_timer_.stop();
    update_timer_.start(1000 / fps);
}

void StreamController::process()
{
    graphics_->update();
    if(!is_active_) return;

    int ts = time_.elapsed();

    QPainter p(&rendered_);
    graphics_->render(&p);
    requestProcessVideoData(ts, rendered_);

    requestProcessAudioData(ts, audio_->data());
}

void StreamController::onOpenSaveFile()
{
    QString dest = QFileDialog::getSaveFileName(general_tab_, QString("Destination File"), QDir::homePath());
    if(dest != QString()) {
        general_ui->dest_file_->setText(dest);
    }
}

void StreamController::onSizeChanged()
{
    int index = encoder_ui->v_size_->currentIndex();
    switch(index) {
    case 0:
        setSize(QSize(640, 480));
        break;
    case 1:
        setSize(QSize(512, 384));
        break;
    default:
        qDebug() << "unknown index of size";
        break;
    }
}

void StreamController::onFPSChanged()
{
    setFPS(encoder_ui->v_framerate_->value());
}
