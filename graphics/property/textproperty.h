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

#ifndef TEXTPROPERTY_H
#define TEXTPROPERTY_H

#include "propertybase.h"

class Text;

namespace Ui {
class TextProperty;
}

class TextProperty : public PropertyBase
{
    Q_OBJECT
public:
    TextProperty(QWidget *parent = 0);
    void setItem(Text *item);
    void reset() override;
    void update() override;

protected:
    void onLWidth() override;

private slots:
    void onChooseFont();
    void onChooseColor();
    void onTextEdited();

private:
    Ui::TextProperty *text_ui; // no ownership
    Text *item_; // no ownership
};

#endif // TEXTPROPERTY_H
