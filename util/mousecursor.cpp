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

#include "mousecursor.h"

#include <QImage>
#include <QCursor>

#ifdef Q_OS_LINUX
    #include <QX11Info>
    extern "C" {
        #include <X11/Xlib.h>
        #include <X11/extensions/Xfixes.h>
    }
#endif

MouseCursor::MouseCursor() :
    pixmap_data_(0),
    reserved_(1024)
{
    pixmap_data_ = new uint32_t[1024];
}

MouseCursor::~MouseCursor()
{
    delete pixmap_data_;
}

void MouseCursor::update()
{
#ifdef Q_OS_LINUX
    XFixesCursorImage *img = XFixesGetCursorImage(QX11Info::display());

    // realloc enough buffer
    size_ = QSize(img->width, img->height);
    int pixels = size_.width() * size_.height();
    if(pixels > reserved_) {
        delete pixmap_data_;
        reserved_ = pixels;
        pixmap_data_ = new uint32_t[reserved_];
    }

    /* copy Data removing redundant bits if needed
     * Note: sizeof(unsigned long) is 8 bytes in 64bit unix,
     *       but only bottom 4 bytes are used. (00000000AARRGGBB)
     *       So we have to remove top 4 bytes.
     */
    unsigned long *data = img->pixels;
    for(int i = 0; i < pixels; ++i) {
        pixmap_data_[i] = data[i];
    }

    // calc mouse pos
    pos_ = QCursor::pos();
    pos_ -= QPoint(img->xhot, img->yhot);

    free(img);
#else
    return;
    // TODO: write Platform dependent code for other than X11
    //       such as MS_Windows and Mac_OSX
#endif

}

QImage MouseCursor::image()
{
    return QImage((uchar*)pixmap_data_, size_.width(), size_.height(),
                  QImage::Format_ARGB32_Premultiplied);
}

QPoint MouseCursor::pos()
{
    return pos_;
}
