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


#include "building.h"
#include <qpainter.h>
#include <KDebug>

Koushin::Building::Building(Town* town)
  : m_town(town)
  , m_name("NoName")
{
  m_textItem = new QGraphicsTextItem(m_name);
  m_textItem->setScale(1 / m_textItem->boundingRect().width());
  m_textItem->setPos(0, 0.5 - m_textItem->boundingRect().height()/(2*m_textItem->boundingRect().width()));
  m_textItem->setParentItem(this);
}

Koushin::Building::~Building()
{

}

const QMap< QString, Koushin::ActionProperties > Koushin::Building::getPossibleActions() const
{
  return Koushin::ActionObject::getPossibleActions();
}

const QString Koushin::Building::getLocal(QString name, QString additionalContent)
{
  if(m_globalReplacements.contains(name))
    return m_globalReplacements.value(name);
  if(m_globalAdditions.contains(name))
    additionalContent += m_globalAdditions.value(name);
  return m_town->getLocal(name, additionalContent);
}

void Koushin::Building::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(option)
  Q_UNUSED(widget)
  painter->drawEllipse(QRect(0,0,1,1));
  m_textItem->setPlainText(m_name);
  m_textItem->setScale(1 / m_textItem->boundingRect().width());
  m_textItem->setPos(0, 0.5 - m_textItem->boundingRect().height()/(2*m_textItem->boundingRect().width()));
}

QRectF Koushin::Building::boundingRect() const
{
  return QRectF(0.0, 0.0, 1.0, 1.0);
}
