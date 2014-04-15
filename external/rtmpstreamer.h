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

#ifndef RTMPSTREAMER_H
#define RTMPSTREAMER_H

#include <QIODevice>
#include <QByteArray>

class RTMP;

class RTMPStreamer : public QIODevice
{
    Q_OBJECT
public:
    explicit RTMPStreamer(QObject *parent = 0);
    virtual ~RTMPStreamer();
    virtual bool open(OpenMode mode) override;
    bool connectToServer(const QString &url);
    virtual void close() override;
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

private:
    RTMP *rtmp_;
    QByteArray url_;
};

#endif // RTMPSTREAMER_H
