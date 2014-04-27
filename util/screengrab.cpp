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

#include "screengrab.h"
#include "mousecursor.h"

#include <QImage>
#include <QPainter>
#include <QScreen>
#include <QTime>
#include <QMetaType>
#include <QDebug>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

struct ScreenGrabPrivate
{
    ScreenGrabPrivate() : display_(0), screen_(0), shminfo_(0), image_(0)
    {}
    Display *display_;
    int screen_;
    XShmSegmentInfo *shminfo_;
    XImage *image_;
};

Q_DECLARE_METATYPE(Qt::AspectRatioMode)
Q_DECLARE_METATYPE(Qt::TransformationMode)

ScreenGrab::ScreenGrab(QObject *parent) :
    QObject(parent),
    cursor_(new MouseCursor),
    d_(new ScreenGrabPrivate)
{
    qRegisterMetaType<Qt::AspectRatioMode>();
    qRegisterMetaType<Qt::TransformationMode>();

    d_->shminfo_ = new XShmSegmentInfo;

    d_->display_ = XOpenDisplay(0);
    d_->screen_ = XDefaultScreen(d_->display_);
    Q_ASSERT(XShmQueryExtension(d_->display_));

    d_->image_ = XShmCreateImage(
                d_->display_,
                DefaultVisual(d_->display_, d_->screen_),
                DefaultDepth(d_->display_, d_->screen_),
                ZPixmap,
                NULL,
                d_->shminfo_,
                XDisplayWidth(d_->display_, d_->screen_),
                XDisplayHeight(d_->display_, d_->screen_));

    d_->shminfo_->shmid = shmget(IPC_PRIVATE,
                                 d_->image_->bytes_per_line * d_->image_->width,
                                 IPC_CREAT | 0777);
    d_->shminfo_->shmaddr = (char*)shmat(d_->shminfo_->shmid, 0, 0);
    d_->shminfo_->readOnly = False;
    d_->image_->data = d_->shminfo_->shmaddr;
    bool is_ok = XShmAttach(d_->display_, d_->shminfo_);
    Q_ASSERT(is_ok);
}

ScreenGrab::~ScreenGrab()
{
    XShmDetach(d_->display_, d_->shminfo_);
    shmdt(d_->shminfo_->shmaddr);
    shmctl(d_->shminfo_->shmid, IPC_RMID, 0);
    XDestroyImage(d_->image_);
    XCloseDisplay(d_->display_);
    delete d_;
    delete cursor_;
}

void ScreenGrab::capture(
        QScreen *screen,
        QRect captured, QSize target, bool include_cursor,
        Qt::AspectRatioMode AR_mode, Qt::TransformationMode TF_mode)
{
    if(captured.width() != d_->image_->width ||
            captured.height() != d_->image_->height) {
        XDestroyImage(d_->image_);
        d_->image_ = XShmCreateImage(
                    d_->display_,
                    DefaultVisual(d_->display_, d_->screen_),
                    DefaultDepth(d_->display_, d_->screen_),
                    ZPixmap,
                    NULL,
                    d_->shminfo_,
                    captured.width(),
                    captured.height());
        d_->image_->data = d_->shminfo_->shmaddr;
    }

    QPoint topleft = captured.topLeft();
    XShmGetImage(d_->display_,
                 RootWindow(d_->display_, DefaultScreen(d_->display_)),
                 d_->image_, topleft.x(), topleft.y(), AllPlanes);
    QPixmap pixmap = QPixmap::fromImage(
                QImage((uchar*)d_->image_->data,
                       d_->image_->width, d_->image_->height,
                       QImage::Format_ARGB32));

    if(include_cursor) {
        QPainter p(&pixmap);
        cursor_->update();
        p.drawImage(cursor_->pos() - captured.topLeft(), cursor_->image());
    }

    if(target == captured.size()) {
        emit pixmapReady(pixmap);
    } else {
        emit pixmapReady(pixmap.scaled(target, AR_mode, TF_mode));
    }
}
