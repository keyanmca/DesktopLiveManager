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

#ifndef FLVSTREAMER_H
#define FLVSTREAMER_H

enum AVCPacketType
{
    AVC_SEQUENCE_HEADER = 0,
    AVC_NALU            = 1,
    AVC_END_OF_SEQUENCE = 2
};


class FLVFormatter
{
public:
    FLVFormatter();
    ~FLVFormatter();
    void writeHeader();
    void writeScriptData(int width, int height, int video_bitrate, int framerate,
                         int audio_bitrate, int samplerate, int samplesize);
    void writeH264(const char *data, int size, bool isKeyFrame, AVCPacketType packet_type, int ts);
    void writeMP3(const char *data, int size, int ts);
    char* data();
    int size();
private:
    char *buffer_;
    int bSize_;
    int bReserved_;
};

#endif // FLVSTREAMER_H
