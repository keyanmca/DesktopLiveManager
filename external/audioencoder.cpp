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

#include "audioencoder.h"

#include <QDebug>

#include <algorithm>

using namespace std;

AudioEncoder::AudioEncoder(int sample_rate, int bit_rate, QObject *parent) :
    QIODevice(parent),
    sample_rate_(sample_rate),
    bit_rate_(bit_rate),
    occupied_(0)
{
    // initialize lame encoder
    lame_ = lame_init();
}

AudioEncoder::~AudioEncoder()
{
    //int bytes = lame_encode_flush(lame, (uchar*)mp3Buffer, BUFFER_SIZE);
    close();
}

bool AudioEncoder::open(OpenMode mode)
{
    if(mode == ReadWrite) { // force ReadWrite
        lame_set_in_samplerate(lame_, sample_rate_);
        lame_set_VBR(lame_, vbr_default);
        lame_set_VBR_max_bitrate_kbps(lame_, bit_rate_);
        lame_init_params(lame_);
        return QIODevice::open(mode);
    }
    return false;
}

void AudioEncoder::close()
{
    lame_close(lame_);
    QIODevice::close();
}

qint64 AudioEncoder::readData(char *data, qint64 maxSize)
{
    int copied = min((int)maxSize, occupied_);
    copy_n(mp3Buffer_, copied, data);
    occupied_ -= copied;
    copy_n(&mp3Buffer_[copied], occupied_, mp3Buffer_); // truncate copied data
    return copied;
}

qint64 AudioEncoder::writeData(const char *data, qint64 maxSize)
{
    occupied_ += lame_encode_buffer_interleaved(
                lame_, (short int*)data, maxSize/4,
                (uchar*)&mp3Buffer_[occupied_], BUFFER_SIZE - occupied_);
    return maxSize;
}
