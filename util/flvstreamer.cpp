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

#include "flvstreamer.h"

#include <QIODevice>
#include <QByteArray>
#include <QtEndian>

#include <cstdint>

union intfloat64{
    uint64_t i;
    double f;
};

typedef enum
{
    AMF_DATA_TYPE_NUMBER      = 0x00,
    AMF_DATA_TYPE_BOOL        = 0x01,
    AMF_DATA_TYPE_STRING      = 0x02,
    AMF_DATA_TYPE_OBJECT      = 0x03,
    AMF_DATA_TYPE_NULL        = 0x05,
    AMF_DATA_TYPE_UNDEFINED   = 0x06,
    AMF_DATA_TYPE_REFERENCE   = 0x07,
    AMF_DATA_TYPE_MIXEDARRAY  = 0x08,
    AMF_DATA_TYPE_OBJECT_END  = 0x09,
    AMF_DATA_TYPE_ARRAY       = 0x0a,
    AMF_DATA_TYPE_DATE        = 0x0b,
    AMF_DATA_TYPE_LONG_STRING = 0x0c,
    AMF_DATA_TYPE_UNSUPPORTED = 0x0d,
} AMFDataType;


inline void w8(QByteArray *array, uint8_t d)
{
    array->append(d);
}

inline void wb16(QByteArray *array, uint16_t d)
{
    uint16_t be = qToBigEndian(d);
    array->append((char*)&be, sizeof(be));
}

inline void wb24(QByteArray *array, uint32_t d)
{
    uint32_t be = qToBigEndian(d);
    array->append(((char*)&be) + 1, sizeof(be) - 1);
}

inline void wb32(QByteArray *array, uint32_t d)
{
    uint32_t be = qToBigEndian(d);
    array->append((char*)&be, sizeof(be));
}

inline void wb64(QByteArray *array, unsigned long long d)
{
    static_assert(sizeof(d) == 8, "long long is not 64bit integer");
    unsigned long long be = qToBigEndian(d);
    array->append((char*)&be, sizeof(be));
}

void putAmfString(QByteArray *array, const char *str)
{
    uint16_t size = qstrlen(str);
    wb16(array, size);
    array->append(str);
}

void putAmfDouble(QByteArray *array, double f)
{
    w8(array, AMF_DATA_TYPE_NUMBER);
    union intfloat64 v;
    v.f = f;
    wb64(array, v.i);
}

void putAmfBool(QByteArray *array, bool b)
{
    w8(array, AMF_DATA_TYPE_BOOL);
    w8(array, b);
}

// FLVFormatter Class
FLVFormatter::FLVFormatter() :
    buffer_(0),
    bSize_(0),
    bReserved_(8192)
{
    buffer_  = new char[bReserved_];
}

FLVFormatter::~FLVFormatter()
{
    delete[] buffer_;
}

void FLVFormatter::writeHeader()
{
    bSize_ = 0;

    buffer_[0] = 'F';
    buffer_[1] = 'L';
    buffer_[2] = 'V';
    buffer_[3] = 1; // version
    buffer_[4] = 0;
    buffer_[4] |= (1 << 2); // enable audio
    buffer_[4] |= (1 << 0); // enable video // debug
    buffer_[5] = 0; // data offset (4bytes)
    buffer_[6] = 0;
    buffer_[7] = 0;
    buffer_[8] = 9;
    buffer_[9] = 0; // previous tag size (4bytes)
    buffer_[10] = 0;
    buffer_[11] = 0;
    buffer_[12] = 0;

    bSize_ += 13;
}

void FLVFormatter::writeScriptData(int width, int height, int video_bitrate, int framerate,
                                  int audio_bitrate, int samplerate, int samplesize)
{
    QByteArray buf;
    buf.reserve(1024);

    w8(&buf, 18); // tag type META
    int metadata_size_pos = buf.size();
    wb24(&buf, 0); // size of data part
    wb24(&buf, 0); // timestamp
    wb32(&buf, 0); // reserved

    int data_tags_begin_pos = buf.size();
    w8(&buf, AMF_DATA_TYPE_STRING);
    putAmfString(&buf, "onMetaData");

    w8(&buf, AMF_DATA_TYPE_MIXEDARRAY);
    int metadata_count =
            5 + // video
            5; // audio
            //1 + // data
            //2;  // duration and file size
    wb32(&buf, metadata_count);

    //putAmfString(&buf, "duration");
    //putAmfDouble(&buf, 0);

    // Video
    putAmfString(&buf, "width");
    putAmfDouble(&buf, width);

    putAmfString(&buf, "height");
    putAmfDouble(&buf, height);

    putAmfString(&buf, "videodatarate");
    putAmfDouble(&buf, video_bitrate);

    putAmfString(&buf, "framerate");
    putAmfDouble(&buf, framerate);

    putAmfString(&buf, "videocodecid");
    putAmfDouble(&buf, 7); // avc

    // Audio
    putAmfString(&buf, "audiodatarate");
    putAmfDouble(&buf, audio_bitrate);

    putAmfString(&buf, "audiosamplerate");
    putAmfDouble(&buf, samplerate);

    putAmfString(&buf, "audiosamplesize");
    putAmfDouble(&buf, samplesize);

    putAmfString(&buf, "stereo");
    putAmfDouble(&buf, 2);

    putAmfString(&buf, "audiocodecid");
    putAmfDouble(&buf, 2); // mp3

    // Data enc
    //putAmfStrin&g(buf, "datastream");
    //putAmfDoubl&e(buf, 0);

    //putAmfString(&buf, "filesize");
    //putAmfDouble(&buf, 0);

    // List termi&nator
    putAmfString(&buf, "");
    w8(&buf, 9);

    int data_tags_size = buf.size() - data_tags_begin_pos;
    QByteArray size;
    wb24(&size, data_tags_size);
    buf.replace(metadata_size_pos, size.size(), size);

    // previous tag size
    wb32(&buf, data_tags_size + 11);
}

