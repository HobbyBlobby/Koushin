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
#include "field.h"
#include <QGraphicsScene>
#include <KStandardDirs>

KoushinGUI::TownWidget::TownWidget(QGraphicsItem* parent)
  : QGraphicsItem(parent)
{
}

KoushinGUI::TownWidget::~TownWidget()
{

}

QRectF KoushinGUI::TownWidget::boundingRect() const
{
  return QRectF(0, 0, fieldNumber, fieldNumber);
}

#include <KDebug>
#include <qpainter.h>
void KoushinGUI::TownWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(option)
  Q_UNUSED(widget)
  ///temporary solution for testing the image
  KStandardDirs dir;
  QString fileName = dir.findResource("data", "koushin/data/images/landscape.jpg");
  QPixmap* pixmap = new QPixmap(fileName);
  painter->drawPixmap(QRect(0, 0, fieldNumber, fieldNumber), *pixmap, QRect(0.0, 0.0, 1024, 1024));
}


#include <kdebug.h>
// void KoushinGUI::TownWidget::drawBuildings(QMap< Koushin::Building*, QPoint > buildingMap)
// {
//   if(!buildingMap.isEmpty()) {
//     foreach(Koushin::Building* building, buildingMap.keys()) {
//       building->setParentItem(this); 
//       building->setPos(buildingMap.value(building));
//     }
//   }
// }

// void KoushinGUI::TownWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
// {
//   QPointF pos = event->pos();
//   emit townClicked(QPoint((int)pos.x(), (int)pos.y()));
//   QGraphicsItem::mousePressEvent(event);
// }

#include "townwidget.moc"