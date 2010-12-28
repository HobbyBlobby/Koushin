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


#ifndef PLAYER_H
#define PLAYER_H

#include <QList>
#include <QObject>
#include <QPoint>
#include <QMap>

#include "actionobject.h"
#include "GUI/resourceinfowidget.h"
#include <KConfigGroup>
class QListWidgetItem;

namespace KoushinGUI {
  class ConstructionMenu;
  class BuildingInfoWidget;
}

namespace Koushin {
  class ActionManager;
  class Town;
  class Game;
  
  namespace PlayerInteraction {
    enum Interaction {
      noInteraction = 0,
      plainFieldClicked,
      buildingClicked,
      freeFieldActionChoosen,
      roundedEnded,
      roundedStarted
    };
  }
  
  class Player : public QObject, public ActionObject {
    Q_OBJECT
    public:
      Player(QString name, Game* game);
      virtual ~Player();
      QList<Town* > getTowns() const {return m_townList;}
      ActionManager* getActionManager() const {return m_actionManager;}
      void setActionManager(ActionManager* manager) {m_actionManager = manager;}
      void addTown(Town* town) {m_townList << town;}
      void setBuildingList(QMap<QString, QString> buildings) {m_listOfAllBuildings = buildings;}
      void setResourceInfoWidget(KoushinGUI::ResourceInfoWidget* widget) {m_resourceInfo = widget;}
      KoushinGUI::ResourceInfoWidget* getResourceInfoWidget() const {return m_resourceInfo;}
      QString getName() const {return m_name;}
      ::KoushinGUI::BuildingInfoWidget* getBuidlingInfoWidget() const {return m_buildingInfo;}
      void setBuildingInfoWidget(::KoushinGUI::BuildingInfoWidget* widget);
      void setSelectedBuilding(Building* building);
      
      const actionObjectType getActionObjectType() {return actionObjectIsPlayer;}
      static const QMap<QString, ActionProperties> getPossibleActions();
      const QString getLocal(QString name, QString additionalContent = QString());
      

    public Q_SLOTS:
      void endRound();
      void startRound();
      void fieldClicked(Field* field);
      void buildingChosen(QString buildingConfig);
      void fieldActionSelected(QListWidgetItem* item);
    private:
      void townClicked(QPoint point);
      void fieldForActionChoosen(Field* field);

      QList<Town* > m_townList;
      ActionManager* m_actionManager;
      QMap<QString, QString> m_listOfAllBuildings;
      QPoint m_buildingLot;
      KoushinGUI::ConstructionMenu* m_constructionMenu;
      KoushinGUI::ResourceInfoWidget* m_resourceInfo;
      KoushinGUI::BuildingInfoWidget* m_buildingInfo;
      QString m_name;
      Game* m_game;
      Building* m_selectedBuilding;
      KConfigGroup* m_openFieldConfig;
      PlayerInteraction::Interaction m_lastInteraction;
      QList<Field* > m_fieldsForFieldAction;
  };
}

#endif // PLAYER_H
