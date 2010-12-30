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


#include "field.h"
#include "GUI/townwidget.h"
#include <qgraphicsitem.h>
#include <QGraphicsRectItem>
#include <QBrush>
#include <kdebug.h>

Koushin::Field::Field(Koushin::Town* town, Koushin::FieldType type)
  : m_type(type)
  , m_town(town)
  , m_building(0)
  , m_fieldItem(new KoushinGUI::FieldItem(this))
  , m_isMarked(false)
{
}

Koushin::Field::Field(const Koushin::Field& oldField)
  : m_type(oldField.getType())
  , m_town(oldField.getTown())
  , m_resources(oldField.getResources())
  , m_building(oldField.getBuilding())
  , m_fieldItem(oldField.getFieldItem())
  , m_isMarked(oldField.isMarked())
{
  kDebug() << "Field is copied";
}


QString Koushin::Field::fieldTypeToQString(Koushin::FieldType type)
{
  if(type == Koushin::plainField)
    return "plainField";
  if(type == Koushin::fieldWithBuilding)
    return "fieldWithBuilding";
  if(type == Koushin::fieldWithForest)
    return "fieldWithForest";
  if(type == Koushin::fieldWithRocks)
    return "fieldWithRocks";
  if(type == Koushin::fieldWithWater)
    return "fieldWithWater";
  if(type == Koushin::fieldNotUsable)
    return "fieldNotUsable";
  return "";
}

Koushin::FieldType Koushin::Field::QStringToFieldType(QString string)
{
  if(string == "fieldWithBuilding")
    return Koushin::fieldWithBuilding;
  if(string == "fieldWithForest")
    return Koushin::fieldWithForest;
  if(string == "fieldWithRocks")
    return Koushin::fieldWithRocks;
  if(string == "fieldWithWater")
    return Koushin::fieldWithWater;
  if(string == "fieldNotUsable")
    return Koushin::fieldNotUsable;
  return Koushin::plainField;
}

const Koushin::actionObjectType Koushin::Field::getActionObjectType()
{
  return Koushin::actionObjectIsField;
}

const QString Koushin::Field::getLocal(QString name, QString additionalContent)
{
  if(m_globalReplacements.contains(name))
    return m_globalReplacements.value(name);
  if(m_globalAdditions.contains(name))
    additionalContent += m_globalAdditions.value(name);
  return m_town->getLocal(name, additionalContent);
}

const QMap< QString, Koushin::ActionProperties > Koushin::Field::getPossibleActions()
{
  QMap<QString, Koushin::ActionProperties> actions;
  actions.insert("gatherResource", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Reduces the resource of this field and adds the amount to the town. string=ResourceType, int=value"));
  actions.insert("growResource", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Increases a resource for a field. string=ResourceType, ind=value"
  ));
  foreach(QString name, Koushin::ActionObject::getPossibleActions().keys())
    actions.insert(name, Koushin::ActionObject::getPossibleActions().value(name));
  return actions;

}

bool Koushin::Field::gatherResource(Koushin::ResourceType type, int value)
{
  int currentResource = getResource(type);
  Koushin::Resource* resource = m_town->getResources().value(type);
  int spaceInTown = resource->maximumCapacity - resource->amount;
  kDebug() << "gather Resource. Space in Town: " << spaceInTown << " and remaining Resource: " << currentResource;
  if(currentResource == 0 || spaceInTown == 0) return false;
  value = qMin(value, currentResource);
  value = qMin(value, spaceInTown);
  addToResource(type, -value);
  m_town->changeResource(type, value);
  kDebug() << "gatherResource: " << value;
  return true;  
}

bool Koushin::Field::growResource(Koushin::ResourceType type, int value)
{
  addToResource(type, value);
  return true;
}
