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

Koushin::Town::Town(Player* owner)
  : m_owner(owner)
{
  //Create Resources with town
  for (int i = 1; i < (int)Koushin::ResourceTypeCount; ++i) {
    Koushin::ResourceType type = (Koushin::ResourceType)i;
    m_resources.insert(type, Koushin::Resource(type));
  }
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
    return 0;
  }
  if (res.amount + difference < 0) {
    kDebug() << "Not enough resources: " << res.amount - difference;
    return 0;
  }
  res.amount += difference;
  return 1;
}

Koushin::ResourceType Koushin::Town::getResourceTypeFromQString(QString resourceName)
{
  if(resourceName == "Wood") return Koushin::ResourceWood;
  if(resourceName == "Stone") return Koushin::ResourceStone;
  if(resourceName == "Rice") return Koushin::ResourceRice;
  return Koushin::ResourceUnspezifed;
}

