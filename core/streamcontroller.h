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

#ifndef STREAMCONTROLLER_H
#define STREAMCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QTime>
#include <QSize>
#include <QImage>
#include <QByteArray>

class GraphicsController;
class AudioController;
class MediaProcessor;

namespace Ui {
    class GeneralSetting;
    class EncoderSetting;
}

class StreamController : public QObject
{
    Q_OBJECT

public:
    explicit StreamController(GraphicsController *graphics, AudioController *audio,
                              QWidget *general_setting, QWidget *encoder_setting, QObject *parent = 0);
    ~StreamController();
    bool start();
    void stop();
    bool isActive();
    int elapsed();

signals:
    void requestProcessVideoData(int timestamp_ms, QImage data);
    void requestProcessAudioData(int timestamp_ms, QByteArray pcm_data);
    void requestStop();

private slots:
    void process();
    void onOpenSaveFile();

    // video encoder
    void onSizeChanged();
    void setFramerate(int framerate);

private:
    // video encoder
    void setSize(const QSize &size);
    void setFramePeriod(int period);

    Ui::GeneralSetting *general_ui;
    Ui::EncoderSetting *encoder_ui;

    GraphicsController *graphics_; // no ownership
    AudioController *audio_; // no ownership
    MediaProcessor *processor_;
    QThread media_process_thread_;

    bool is_active_;
    int target_frame_period_;
    int frame_period_;
    QTimer update_timer_;
    QTime time_;

    QSize image_size_;
    QImage rendered_;
};

#endif // STREAMCONTROLLER_H
