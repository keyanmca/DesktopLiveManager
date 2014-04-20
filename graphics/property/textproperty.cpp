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

#include "textproperty.h"
#include "ui_propertybase.h"
#include "ui_textproperty.h"
#include "../item/text.h"

#include <QFont>
#include <QFontDialog>
#include <QColorDialog>

TextProperty::TextProperty(QWidget *parent) :
    PropertyBase(parent),
    text_ui(new Ui::TextProperty),
    item_(0)
{
    ui->l_aspectratio_->setEnabled(false);
    ui->l_width_->setMinimum(10);
    ui->l_height_->setEnabled(false);

    QWidget *text = new QWidget;

    ui->toolBox->addItem(text, QString("Text"));
    text_ui->setupUi(text);

    connect(text_ui->choose_font_, SIGNAL(clicked()), this, SLOT(onChooseFont()));
    connect(text_ui->choose_color_, SIGNAL(clicked()), this, SLOT(onChooseColor()));
    connect(text_ui->text_, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited()));
}

void TextProperty::setItem(Text *item)
{
    reset();
    PropertyBase::setItem(item);
    item_ = item;
}

void TextProperty::reset()
{
    item_ = 0;
    PropertyBase::reset();
}

void TextProperty::update()
{
    if(!item_) return;
    lockWith([&]() {
        text_ui->font_->setText(item_->font().toString());
        text_ui->color_->setText(item_->defaultTextColor().name());
        text_ui->text_->setText(item_->toPlainText());
    });

    PropertyBase::update();
}

void TextProperty::onLWidth()
{
    if(!item_) return;
    lockWith([&]() {
        item_->setTextWidth(ui->l_width_->value());
        item_->updatePos();
    });
    update();
}

void TextProperty::onChooseFont()
{
    if(!item_) return;
    lockWith([&]() {
        bool ok = false;
        QFont font = QFontDialog::getFont(&ok);
        if(ok) {
            item_->setFont(font);
        }
    });
}

void TextProperty::onChooseColor()
{
    if(!item_) return;
    lockWith([&]() {
        QColor color = QColorDialog::getColor(item_->defaultTextColor());
        if(color.isValid()) {
            item_->setDefaultTextColor(color);
        }
    });
}

void TextProperty::onTextEdited()
{
    if(!item_) return;
    lockWith([&]() {
        item_->setPlainText(text_ui->text_->text());
    });
}
