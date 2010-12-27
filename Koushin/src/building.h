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

#include <QGraphicsItem>

#include "town.h"
#include "actionobject.h"

class KConfigGroup;
namespace Koushin {
  class Building : public QGraphicsItem, public ActionObject {
    public:
      Building(Town* town);
      virtual ~Building();
      
      void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
      QRectF boundingRect() const;
      void setName(QString name) {m_name = name;}
      QString getName() const {return m_name;}
      Town* getTown() {return m_town;}
      int getLevel() const {return m_level;}
      int increaseLevel() {return ++m_level;}
      void setLevel(int newLevel) {m_level = newLevel;}
      int getNumberOfCreatedOpenFieldActions(QString name) const {return m_createdOpenFieldActions.value(name, 0);}
      void addOpenFieldAction(KConfigGroup* config);
      QList<KConfigGroup* > getOpenFieldActions() const {return m_openFieldActions;}

      const actionObjectType getActionObjectType() {return actionObjectIsBuiling;};
      static const QMap<QString, ActionProperties> getPossibleActions();
      const QString getLocal(QString name, QString additionalContent = QString());
      
    private:
      Town* m_town;
      QString m_name;
      QGraphicsTextItem* m_textItem;
      int m_level;
      QMap<QString, int > m_createdOpenFieldActions;
      QList<KConfigGroup* > m_openFieldActions;
  };
}

#endif // BUILDING_H
