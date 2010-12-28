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
    Foundation, Inc., 51 
    Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "building.h"
#include "player.h"
#include <kdebug.h>
#include "town.h"
#include "GUI/townwidget.h"
#include "GUI/buildinginfowidget.h"
#include "GUI/constructionmenu.h"
#include <QGraphicsScene>
#include <KConfigGroup>
#include <KConfig>
#include "actionparser.h"
#include "actionmanager.h"
#include "GUI/resourceinfowidget.h"
#include "action.h"
#include "game.h"
#include "field.h"

Koushin::Player::Player(QString name, Koushin::Game* game)
  : m_actionManager(new Koushin::ActionManager(this))
  , m_buildingLot(QPoint(0,0))
  , m_constructionMenu(0)
  , m_name(name)
  , m_game(game)
  , m_buildingInfo(0)
  , m_selectedBuilding(0)
  , m_openFieldConfig(0)
  , m_lastInteraction(Koushin::PlayerInteraction::noInteraction)
{

}

Koushin::Player::~Player()
{
  delete m_actionManager;
}


const QMap< QString, Koushin::ActionProperties > Koushin::Player::getPossibleActions()
{
  QMap<QString, Koushin::ActionProperties> actions;
  actions.insert("setGlobalTo", Koushin::ActionProperties(
    QStringList() << "string" << "string",
    "Player: overwrites a value. string=NameOfGlobal, string=NewValue "));
  actions.insert("addToGlobal", Koushin::ActionProperties(
    QStringList() << "string" << "string",
    "Player: add a string to a global. string=NameOfGlobal, string=AdditionalContent"));
  foreach(QString name, Koushin::ActionObject::getPossibleActions().keys())
    actions.insert(name, Koushin::ActionObject::getPossibleActions().value(name));
  return actions;
}

const QString Koushin::Player::getLocal(QString name, QString additionalContent)
{
  if(m_globalReplacements.contains(name))
    return m_globalReplacements.value(name);
  if(m_globalAdditions.contains(name))
    additionalContent += m_globalAdditions.value(name);
  return m_actionManager->getGlobalParameterValue(name) + additionalContent;
}

void Koushin::Player::townClicked(QPoint point) //create member with active town
{
  if(!m_townList.isEmpty() && !m_constructionMenu) {
    m_buildingLot = point;
    m_constructionMenu = new KoushinGUI::ConstructionMenu(m_townList.first()->getPossibleBuildings(m_listOfAllBuildings));
    QRect sceneRect = m_townList.first()->getTownWidget()->scene()->sceneRect().toRect();
    m_constructionMenu->setGeometry(sceneRect.width()/4, 0, sceneRect.width()/2, sceneRect.height()*3/4);
    m_townList.first()->getTownWidget()->scene()->addWidget(m_constructionMenu);
    connect(m_constructionMenu, SIGNAL(buildingChosen(QString)), this, SLOT(buildingChosen(QString)));
  }
}

void Koushin::Player::buildingChosen(QString buildingConfig)
{
  if(!m_actionManager) {
    kDebug() << "No ActionManger set. Can't create Building without a manager.";
    return; 
  }
  Koushin::Building* newBuilding = new Koushin::Building(m_townList.first());
  m_townList.first()->addBuilding(newBuilding, m_buildingLot);
  KConfig* config = new KConfig(buildingConfig);
  newBuilding->setName(KConfigGroup(config , "general").readEntry("name", QString("NoName")));

  KConfigGroup* tasksGroup = new KConfigGroup(config, "tasks");
  QList<Action* > actions = ActionParser::createActionsFromConfig(tasksGroup, newBuilding, m_game->getCurrentRound());
  m_actionManager->addAction(actions);
  Koushin::ActionParser::parseGlobals(tasksGroup->group("globals"), m_actionManager);
  
  ActionParser::createOpenFieldActions(new KConfigGroup(config, "fieldTasks"), newBuilding);
  

//   m_townList.first()->getTownWidget()->drawBuildings(m_townList.first()->getBuildings());
  newBuilding->getField()->getFieldItem()->update(newBuilding->getField()->getFieldItem()->boundingRect());
//   m_townList.first()->getTownWidget()->scene()->removeWidget(m_constructionMenu);
//   delete(m_constructionMenu);
  //TODO: find a way to delete the widget
  if(m_constructionMenu) {
    m_constructionMenu->setVisible(false);
    m_constructionMenu = 0;
  }
}

void Koushin::Player::endRound()
{
  m_game->endRound();
}

void Koushin::Player::startRound()
{
  m_lastInteraction = Koushin::PlayerInteraction::roundedStarted;
}


void Koushin::Player::setBuildingInfoWidget(KoushinGUI::BuildingInfoWidget* widget)
{
  m_buildingInfo = widget;
  connect(m_buildingInfo, SIGNAL(fieldActionSelected(QListWidgetItem*)), this, SLOT(fieldActionSelected(QListWidgetItem*)));
}

void Koushin::Player::fieldActionSelected(QListWidgetItem* item)
{
  if(!m_selectedBuilding) return;
  foreach(KConfigGroup* group, m_selectedBuilding->getOpenFieldActions())
    if(group->name() == item->text()) {
      m_openFieldConfig = group;
      break;
    }
  qreal radius = m_openFieldConfig->readEntry("fieldRadius", qreal(1));
  QString typeLine = m_openFieldConfig->readEntry("needs", QString());
  typeLine = ActionParser::separateNameAndParameters(typeLine).second.first();
  Koushin::FieldType type = Koushin::Field::QStringToFieldType(typeLine);
  m_fieldsForFieldAction = m_townList.first()->getPossibleFields(m_selectedBuilding->pos().toPoint(), radius, type);
  m_townList.first()->markFields(m_fieldsForFieldAction);
}

void Koushin::Player::setSelectedBuilding(Koushin::Building* building)
{
  m_selectedBuilding = building;
  m_selectedBuilding->getField()->markField(Qt::green);
  m_buildingInfo->setBuilding(m_selectedBuilding);
}

void Koushin::Player::fieldForActionChoosen(Koushin::Field* field)
{
  if(!m_selectedBuilding) return;
  QList<Action* > actions = ActionParser::createActionsFromConfig(m_openFieldConfig, field, m_game->getCurrentRound(), true);
  m_actionManager->addAction(actions);
  m_fieldsForFieldAction.clear();
  m_selectedBuilding->removeOpenFieldAction(m_openFieldConfig);
  m_buildingInfo->repaint();
}

void Koushin::Player::fieldClicked(Koushin::Field* field)
{
  kDebug() << "field clicked: " << field->getFieldItem()->pos();
  switch (m_lastInteraction) {
    case Koushin::PlayerInteraction::roundedStarted: case Koushin::PlayerInteraction::noInteraction:
      if(field->getType() == Koushin::plainField) {
	townClicked(field->getFieldItem()->pos().toPoint());
      }
      if(field->getType() == Koushin::fieldWithBuilding && field->getBuilding()) {
	setSelectedBuilding(field->getBuilding());
	m_buildingInfo->repaint();
	m_lastInteraction = Koushin::PlayerInteraction::buildingClicked;
      }
      break;
    case Koushin::PlayerInteraction::buildingClicked:
      if(m_fieldsForFieldAction.contains(field)) {
	fieldForActionChoosen(field);
	field->getTown()->unmarkAllFields();
	m_buildingInfo->repaint();
      } else {
	field->getTown()->unmarkAllFields();
	m_lastInteraction = Koushin::PlayerInteraction::noInteraction;
      }
      break;
    default:
      kDebug() << "Do not know what to do.";
  }
}


#include "player.moc"
