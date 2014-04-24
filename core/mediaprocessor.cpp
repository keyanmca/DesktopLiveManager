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

#include "mediaprocessor.h"
#include "external/videoencoder.h"
#include "external/audioencoder.h"
#include "external/rtmpstreamer.h"
#include "util/flvstreamer.h"

#include <QFile>
#include <QDebug>

MediaProcessor::MediaProcessor(QObject *parent) :
    QObject(parent),
    video_encoder_(0),
    audio_encoder_(0),
    flv_formatter_(new FLVFormatter),
    rtmp_streamer_(0),
    dest_file_(0),
    is_active_(false),
    last_timestamp_(0)
{
}

MediaProcessor::~MediaProcessor()
{
    delete video_encoder_;
    delete audio_encoder_;
    delete flv_formatter_;
    delete rtmp_streamer_;
}

void MediaProcessor::EnableRTMPStreaming(const QString &url)
{
    rtmp_url_ = url;
}

void MediaProcessor::EnableSaveToLocalFile(const QString &url)
{
    file_url_ = url;
}

void MediaProcessor::DisableRTMPStreaming()
{
    delete rtmp_streamer_;
    rtmp_streamer_ = 0;
    rtmp_url_ = QString();
}

void MediaProcessor::DisableSaveToLocalFile()
{
    delete dest_file_;
    dest_file_ = 0;
    file_url_ = QString();
}

void MediaProcessor::setupAudioEncoder(int samplerate, int bitrate_kbps)
{
    if(audio_encoder_) {
        delete audio_encoder_;
    }

    audio_encoder_ = new AudioEncoder(samplerate, bitrate_kbps);
    audio_encoder_->open(QIODevice::ReadWrite);
}

void MediaProcessor::setupVideoEncoder(
        QSize image_size, int framerate, int bitrate_kbps, int preset_index)
{
    if(video_encoder_) {
        delete video_encoder_;
    }

    video_encoder_ = new VideoEncoder(
                image_size, framerate, bitrate_kbps, preset_index);
}

void MediaProcessor::start()
{
    if(is_active_) return;
    is_active_ = true;
    last_timestamp_ = 0;

    if(rtmp_url_ != QString()) {
        rtmp_streamer_ = new RTMPStreamer();
        rtmp_streamer_->open(QIODevice::WriteOnly);
        rtmp_streamer_->connectToServer(rtmp_url_);
    }

    if(file_url_ != QString()) {
        dest_file_ = new QFile(file_url_);
        dest_file_->open(QIODevice::WriteOnly | QIODevice::Truncate);

        // write flv header
        flv_formatter_->writeHeader();
        dest_file_->write(flv_formatter_->data(), flv_formatter_->size());
    }

    // write SPS/PPS info
    if(video_encoder_) {
        QByteArray sps_pps = video_encoder_->flvSequenceHeader();
        flv_formatter_->writeH264(sps_pps.data(), sps_pps.size(), true,
                                  AVC_SEQUENCE_HEADER, 0);
        output(flv_formatter_->data(), flv_formatter_->size());
    }
}

bool MediaProcessor::isActive() const
{
    return is_active_;
}

int MediaProcessor::lastTimestamp() const
{
    return last_timestamp_;
}

void MediaProcessor::stop()
{
    if(!is_active_) return;
    is_active_ = false;

    if(rtmp_streamer_) {
        rtmp_streamer_->close();
        delete rtmp_streamer_;
        rtmp_streamer_ = 0;
    }

    if(dest_file_) {
        dest_file_->close();
        delete dest_file_;
        dest_file_ = 0;
    }
}

void MediaProcessor::processAudioData(int timestamp_ms, QByteArray pcm_data)
{
    if(!audio_encoder_) return;

    audio_encoder_->write(pcm_data);
    QByteArray encoded = audio_encoder_->readAll();
    if(encoded != QByteArray()) {
        qDebug() << "audio:" << timestamp_ms << encoded.size();
        flv_formatter_->writeMP3(encoded.data(), encoded.size(), timestamp_ms);
    }

    output(flv_formatter_->data(), flv_formatter_->size());
}

void MediaProcessor::processVideoData(int timestamp_ms, QImage data)
{
    if(!video_encoder_) return;

    video_encoder_->encode(data);
    if(video_encoder_->getFrameSize() != 0) {
        qDebug() << "video:" << timestamp_ms << video_encoder_->getFrameSize();
        flv_formatter_->writeH264(
                    video_encoder_->getData(), video_encoder_->getFrameSize(),
                    video_encoder_->isKeyFrame(), AVC_NALU, timestamp_ms);
    }

    output(flv_formatter_->data(), flv_formatter_->size());
    last_timestamp_ = timestamp_ms;
}

void MediaProcessor::output(char *data, int size)
{
    if(rtmp_streamer_) {
        rtmp_streamer_->write(data, size);
    }

    if(dest_file_) {
        dest_file_->write(data, size);
    }
}
