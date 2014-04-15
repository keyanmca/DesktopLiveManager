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

#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include <cstdint> // needed to include x264.h
extern "C" {
#include <x264.h>
}

#include <QByteArray>

class QSize;
class QImage;

class VideoEncoder
{
public:
    VideoEncoder(const QSize &imageSize, int fps, int bitrate, int preset_index);
    ~VideoEncoder() throw();
    QByteArray flvSequenceHeader();
    bool encode(const QImage &image); // RGB888
    int getFrameSize() const;
    bool isKeyFrame() const;
    const char* getData();
private:
    x264_param_t param;
    x264_t* encoder;
    x264_picture_t pic_in, pic_out;
    x264_nal_t* nals;
    int i_nals;
    int nals_size_;
};

#endif // VIDEOENCODER_H
