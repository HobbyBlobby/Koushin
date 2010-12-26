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


#ifndef FIELD_H
#define FIELD_H

#include "actionobject.h"
#include <QString>
#include "town.h"

class QGraphicsWidget;
class QGraphicsItem;
namespace Koushin {

class Building;


  enum FieldType {
    plainField = 0,
    fieldWithForest,
    fieldWithRocks,
    fieldWithWater,
    fieldWithBuilding,
    numberOfFieldTypes
  };
  
  class Field : public ActionObject {
    public:
      Field(Town* town, FieldType type = plainField);
      drawField(QGraphicsItem* parent);
      
//virtual functions from ActionObject:
      virtual const actionObjectType getActionObjectType();
      virtual const QString getLocal(QString name, QString additionalContent = QString());
      
//getters and setters:
      void setType(ResourceType type) {m_type = type;}
      void setResource(ResourceType type, int newValue) {m_resources.insert(type, newValue);}
      void addToResource(ResourceType type, int value) {value += getResource(type); m_resources.insert(type, value);}
      int getResource(ResourceType type) {m_resources.value(type, 0);}
      void addBuilding(Building* building) {m_building = building;}
      Building* getBuilding() const {return m_building;}
      
      static QString fieldTypeToQString(FieldType type);
      static FieldType QStringToFieldType(QString string);
      
//actions for parser:
      bool gatherResource(type, value);
      bool growResource(type, value);
    private:
      ResourceType m_type;
      Town* m_town;
      QMap<ResourceType, int > m_resources;
      Building* m_building;
  };
}

#endif // FIELD_H
