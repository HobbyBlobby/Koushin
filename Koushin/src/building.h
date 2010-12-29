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


#ifndef BUILDING_H
#define BUILDING_H

#include "town.h"
#include "actionobject.h"
#include "field.h"
#include "GUI/fielditem.h"

class KConfig;
class KConfigGroup;
namespace Koushin {

class Field;

  class Building : public ActionObject {
    public:
      Building(Town* town, KConfig* config);
      virtual ~Building();
      
      void setName(QString name) {m_name = name;}
      QString getName() const {return m_name;}
      Town* getTown() {return m_town;}
      int getLevel() const {return m_level;}
      int increaseLevel() {return ++m_level;}
      void setLevel(int newLevel) {m_level = newLevel;}
      int getNumberOfCreatedOpenFieldActions(QString name) const {return m_createdOpenFieldActions.value(name, 0);}
      void addOpenFieldAction(QString groupName);
      void removeOpenFieldAction(QString groupName);
      QStringList getOpenFieldActions() const {return m_openFieldActions;}
      void setField(Field* field) {m_field = field;}
      Field* getField() const {return m_field;}
      QPointF pos() const {return m_field->getFieldItem()->pos();}
      void useField(Field* field) {m_usedFields << field;}
      QList<Field* > getUsedFields() const {return m_usedFields;}
      KConfig* getConfig() const {return m_config;}
      void select();
      void unselect();

      const actionObjectType getActionObjectType() {return actionObjectIsBuiling;};
      static const QMap<QString, ActionProperties> getPossibleActions();
      const QString getLocal(QString name, QString additionalContent = QString());
      
    private:
      Town* m_town;
      QString m_name;
      int m_level;
      QMap<QString, int > m_createdOpenFieldActions;
      QStringList m_openFieldActions;
      QList<Field* > m_usedFields;
      Field* m_field;
      KConfig* m_config;
  };
}

#endif // BUILDING_H
