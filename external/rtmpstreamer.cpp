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

#include "rtmpstreamer.h"
#include <librtmp/rtmp.h>
#include <librtmp/log.h>

#include <QDebug>

RTMPStreamer::RTMPStreamer(QObject *parent) :
    QIODevice(parent),
    rtmp_(0)
{
    RTMP_LogSetLevel(RTMP_LOGDEBUG);
}

RTMPStreamer::~RTMPStreamer()
{
    close();
}

bool RTMPStreamer::open(QIODevice::OpenMode mode)
{
    if(mode == WriteOnly) {
        if(!rtmp_) {
            rtmp_ = RTMP_Alloc();
            RTMP_Init(rtmp_);
        }
        return QIODevice::open(mode);
    }
    return false;
}

bool RTMPStreamer::connectToServer(const QString &url)
{
    url_ = url.toUtf8();
    if(!RTMP_SetupURL(rtmp_, url_.data())) {
        qDebug() << "failed RTMP_SetupURL";
        return false;
    }
    RTMP_EnableWrite(rtmp_);

    if(!RTMP_Connect(rtmp_, NULL)) {
        qDebug() << "failed RTMP_Connect";
        return false;
    }
    if(!RTMP_ConnectStream(rtmp_, 0)) {
        qDebug() << "failed RTMP_ConnectStream";
        return false;
    }

    return true;
}

void RTMPStreamer::close()
{
    if(rtmp_) {
        RTMP_Close(rtmp_);
        RTMP_Free(rtmp_);
        rtmp_ = 0;
    }
}

qint64 RTMPStreamer::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    return maxlen;
}

qint64 RTMPStreamer::writeData(const char *data, qint64 len)
{
    RTMP_Write(rtmp_, data, len);
    return len;
}
