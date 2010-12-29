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
#include <KDebug>
#include <building.h>
#include <player.h>
#include "buildinginfowidget.h"
#include <QTextItem>

KoushinGUI::FieldItem::FieldItem(Koushin::Field* field)
  : m_field(field)
  , m_textItem(new QGraphicsTextItem())
  , m_markColor(Qt::red)
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
  else if(m_field->getType() == Koushin::fieldNotUsable)
    painter->setBrush(QBrush(Qt::darkGray));
  else if(m_field->getType() == Koushin::plainField)
    painter->setBrush(QBrush(QColor(200, 255, 130)));

  painter->setOpacity(0.3);
  painter->drawRect(0, 0, 1, 1);
  
  if(m_field->getType() == Koushin::fieldWithBuilding && m_field->getBuilding()) {
    painter->setPen(QPen());
    painter->setOpacity(1.0);
    painter->drawEllipse(0,0,1,1);
    m_textItem->setPlainText(m_field->getBuilding()->getName());
    m_textItem->setScale(1 / m_textItem->boundingRect().width());
    m_textItem->setPos(0, 0.5 - m_textItem->boundingRect().height()/(2*m_textItem->boundingRect().width()));
    m_textItem->setParentItem(this);
  }
    
  if(m_field->isMarked()) {
    painter->setOpacity(0.5);
    painter->setBrush(QBrush(m_markColor));
    painter->setPen(QPen());
    painter->drawRect(0,0,1,1);
  }
}

QRectF KoushinGUI::FieldItem::boundingRect() const
{
  return QRectF(0.0, 0.0, 1.0, 1.0);
}

void KoushinGUI::FieldItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  m_field->getTown()->getOwner()->fieldClicked(m_field);
  QGraphicsItem::mousePressEvent(event);
}

