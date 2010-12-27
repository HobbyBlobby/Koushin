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


#include "fielditem.h"
#include "field.h"
#include "GUI/townwidget.h"
#include <qpainter.h>

KoushinGUI::FieldItem::FieldItem(Koushin::Field* field)
  : m_field(field)
{
  setParentItem(field->getTown()->getTownWidget());
}

void KoushinGUI::FieldItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  if(m_field->getType() == Koushin::fieldWithForest)
    painter->setBrush(QBrush(Qt::green));
  else if(m_field->getType() == Koushin::fieldWithRocks)
    painter->setBrush(QBrush(Qt::gray));
  else if(m_field->getType() == Koushin::fieldWithBuilding)
    painter->setBrush(QBrush(Qt::white));
  else if(m_field->getType() == Koushin::fieldWithWater)
    painter->setBrush(QBrush(Qt::blue));
  else if(m_field->getType() == Koushin::plainField)
    painter->setBrush(QBrush(QColor(100, 255, 100)));

  if(m_field->isMarked()) {
    painter->setPen(QPen(QBrush(Qt::red), 0.1));
  } else {
    painter->setPen(QPen());
  }
  painter->drawRect(0, 0, 1, 1);
}

QRectF KoushinGUI::FieldItem::boundingRect() const
{
  return QRectF(0.0, 0.0, 1.0, 1.0);
}

#include <KDebug>
#include <building.h>
#include <player.h>
#include "buildinginfowidget.h"
void KoushinGUI::FieldItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  //deselect building:
  m_field->getTown()->getOwner()->setSelectedBuilding(0);
  //chosse, what is selected:
  if(m_field->getType() == Koushin::plainField) {
    m_field->getTown()->getTownWidget()->sendSignalTownClicked(QPoint((int)pos().x(), (int)pos().y()));
  }
  if(m_field->getType() == Koushin::fieldWithBuilding && m_field->getBuilding()) {
      m_field->getTown()->getOwner()->setSelectedBuilding(m_field->getBuilding());
  }
  m_field->getTown()->getOwner()->getBuidlingInfoWidget()->repaint();
  m_field->getTown()->unmarkAllFields();
  QGraphicsItem::mousePressEvent(event);
}

