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

#ifndef PROPERTYBASE_H
#define PROPERTYBASE_H

#include <QWidget>
#include <QGraphicsItem>

class IGraphicsItem;

namespace Ui {
class PropertyBase;
}

class PropertyBase : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyBase(QWidget *parent = 0);
    virtual ~PropertyBase();
    void setItem(IGraphicsItem* item);
    virtual void reset();
    virtual void update();
    static PropertyBase* nullProperty();

protected:
    template <class F> void lockWith(F func) // lock for avoidng recursion
    {
        if(lock_) return;
        lock_ = true;
        func();
        lock_ = false;
    }

    Ui::PropertyBase *ui;

protected slots:
    void onLPos(int index);
    void onLX();
    void onLY();
    void onLAspectRatioMode(int index);
    virtual void onLWidth();
    void onLHeight();
    virtual void onLDefaultSize();
    void onOpacity();

private:
    bool lock_;
    IGraphicsItem *item_; // no ownership
    QGraphicsItem *qitem_; // no ownership

private slots:
    virtual void updateEventFilter(QGraphicsItem::GraphicsItemChange change, const QVariant& value);
};

#endif // PROPERTYBASE_H
