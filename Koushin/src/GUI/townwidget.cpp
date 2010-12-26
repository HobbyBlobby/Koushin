/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "townwidget.h"

#include <QBrush>
#include <QColor>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

#include "building.h"
#include <QGraphicsScene>

KoushinGUI::TownWidget::TownWidget(QGraphicsItem* parent)
  : QGraphicsItem(parent)
{
  QGraphicsRectItem* rect = new QGraphicsRectItem(0.0, 0.0, 10.0, 10.0, this);
  rect->setBrush(Qt::cyan);
  for (int i = 1; i < 10; ++i) {
    new QGraphicsLineItem(0.0, i, 10.0, i, this);
    new QGraphicsLineItem(i, 0.0, i, 10.0, this);
  }
//  setGeometry(0, 0, 10, 10);
}

KoushinGUI::TownWidget::~TownWidget()
{

}

QRectF KoushinGUI::TownWidget::boundingRect() const
{
  return QRectF(0, 0, 10, 10);
}

void KoushinGUI::TownWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(painter)
  Q_UNUSED(option)
  Q_UNUSED(widget)
}


#include <kdebug.h>
void KoushinGUI::TownWidget::drawBuildings(QMap< Koushin::Building*, QPoint > buildingMap)
{
  if(!buildingMap.isEmpty()) {
    foreach(Koushin::Building* building, buildingMap.keys()) {
      building->setParentItem(this); 
      building->setPos(buildingMap.value(building));
    }
  }
}

void KoushinGUI::TownWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QPointF pos = event->pos();
  emit townClicked(QPoint((int)pos.x(), (int)pos.y()));
  QGraphicsItem::mousePressEvent(event);
}

#include "townwidget.moc"