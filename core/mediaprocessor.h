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

#ifndef MEDIAPROCESSOR_H
#define MEDIAPROCESSOR_H

#include <QObject>
#include <QSize>
#include <QImage>
#include <QString>
#include <QByteArray>

class VideoEncoder;
class AudioEncoder;
class FLVFormatter;
class RTMPStreamer;

class QFile;

class MediaProcessor : public QObject
{
    Q_OBJECT
public:
    explicit MediaProcessor(QObject *parent = 0);
    ~MediaProcessor();

    void EnableRTMPStreaming(const QString &url);
    void DisableRTMPStreaming();

    void EnableSaveToLocalFile(const QString &url);
    void DisableSaveToLocalFile();

    void setupAudioEncoder(int samplerate, int bitrate_kbps);
    void setupVideoEncoder(QSize image_size, int framerate, int bitrate_kbps, int preset_index);

public slots:
    void start();
    void stop();
    /* sample size is always 16 bit signed integer and pcm_data is interleaved */
    void processAudioData(int timestamp_ms, QByteArray pcm_data);
    void processVideoData(int timestamp_ms, QImage data);

private:
    void output(char *data, int size);

    VideoEncoder *video_encoder_;
    AudioEncoder *audio_encoder_;

    FLVFormatter *flv_formatter_;
    RTMPStreamer *rtmp_streamer_;
    QFile *dest_file_;

    QString rtmp_url_;
    QString file_url_;

    bool is_active_;
};

#endif // MEDIAPROCESSOR_H
