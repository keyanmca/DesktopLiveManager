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

#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <QObject>
#include <QList>
#include <QAudioDeviceInfo>

class QWidget;
class QComboBox;
class QTimer;
class QAudioInput;
class QAudioFormat;
class QIODevice;

class AudioController : public QObject
{
    Q_OBJECT
public:
    AudioController(QObject *parent = 0);
    ~AudioController();
    void setupUi(QComboBox *parent);
    void start();
    QByteArray data();
    int timestamp() const;
    void setSampleRate(int sample_rate);
    void stop();

private:
    QAudioInput *audio_;
    QAudioFormat *format_;
    QIODevice *device_;
    QList<QAudioDeviceInfo> available_devices_;
    QComboBox *devices_; // no ownership
};

#endif // AUDIOCONTROLLER_H