void FLVFormatter::writeH264(const char *data, int size, bool isKeyFrame, AVCPacketType packet_type, int ts)
{
    if(size <= 0) return;

    // reserve buffer
    bSize_ = 0;
    int expected = size + 32;
    if(expected > bReserved_) {
        delete[] buffer_;
        while(expected > bReserved_) {
            bReserved_ *= 2;
        }
        buffer_ = new char[bReserved_];
    }

    // write FLV TAG HEADER (11bytes)
    const int dataSize = size + 5; // VIDEO TAG SIZE
    buffer_[0] = 9; // video type
    buffer_[1] = 0xff & (dataSize >> 16); // data size (3bytes)
    buffer_[2] = 0xff & (dataSize >> 8);
    buffer_[3] = 0xff & (dataSize);
    buffer_[4] = 0xff & (ts >> 16); // timestamp (3bytes)
    buffer_[5] = 0xff & (ts >> 8);
    buffer_[6] = 0xff & (ts);
    buffer_[7] = 0xff & (ts >> 24);   // extended timestamp
    buffer_[8] = 0; // stream id (3bytes) is ALWAYS 0
    buffer_[9] = 0;
    buffer_[10] = 0;
    bSize_ += 11;

    // write FLV VIDEO TAG HEADER (5bytes)
    buffer_[11] = ((isKeyFrame ? 1 : 2) << 4) | 7; // 7 = AVC(h246)
    buffer_[12] = packet_type; // AVC NALU
    buffer_[13] = 0; // composition time offset (3bytes)
    buffer_[14] = 0;
    buffer_[15] = 0;
    bSize_ += 5;

    // write VIDEO TAG BODY (video data)
    char *buf = buffer_ + bSize_;
    for(int i = 0; i < size; ++i) {
        buf[i] = data[i];
    }
    bSize_ += size;

    // write PreviousTagSize
    buf = buffer_ + bSize_;
    buf[0] = 0xff & (bSize_ >> 24);
    buf[1] = 0xff & (bSize_ >> 16);
    buf[2] = 0xff & (bSize_ >> 8);
    buf[3] = 0xff & (bSize_);
    bSize_ += 4;
}

void FLVFormatter::writeMP3(const char *data, int size, int ts)
{
    if(size <= 0) return;

    // reserve buffer
    bSize_ = 0;
    int expected = size + 32;
    if(expected > bReserved_) {
        delete[] buffer_;
        while(expected > bReserved_) {
            bReserved_ *= 2;
        }
        buffer_ = new char[bReserved_];
    }

    // write FLV TAG HEADER (11bytes)
    const int dataSize = size + 1; // VIDEO TAG SIZE
    buffer_[0] = 8; // audio type
    buffer_[1] = 0xff & (dataSize >> 16); // data size (3bytes)
    buffer_[2] = 0xff & (dataSize >> 8);
    buffer_[3] = 0xff & (dataSize);
    buffer_[4] = 0xff & (ts >> 16); // timestamp (3bytes)
    buffer_[5] = 0xff & (ts >> 8);
    buffer_[6] = 0xff & (ts);
    buffer_[7] = 0xff & (ts >> 24);   // extended timestamp
    buffer_[8] = 0; // stream id (3bytes) is ALWAYS 0
    buffer_[9] = 0;
    buffer_[10] = 0;
    bSize_ += 11;

    // write FLV AUDIO TAG HEADER (1byte)
    buffer_[11] = 0;
    buffer_[11] |= (2 << 4); // MP3
    buffer_[11] |= (3 << 2); // 44khz
    buffer_[11] |= (1 << 1); // 16-bit samples
    buffer_[11] |= 1; // stereo sound
    bSize_ += 1;

    // write AUDIO TAG BODY (audio data)
    char *buf = buffer_ + bSize_;
    for(int i = 0; i < size; ++i) {
        buf[i] = data[i];
    }
    bSize_ += size;

    // write PreviousTagSize
    buf = buffer_ + bSize_;
    buf[0] = 0xff & (bSize_ >> 24);
    buf[1] = 0xff & (bSize_ >> 16);
    buf[2] = 0xff & (bSize_ >> 8);
    buf[3] = 0xff & (bSize_);
    bSize_ += 4;
}

char *FLVFormatter::data()
{
    return buffer_;
}

int FLVFormatter::size()
{
    return bSize_;
}
