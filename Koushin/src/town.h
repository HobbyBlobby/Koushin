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


#ifndef TOWN_H
#define TOWN_H

#include <QMap>
#include <QPoint>
#include "actionobject.h"

class KConfig;
namespace KoushinGUI {
  class TownWidget;
}

namespace Koushin {

  enum ResourceType {
    ResourceUnspezifed = 0,
    ResourceWood,
    ResourceStone,
    ResourceRice,
    ResourceTypeCount
  };
  
  enum FieldType {
    plainField = 0,
    fieldWithForest,
    fieldWithRocks,
    fieldWithWater,
    fieldWithBuilding,
    numberOfFieldTypes
  };

  class Resource {
    public:
      Resource(ResourceType type = ResourceUnspezifed)
	: maximumCapacity(0)
	, amount(0)
	, type(type) {}
      void setAmount(int val) {amount = val;}
      int maximumCapacity;
      int amount;
      ResourceType type;
  };
  
  class Building;
  class Field;
  class Player;
  class Town : public ActionObject {
    public:
      Town(Player* owner, KConfig* config = 0);
      virtual ~Town();
      Player* getOwner() {return m_owner;}

// Function to access also with parser      
      bool changeResource(ResourceType type, int difference);
      bool setResourceCapacity(ResourceType type, int value);
//other functions
      QMap<ResourceType, Resource*> getResources() {return m_resources;}
      ::KoushinGUI::TownWidget* getTownWidget() const {return m_townWidget;}
      bool addBuilding(Building* building, QPoint pos);
      QMap<Building*, QPoint> getBuildings() const {return m_buildings;}
      QMap<QString, QString> getPossibleBuildings(QMap<QString, QString> allBuildings) const;
      QList<Field* > getPossibleFields(QPoint aroundPos, qreal radius, FieldType type);
      void markFields(QList<Field* > fields);
      void unmarkAllFields();
      Field* getFieldFromBuilding(Building* building);
//       void setTownConfig(KConfig* config) {m_townConfig = config;}
      KConfig* getTownConfig() const {return m_townConfig;}
      
      static ResourceType getResourceTypeFromQString(QString resourceName);
      
      const actionObjectType getActionObjectType() {return actionObjectIsTown;};
      static const QMap<QString, ActionProperties> getPossibleActions();
      const QString getLocal(QString name, QString additionalContent = QString());

    private:
      Player* m_owner;
      QMap<ResourceType, Resource*> m_resources;
      QMap<Building*, QPoint> m_buildings;
      QMap<QPoint, Field* > m_fields;
      ::KoushinGUI::TownWidget* m_townWidget;
      KConfig* m_townConfig;
  };
}

#endif // TOWN_H
