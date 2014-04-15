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

#ifndef VIDEOPROPERTY_H
#define VIDEOPROPERTY_H

#include "propertybase.h"
#include <QTimer>

class Video;

namespace Ui {
class VideoProperty;
}

class VideoProperty : public PropertyBase
{
    Q_OBJECT
public:
    VideoProperty(QWidget *parent = 0);
    void setItem(Video *item);
    void reset() override;
    void update() override;

private slots:
    void updateProgressBar();
    void onSeek();
    void onUrl();
    void onOpen();
    void onPlay();
    void onPause();
    void onStop();
    void onMute();
    void onVolume();

private:
    Ui::VideoProperty *video_ui; // no ownership
    Video *item_; // no ownership
    QTimer updater_;
};

#endif // VIDEOPROPERTY_H
