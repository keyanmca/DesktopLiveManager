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

#ifndef AUDIOENCODER_H
#define AUDIOENCODER_H

#include <lame/lame.h>

#include <QIODevice>

class AudioEncoder : public QIODevice
{
    Q_OBJECT
public:
    explicit AudioEncoder(int sample_rate, int bit_rate, QObject *parent = 0);
    ~AudioEncoder();
    bool open(OpenMode mode) override;
    void close() override;
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

signals:

public slots:
private:
    lame_t lame_;

    int sample_rate_;
    int bit_rate_;

    static const int BUFFER_SIZE = 8192;
    char mp3Buffer_[BUFFER_SIZE];
    int occupied_;
};

#endif // AUDIOENCODER_H
