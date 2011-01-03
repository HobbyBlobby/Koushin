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
#include <kconfig.h>
#include "actionparser.h"
#include <QMetaClassInfo>

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

Koushin::Town::Town(Player* owner, KConfig* config)
  : m_owner(owner)
  , m_townWidget(new KoushinGUI::TownWidget)
  , m_townConfig(config)
{
  if(owner) {
    //Create Resources with town
    for (int i = 1; i < (int)Koushin::ResourceTypeCount; ++i) {
      Koushin::ResourceType type = (Koushin::ResourceType)i;
      m_resources.insert(type, new Koushin::Resource(type));
    }
    m_owner->addTown(this);
    
    if(m_townConfig) {
      m_townWidget->updateTownPixmap(m_townConfig);
      KConfigGroup fieldGroup = m_townConfig->group("fields");
      foreach(QString groupName, fieldGroup.groupList()) {
	KConfigGroup group = fieldGroup.group(groupName);
	foreach(QString key, group.keyList()) {
	  QPoint point = group.readEntry(key, QPoint(0,0));
	  Koushin::Field* field = new Koushin::Field(this);
	  field->setType(Koushin::Field::QStringToFieldType(groupName));
	  field->setPos(point);
	  m_fields.insert(point, field);
	  if(field->getType() == Koushin::fieldWithForest)
	    field->setResource(Koushin::ResourceWood, 1000);
	  if(field->getType() == Koushin::fieldWithRocks)
	    field->setResource(Koushin::ResourceStone, 1000);
	}// loop over fields
      }// loop over field types
    }// if(m_townConfig)
  } // if(owner)
}

Koushin::Town::~Town()
{

}

const QMap< QString, Koushin::ActionProperties > Koushin::Town::getPossibleActions()
{
  QMap<QString, Koushin::ActionProperties> actions;
  for(int i = 0; i < metaObject()->methodCount(); ++i) {
    QPair<QString, QStringList> function = Koushin::ActionParser::separateNameAndParameters(metaObject()->method(i).signature());
    Koushin::ActionProperties prop(function.second, QString());
    if(function.first == "increaseResource") {
      prop.activate("Town: Add the given parameter to the given resource. string=ResourceName, int=difference");
    }
    else if(function.first == "decreaseResource") {
      prop.activate("Town: Removes the given parameter from the given resource. string=ResourceName, int=difference");
    }
    else if(function.first == "setResourceCapacity") {
      prop.activate("Town: Sets the capacity of a given resource to the given value. string=ResourceName, int=new value");
    }
    actions.insert(function.first, prop);
  }
  adjustActionProperties(actions);
  kDebug() << actions.keys();
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
    kDebug() << "Not enough resources: " << res->amount << " of " << qAbs(difference);
    return false;
  }
  res->amount += difference;
  return true;
}

bool Koushin::Town::setResourceCapacity(Koushin::ResourceType type, int value)
{
  Koushin::Resource* res = m_resources.value(type);
  kDebug() << (int)type << " = " << value;
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
    building->setField(field);
  }
  return 1;
}

QMap< QString, QString > Koushin::Town::getPossibleBuildings(QMap< QString, QString > allBuildings) const
{
  //Look for prerequisits, now: no prerequisits are implemented, so all buildings will returned
  return allBuildings;
}

void Koushin::Town::markFields(QList< Koushin::Field* > fields)
{
  foreach(Koushin::Field* field, fields)
    field->markField();
}

QList< Koushin::Field* > Koushin::Town::getPossibleFields(QPoint aroundPos, qreal radius, Koushin::FieldType type)
{
  QList<Koushin::Field* > list;
  foreach(QPoint point, m_fields.keys()) {
    if(distance(point, aroundPos) <= radius) {
      Koushin::Field* field = m_fields.value(point);
      if(field->getType() == type)
	list << field;
    }
  }
  return list;
}


void Koushin::Town::unmarkAllFields()
{
  foreach(Koushin::Field* field, m_fields.values())
    field->unmarkField();
}

Koushin::Field* Koushin::Town::getFieldFromBuilding(Koushin::Building* building)
{
  return m_fields.value(m_buildings.value(building));
}

Koushin::Field* Koushin::Town::getFieldFromPoint(QPoint point)
{
  return m_fields.value(point);
}

QMap< QString, QString > Koushin::Town::getBuildingList()
{
  if(m_owner)
    return m_owner->getBuildingList();
  return QMap<QString, QString>(); //avoid crash, but this case is actual not possible
}
