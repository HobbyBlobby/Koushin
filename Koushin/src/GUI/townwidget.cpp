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
  , m_townPixmap(0)
  , m_boundingRect(0, 0, fieldNumber, fieldNumber)
  , m_portionRect(m_boundingRect)
{
}

KoushinGUI::TownWidget::~TownWidget()
{

}

QRectF KoushinGUI::TownWidget::boundingRect() const
{
  return m_boundingRect;
}

#include <KDebug>
#include <qpainter.h>
#include <kconfig.h>
void KoushinGUI::TownWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(option)
  Q_UNUSED(widget)
  if(m_townPixmap) {
    painter->drawPixmap(m_boundingRect, *m_townPixmap, m_portionRect);
  }
}

void KoushinGUI::TownWidget::updateTownPixmap(KConfig* config)
{
  if(m_townPixmap) {
    delete m_townPixmap;
    m_boundingRect = QRect(0,0,fieldNumber,fieldNumber);
    m_townPixmap = 0; //set Pointer to 0 if something happens with creating a new QPixmap
  }
//open file:
  KStandardDirs dir;
  KConfigGroup imageGroup = config->group("Image");
//read file:
  QString imageName = imageGroup.readEntry("image", QString());
  QString fileName = dir.findResource("data", "koushin/data/towns/" + imageName);
  if(fileName.isEmpty()) return; //do not create pixmap if file not found
//read parameters for rendering:
  int fieldsHorizontal = imageGroup.readEntry("fieldsHorizontal", int(-1));
  int fieldsVertical = imageGroup.readEntry("fieldsVertical", int(-1));
  
  m_townPixmap = new QPixmap(fileName);
  if(m_townPixmap->height() + m_townPixmap->width() == 0) {//empty image
    m_townPixmap = 0;
    return;
  }
//use proper standard paremters
  int fieldSize = 1;
  if(fieldsHorizontal + fieldsVertical == -2) {
    fieldsHorizontal = fieldNumber;
    fieldSize = m_townPixmap->width() / fieldsHorizontal;
    fieldsVertical = m_townPixmap->height() / fieldSize;
  } else if(fieldsHorizontal == -1) {
    fieldSize = m_townPixmap->height() / fieldsVertical;
    fieldsHorizontal = m_townPixmap->width() / fieldSize;
  } else if(fieldsVertical == -1) {
    fieldSize = m_townPixmap->width() / fieldsHorizontal;
    fieldsVertical = m_townPixmap->height() / fieldSize;
  } else {
    fieldSize = qMin(m_townPixmap->width() / fieldsHorizontal, m_townPixmap->height() / fieldsVertical);
  }
  
//   m_boundingRect = QRect(3, 3, 10, 10);
  m_boundingRect = QRect(0, 0, fieldsHorizontal, fieldsVertical);
  m_portionRect = QRect(0, 0, fieldSize*fieldsHorizontal, fieldSize*fieldsVertical);
}

// void KoushinGUI::TownWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
// {
//   kDebug() << "moving";
// }
// void KoushinGUI::TownWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
// {
//   kDebug() << "clicking";
// }


#include "townwidget.moc"