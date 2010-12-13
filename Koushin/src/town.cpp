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

Koushin::Town::Town(Player* owner)
  : m_owner(owner)
  , m_townWidget(new KoushinGUI::TownWidget)
{
  //Create Resources with town
  for (int i = 1; i < (int)Koushin::ResourceTypeCount; ++i) {
    Koushin::ResourceType type = (Koushin::ResourceType)i;
    m_resources.insert(type, Koushin::Resource(type));
  }
  m_owner->addTown(this);
}

Koushin::Town::~Town()
{

}

#include <KDebug>
bool Koushin::Town::changeResource(Koushin::ResourceType type, int difference)
{
  Koushin::Resource res = m_resources.value(type);
  if (res.type == Koushin::ResourceUnspezifed) {
    kDebug() << "Resource not found.";
    return 0;
  }
  if (res.amount + difference > res.maximumCapacity) {
    kDebug() << "Capacity reached" << res.amount + difference << " of " << res.maximumCapacity;
    res.amount = res.maximumCapacity;
    return 0;
  }
  if (res.amount + difference < 0) {
    kDebug() << "Not enough resources: " << res.amount - difference;
    return 0;
  }
  res.amount += difference;
  return 1;
}

void Koushin::Town::setResourceCapacity(Koushin::ResourceType type, int value)
{
  Koushin::Resource res = m_resources.value(type);
  res.maximumCapacity = value;
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
  return 1;
}

QMap< QString, QString > Koushin::Town::getPossibleBuildings(QMap< QString, QString > allBuildings) const
{
  //Look for prerequisits, now: no prerequisits are implemented, so all buildings will returned
  return allBuildings;
}
