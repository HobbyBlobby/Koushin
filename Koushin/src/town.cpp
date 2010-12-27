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

#include "town.h"
#include "GUI/townwidget.h"
#include "player.h"

#include <KDebug>
#include "building.h"
#include <math.h>
#include "field.h"

//define operator "<" for QPoint after lexigographic order for using it as key in QMap:
bool operator<(const QPoint& lPoint, const QPoint& rPoint) {
  if(lPoint.x() < rPoint.x())
    return true;
  if(lPoint.x() == rPoint.x() && lPoint.y() < rPoint.y())
    return true;
  return false;
}

qreal distance(QPoint p1, QPoint p2) {
  QPoint tmpP = p2 - p1;
  return sqrt(pow(tmpP.x(), 2) + pow(tmpP.y(), 2));
}

Koushin::Town::Town(Player* owner)
  : m_owner(owner)
  , m_townWidget(new KoushinGUI::TownWidget)
{
  //Create Resources with town
  for (int i = 1; i < (int)Koushin::ResourceTypeCount; ++i) {
    Koushin::ResourceType type = (Koushin::ResourceType)i;
    m_resources.insert(type, new Koushin::Resource(type));
  }
  m_owner->addTown(this);
  for (int i = 0; i < 10; ++i) {
    for(int j = 0; j < 10; ++j) {
      Koushin::Field* field =  new Koushin::Field(this);
      field->setPos(QPoint(i, j));
      Koushin::FieldType type;
      while((type = (Koushin::FieldType)(qrand() % Koushin::numberOfFieldTypes)) == Koushin::fieldWithBuilding) {}
	field->setType(type);
      m_fields.insert(QPoint(i,j), field);
    }
  }
}

Koushin::Town::~Town()
{

}

const QMap< QString, Koushin::ActionProperties > Koushin::Town::getPossibleActions()
{
  QMap<QString, Koushin::ActionProperties> actions;
  actions.insert("increaseResource", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Town: Add the given parameter to the given resource. string=ResourceName, int=difference"));
  actions.insert("decreaseResource", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Town: Removes the given parameter from the given resource. string=ResourceName, int=difference"));
  actions.insert("setResourceCapacity", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Town: Sets the capacity of a given resource to the given value. string=ResourceName, int=new value"));
  foreach(QString name, Koushin::ActionObject::getPossibleActions().keys())
    actions.insert(name, Koushin::ActionObject::getPossibleActions().value(name));
  return actions;
}

const QString Koushin::Town::getLocal(QString name, QString additionalContent)
{
  if(m_globalReplacements.contains(name))
    return m_globalReplacements.value(name);
  if(m_globalAdditions.contains(name))
    additionalContent += m_globalAdditions.value(name);
  return m_owner->getLocal(name, additionalContent);
}


bool Koushin::Town::changeResource(Koushin::ResourceType type, int difference)
{
  Koushin::Resource* res = m_resources.value(type);
  if (res->type == Koushin::ResourceUnspezifed) {
    kDebug() << "Resource not found.";
    return false;
  }
  if (res->amount + difference > res->maximumCapacity) {
    kDebug() << "Capacity reached" << res->amount + difference << " of " << res->maximumCapacity;
    res->amount = res->maximumCapacity;
    return false;
  }
  if (res->amount + difference < 0) {
    kDebug() << "Not enough resources: " << qAbs(difference) << " of " << res->amount - difference;
    return false;
  }
  res->amount += difference;
  return true;
}

bool Koushin::Town::setResourceCapacity(Koushin::ResourceType type, int value)
{
  Koushin::Resource* res = m_resources.value(type);
  res->maximumCapacity = value;
  return true; //nothing can go wrong, or?
}

Koushin::ResourceType Koushin::Town::getResourceTypeFromQString(QString resourceName)
{
  if(resourceName == "Wood") return Koushin::ResourceWood;
  if(resourceName == "Stone") return Koushin::ResourceStone;
  if(resourceName == "Rice") return Koushin::ResourceRice;
  return Koushin::ResourceUnspezifed;
}

bool Koushin::Town::addBuilding(Koushin::Building* building, QPoint pos)
{
  if(m_buildings.contains(building)){
    kDebug() << "Building is allready on the map at" << m_buildings.value(building).x() << ", " << m_buildings.value(building).y();
    return 0;
  }
  if(m_buildings.key(pos, 0) != 0) {
    kDebug() << "There is allready a building at this position" << pos.x() << ", " << pos.y();
    return 0;
  }
  m_buildings.insert(building, pos);
  Koushin::Field* field = m_fields.value(pos, 0);
  if(field) {
    field->setType(Koushin::fieldWithBuilding);
    field->addBuilding(building);
  }
  return 1;
}

QMap< QString, QString > Koushin::Town::getPossibleBuildings(QMap< QString, QString > allBuildings) const
{
  //Look for prerequisits, now: no prerequisits are implemented, so all buildings will returned
  return allBuildings;
}

void Koushin::Town::markFields(QPoint aroundPos, qreal radius, Koushin::FieldType type)
{
  foreach(QPoint point, m_fields.keys()) {
    if(distance(point, aroundPos) <= radius) {
      Koushin::Field* field = m_fields.value(point);
      if(field->getType() == type)
	field->markField();
    }
  }
}

void Koushin::Town::unmarkAllFields()
{
  foreach(Koushin::Field* field, m_fields.values())
    field->unmarkField();
}
