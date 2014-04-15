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

#include "audiocontroller.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>

AudioController::AudioController(QObject *parent) :
    QObject(parent),
    audio_(0),
    format_(0),
    device_(0),
    devices_(0)
{
    format_ = new QAudioFormat;
    format_->setSampleRate(44100);
    format_->setChannelCount(2);
    format_->setSampleSize(16);
    format_->setByteOrder(QAudioFormat::LittleEndian);
    format_->setSampleType(QAudioFormat::UnSignedInt);
    format_->setCodec("audio/pcm");
}

AudioController::~AudioController()
{
    if(audio_) {
        audio_->stop();
    }
    delete format_;
}

void AudioController::setupUi(QComboBox *devices)
{
    devices_ = devices;
    available_devices_ = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(auto &d : available_devices_) {
        devices_->addItem(d.deviceName());
    }
}

void AudioController::start()
{
    audio_ = new QAudioInput(available_devices_.at(devices_->currentIndex()), *format_);
    device_ = audio_->start();
}

QByteArray AudioController::data()
{
    return device_->readAll();
}

void AudioController::setSampleRate(int sample_rate)
{
    format_->setSampleRate(sample_rate);
}

void AudioController::stop()
{
    audio_->stop();
    delete audio_;
    audio_ = 0;
    device_ = 0;
}
