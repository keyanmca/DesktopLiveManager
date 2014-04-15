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

#include "videoproperty.h"
#include "ui_propertybase.h"
#include "ui_videoproperty.h"
#include "../item/video.h"

#include <QApplication>
#include <QStyle>
#include <QMediaPlayer>
#include <QDebug>

VideoProperty::VideoProperty(QWidget *parent) :
    PropertyBase(parent),
    video_ui(new Ui::VideoProperty),
    item_(0)
{
    QWidget *video = new QWidget;
    ui->toolBox->addItem(video, QString("Video"));
    video_ui->setupUi(video);

    QStyle *sty = QApplication::style();
    video_ui->v_play_->setIcon(sty->standardIcon(QStyle::SP_MediaPlay));
    video_ui->v_pause_->setIcon(sty->standardIcon(QStyle::SP_MediaPause));
    video_ui->v_stop_->setIcon(sty->standardIcon(QStyle::SP_MediaStop));

    connect(&updater_, SIGNAL(timeout()), this, SLOT(updateProgressBar()));
    connect(video_ui->v_progress_, SIGNAL(valueChanged(int)), this, SLOT(onSeek()));
    connect(video_ui->v_url_, SIGNAL(editingFinished()), this, SLOT(onUrl()));
    connect(video_ui->v_open_, SIGNAL(clicked()), this, SLOT(onOpen()));
    connect(video_ui->v_play_, SIGNAL(clicked()), this, SLOT(onPlay()));
    connect(video_ui->v_pause_, SIGNAL(clicked()), this, SLOT(onPause()));
    connect(video_ui->v_stop_, SIGNAL(clicked()), this, SLOT(onStop()));
    connect(video_ui->v_mute_, SIGNAL(stateChanged(int)), this, SLOT(onMute()));
    connect(video_ui->v_volume_, SIGNAL(valueChanged(int)), this, SLOT(onVolume()));
}

void VideoProperty::setItem(Video *item)
{
    reset();
    PropertyBase::setItem(item);
    item_ = item;
    updater_.start(1000);
}

void VideoProperty::reset()
{
    updater_.stop();
    item_ = 0;
    PropertyBase::reset();
}

void VideoProperty::update()
{
    if(!item_) return;
    lockWith([&]() {
        QMediaPlayer *player = item_->player();
        qint64 dur = player->duration();
        qint64 pos = player->position();
        video_ui->v_url_->setText(player->media().canonicalUrl().toString());
        video_ui->v_mute_->setChecked(player->isMuted());
        video_ui->v_progress_->setMaximum(dur);
        video_ui->v_progress_->setValue(pos);
        video_ui->v_volume_->setValue(player->volume());
    });
}

void VideoProperty::updateProgressBar()
{
    if(!item_) return;
    lockWith([&]() {
        QMediaPlayer *player = item_->player();
        qint64 dur = player->duration();
        qint64 pos = player->position();
        if(!video_ui->v_progress_->isSliderDown()) {
            video_ui->v_progress_->setMaximum(dur);
            video_ui->v_progress_->setValue(pos);
        }

        dur /= 1000; // millisec to sec
        pos /= 1000;
        video_ui->v_time_->setText(
                    QString("%1:%2/%3:%4")
                    .arg(pos / 60, 1, 10, QLatin1Char('0'))
                    .arg(pos % 60, 2, 10, QLatin1Char('0'))
                    .arg(dur / 60, 1, 10, QLatin1Char('0'))
                    .arg(dur % 60, 2, 10, QLatin1Char('0'))
                    );
    });
}

void VideoProperty::onSeek()
{
    if(!item_) return;
    lockWith([&]() {
        item_->player()->setPosition(video_ui->v_progress_->value());
    });
}

void VideoProperty::onUrl()
{
    if(!item_) return;
    lockWith([&]() {
        item_->player()->setMedia(QUrl(video_ui->v_url_->text()));
    });
}

void VideoProperty::onOpen()
{
    if(!item_) return;
    lockWith([&]() {
        item_->openFile();
        video_ui->v_url_->setText(item_->player()->media().canonicalUrl().toString());
    });
}

void VideoProperty::onPlay()
{
    if(!item_) return;
    item_->player()->play();
}

void VideoProperty::onPause()
{
    if(!item_) return;
    item_->player()->pause();
}

void VideoProperty::onStop()
{
    if(!item_) return;
    item_->player()->stop();
}

void VideoProperty::onMute()
{
    if(!item_) return;
    item_->player()->setMuted(video_ui->v_mute_->checkState());
}

void VideoProperty::onVolume()
{
    if(!item_) return;
    item_->player()->setVolume(video_ui->v_volume_->value());
}
