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

#ifndef SCREENCAPTUREPROPERTY_H
#define SCREENCAPTUREPROPERTY_H

#include "propertybase.h"

class ScreenCapture;

namespace Ui {
class ScreenCaptureProperty;
}

class ScreenCaptureProperty : public PropertyBase
{
    Q_OBJECT
public:
    ScreenCaptureProperty(QWidget *parent = 0);
    void setItem(ScreenCapture *item);
    void reset() override;
    void update() override;

private slots:
    void onIncludeCursor();
    void onScaleMode();
    void onFillByScaling();
    void onFillByExpandingCaptureArea();
    void onFitByScaling();
    void onFullScreen();
    void onSelectCapturedArea();
    void onCX();
    void onCY();
    void onCWidth();
    void onCHeight();

private:
    // This class doesn't have any onwership of private members
    Ui::ScreenCaptureProperty *sc_ui;
    ScreenCapture* item_;
};

#endif // SCREENCAPTUREPROPERTY_H
