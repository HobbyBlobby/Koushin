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
#include <QMetaClassInfo>
#include <QGraphicsView>

Koushin::Player::Player(QString name, Koushin::Game* game)
  : m_actionManager(new Koushin::ActionManager(this))
  , m_buildingLot(QPoint(0,0))
  , m_name(name)
  , m_game(game)
  , m_buildingInfo(0)
  , m_selectedBuilding(0)
  , m_openFieldConfig("")
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
  for(int i = 0; i < metaObject()->methodCount(); ++i) {
    QPair<QString, QStringList> function = Koushin::ActionParser::separateNameAndParameters(metaObject()->method(i).signature());
    Koushin::ActionProperties prop(function.second, QString());
    if(function.first == "setGlobalTo") {
      prop.activate("Player: overwrites a value. string=NameOfGlobal, string=NewValue ");
    }
    else if(function.first == "addToGlobal") {
      prop.activate("Player: add a string to a global. string=NameOfGlobal, string=AdditionalContent");
    }
    actions.insert(function.first, prop);
  }
  adjustActionProperties(actions);
  kDebug() << actions.keys();
  return actions;
}

bool Koushin::Player::addToGlobal(QString name, QString content)
{
  if(!m_actionManager) {
    kDebug() << "no action manager given";
    return false;
  }
  kDebug() << "call addToGlobal";
  return m_actionManager->setGlobalParameterContent(name, content);
}

bool Koushin::Player::setGlobalTo(QString name, QString content)
{
  return m_actionManager->addContentToGlobalParameter(name, content);
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
  if(!m_townList.isEmpty()) {
    m_buildingLot = point;
    //find better way to map the point
//     QPoint globalPoint = m_townList.first()->getTownWidget()->scene()->views().first()->mapFromScene(point);
//     globalPoint = m_townList.first()->getTownWidget()->scene()->views().first()->parentWidget()->mapToGlobal(globalPoint);
    emit showConstructionMenu(m_townList.first(), point);
  }
}

void Koushin::Player::buildingChosen(QString buildingConfig)
{
  if(!m_actionManager) {
    kDebug() << "No ActionManger set. Can't create Building without a manager.";
    return; 
  }
  KConfig* config = new KConfig(buildingConfig);
  Koushin::Building* newBuilding = new Koushin::Building(m_townList.first(), config);
  m_townList.first()->addBuilding(newBuilding, m_buildingLot);
  newBuilding->setName(KConfigGroup(config , "general").readEntry("name", QString("NoName")));

  KConfigGroup* tasksGroup = new KConfigGroup(config, "tasks");
  QList<Action* > actions = ActionParser::createActionsFromConfig(tasksGroup, newBuilding, m_game->getCurrentRound());
  m_actionManager->addAction(actions);
  Koushin::ActionParser::parseGlobals(tasksGroup->group("globals"), m_actionManager);
  
  ActionParser::createOpenFieldActions(new KConfigGroup(config, "fieldTasks"), newBuilding);
  
  newBuilding->getField()->getFieldItem()->update(newBuilding->getField()->getFieldItem()->boundingRect());
//   if(m_constructionMenu) {
  emit closeConstructionMenu();
//   }
}

void Koushin::Player::endRound()
{
  if(m_selectedBuilding)
    m_selectedBuilding->unselect();
  setSelectedBuilding(0);
  m_fieldsForFieldAction.clear();
  m_game->endRound();
}

void Koushin::Player::startRound()
{
  emit showResourceInfo(m_townList.first());
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
  m_openFieldConfig = item->text();

  KConfigGroup group = m_selectedBuilding->getConfig()->group("fieldTasks").group(m_openFieldConfig);
  qreal radius = group.readEntry("fieldRadius", qreal(1));
  QString typeLine = group.readEntry("needs", QString()); ///@todo prevent crash when "needs=" is empty
  kDebug() << "needs: " << typeLine;
  typeLine = ActionParser::separateNameAndParameters(typeLine).second.first();
  Koushin::FieldType type = Koushin::Field::QStringToFieldType(typeLine);
  m_fieldsForFieldAction = m_townList.first()->getPossibleFields(m_selectedBuilding->pos().toPoint(), radius, type);
  foreach(Koushin::Field* field, m_selectedBuilding->getUsedFields())
    m_fieldsForFieldAction.removeOne(field);
  m_townList.first()->markFields(m_fieldsForFieldAction);
}

void Koushin::Player::setSelectedBuilding(Koushin::Building* building)
{
  m_selectedBuilding = building;
  if(building) {
    m_buildingInfo->setBuilding(m_selectedBuilding);
  }
}

void Koushin::Player::fieldForActionChoosen(Koushin::Field* field)
{
  if(!m_selectedBuilding) return;
  KConfigGroup* fieldsGroup = new KConfigGroup(m_selectedBuilding->getConfig()->group("fieldTasks"));
//create actions
  QList<Action* > actions = ActionParser::createActionsFromConfig(fieldsGroup, field, m_game->getCurrentRound(), m_openFieldConfig);
  m_actionManager->addAction(actions);

  m_fieldsForFieldAction.clear();
  m_selectedBuilding->useField(field);
  m_selectedBuilding->removeOpenFieldAction(m_openFieldConfig);
  m_buildingInfo->repaint();
}

void Koushin::Player::fieldClicked(Koushin::Field* field)
{
  switch (m_lastInteraction) {
    case Koushin::PlayerInteraction::roundedStarted: case Koushin::PlayerInteraction::noInteraction:
      if(field->getType() == Koushin::plainField) {
	townClicked(field->getFieldItem()->pos().toPoint());
      }
      if(field->getType() == Koushin::fieldWithBuilding && field->getBuilding()) {
	setSelectedBuilding(field->getBuilding());
	field->getBuilding()->select();
	m_buildingInfo->repaint();
	m_lastInteraction = Koushin::PlayerInteraction::buildingClicked;
      }
      break;
    case Koushin::PlayerInteraction::buildingClicked:
      if(m_fieldsForFieldAction.contains(field)) {
	fieldForActionChoosen(field);
	field->getTown()->unmarkAllFields();
	m_selectedBuilding->select();
	m_buildingInfo->repaint();
      } else {
	setSelectedBuilding(0);
	field->getTown()->unmarkAllFields();
	m_buildingInfo->repaint();
	m_lastInteraction = Koushin::PlayerInteraction::noInteraction;
      }
      break;
    default:
      kDebug() << "Do not know what to do.";
  }
}

void Koushin::Player::testPlayer()
{
  int index = metaObject()->indexOfSlot("testSlot(Koushin::FieldType)");
  Koushin::Field* field = m_townList.first()->getFieldFromPoint(QPoint(12,12));
  Koushin::FieldType type = field->getType();
  kDebug() << "Field used: " << Koushin::Field::fieldTypeToQString(field->getType());
  QGenericArgument arg = QGenericArgument("Koushin::FieldType", &type);
  metaObject()->method(index).invoke(this, Qt::DirectConnection, arg);
}

void Koushin::Player::testSlot(Koushin::FieldType ausgabe)
{
  kDebug() << "Ausgabe: " << Koushin::Field::fieldTypeToQString(ausgabe);
}


#include "player.moc"
