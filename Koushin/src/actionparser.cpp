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

#include "actionparser.h"
#include "actionmanager.h"
#include "building.h"
#include "field.h"
#include "player.h"
#include "town.h"

#include <QRegExp>
#include <QString>

#include <kdebug.h>
#include "action.h"
#include "field.h"

QPair< QString, QStringList > Koushin::ActionParser::separateNameAndParameters(QString string)
{
  QString actionName = string.split("(")[0];
  string.remove(0, string.indexOf("(")+1);   //remove all untill/inclusive the first "("
  string.remove(string.length()-1, 1); //remove last char, should be ")"
  QStringList parameters = string.split(",");
  if (string.isEmpty()) parameters = QStringList();
  return QPair<QString, QStringList>(actionName, parameters);
}

QList< Koushin::Action* > Koushin::ActionParser::createActionsFromConfig(KConfigGroup* tasksGroup, Koushin::ActionObject* newOwner, int currentRound, QString selectedGroup)
{
  QMap<QString, Koushin::Action* > actions;
  QStringList actionNames = tasksGroup->groupList();
//Create Actions:
  foreach(QString actionName, actionNames) {
    if(actionName == "globals" || actionName == "conditions") continue;
    if(selectedGroup.isEmpty() || selectedGroup == actionName) {
      KConfigGroup* actionGroup = new KConfigGroup(tasksGroup->group(actionName));
      Koushin::Action* newAction = new Koushin::Action();
      newAction->setOwner(newOwner);
      newAction->setConfiguration(actionGroup);
      setRoundLimit(newAction, actionGroup, currentRound);
      actions.insert(actionName, newAction);
    }
  }
//write conditions to action:
///no conditions for field tasks are possible, because you can create more then one field action from one config
  KConfigGroup conditionGroup = tasksGroup->group("conditions");
  QStringList conditionsList;
  foreach(QString condition, conditionGroup.keyList())
    conditionsList << conditionGroup.readEntryUntranslated(condition, QString());
  addRequirementsToActions(conditionsList, actions);
  
  return actions.values();
}

void Koushin::ActionParser::setRoundLimit(Koushin::Action* action, KConfigGroup* config, int currentRound)
{
  if(!action) return;
  int fromRound = config->readEntry("fromRound", int(-1));
  int toRound = config->readEntry("toRound", int(-1));
  int intervall = config->readEntry("intervall", int(-1));
  if(fromRound + toRound + intervall == -3)
    action->executeInEveryRound(true);
  else {
    if(fromRound <= 0)
      fromRound = currentRound + 1; //execute next round the first time
    else
      fromRound += currentRound;
    if(toRound < fromRound)
      toRound = fromRound;
    else
      toRound += currentRound;
    if(intervall <= 0)
      intervall = 1;
    QList<int > roundList;
    for(int i = fromRound; i <= toRound; i += intervall)
      roundList << i;
    action->executeInRounds(roundList);
    action->executeInEveryRound(false);
  }
}


void Koushin::ActionParser::addRequirementsToActions(QStringList conditionStrings, QMap< QString, Koushin::Action* > actions)
{
  foreach(QString condition, conditionStrings) {
    QStringList actionNames = condition.split("->");
    if(actionNames.size() != 2) {
      kDebug() << "Wrong condition string: " << condition;
      continue;
    }
    QString requirement = actionNames[0];
    requirement.remove(QRegExp("^!"));
    if(actions.value(requirement, 0) == 0) {
      kDebug() << "Requirement is not a valid action: " << actionNames[0];
    }
    if(actions.value(actionNames[1], 0) == 0) {
      kDebug() << "The conditional action is not valid: " << actionNames[1];
    }
    if (requirement == actionNames[0]) {//no "!" as first char, so no char is removed, the strings are equal
      actions.value(actionNames[1])->addRequirement(actions.value(requirement));
    } else {
      actions.value(actionNames[1])->addRequirement(actions.value(requirement), false);
    }
  }
}

bool Koushin::ActionParser::parseGlobals(const KConfigGroup& parameterList, Koushin::ActionManager* manager)
{
  kDebug() << parameterList.keyList();
  foreach(QString parameter, parameterList.keyList()) {
    kDebug() << "################## Add Global " << parameter << "=" << parameterList.readEntry(parameter, QString()) << "########";
    manager->addGlobalParameter(parameter, parameterList.readEntry(parameter, QString()));
  }
  return true;
}

QList<Koushin::ActionObject* > Koushin::ActionParser::parseRecipient(const QString& configLine, ActionObject* owner)
{
  QString expectedObject = Koushin::ActionParser::separateNameAndParameters(configLine.split("->").last()).first;
  
  QString linePart;
//find Players:
  QList<Koushin::Player* > players;
  if(!(linePart = configLine.section(QRegExp("player([A-Za-z0-9,=]*)"), 0)).isEmpty()) { //find e.g.: player(all,minPoints=1000)
    players = findPlayers(Koushin::ActionParser::separateNameAndParameters(linePart).second, owner);
  } else {
    players = findPlayers(QStringList(), owner);
  }
  if(players.isEmpty()) {
    kDebug() << "No reasonable player found" << linePart;
    return QList<Koushin::ActionObject* >();
  }
  if(expectedObject == "player") {
    QList<Koushin::ActionObject* > objects;
    foreach(Koushin::Player* player, players)
      objects << (Koushin::ActionObject*)player;
    return objects;
  }
//find Towns:
  QList<Koushin::Town* > towns;
  if(!(linePart = configLine.section(QRegExp("town([A-Za-z0-9,=]*)"), 0)).isEmpty()) { //find e.g.: town(all,hasBuilding=lumbermill)
    towns = findTowns(Koushin::ActionParser::separateNameAndParameters(linePart).second, owner, players);
  } else {
    towns = findTowns(QStringList(), owner, players);
  }
  if(towns.isEmpty()) {
    kDebug() << "No reasonable town found" << linePart;
    return QList<Koushin::ActionObject* >();
  }
  if(expectedObject == "town") {
    QList<Koushin::ActionObject* > objects;
    foreach(Koushin::Town* town, towns)
      objects << (Koushin::ActionObject*)town;
    return objects;
  }
//find Buildings:
  QList<Koushin::Building* > buildings;
  if(!(linePart = configLine.section(QRegExp("building([A-Za-z0-9,=]*)"), 0)).isEmpty()) { //find e.g.: building(current)
    buildings = findBuildings(Koushin::ActionParser::separateNameAndParameters(linePart).second, owner, towns);
  } else {
    buildings = findBuildings(QStringList(), owner, towns);
  }
  if(buildings.isEmpty() && expectedObject != "field") {
    kDebug() << "No reasonable building found" << linePart;
    return QList<Koushin::ActionObject* >();
  }
  if(expectedObject == "building") {
    QList<Koushin::ActionObject* > objects;
    foreach(Koushin::Building* building, buildings)
      objects << (Koushin::ActionObject*)building;
    return objects;
  }
//find Fields:
  QList<Koushin::Field* > fields;
  if(!(linePart = configLine.section(QRegExp("field([A-Za-z0-9,=]*)"), 0)).isEmpty()) { //find e.g.: field(current)
    fields = findFields(Koushin::ActionParser::separateNameAndParameters(linePart).second, owner, towns);
  } else {
    fields = findFields(QStringList(), owner, towns);
  }
  if(fields.isEmpty()) {
    kDebug() << "No reasonable field found" << linePart;
    return QList<Koushin::ActionObject* >();
  }
  if(expectedObject == "field") {
    QList<Koushin::ActionObject* > objects;
    foreach(Koushin::Field* field, fields)
      objects << (Koushin::ActionObject*)field;
    return objects;
  }

  kDebug() << "Config contains unexpected recipient type: " << expectedObject;
  return QList<Koushin::ActionObject* >();
}
  
QList< Koushin::Player* > Koushin::ActionParser::findPlayers(QStringList parameters, Koushin::ActionObject* owner)
{
  QList<Koushin::Player* > players;
  if(parameters.isEmpty()) parameters.insert(0, "current"); //set "current" as default

  switch(owner->getActionObjectType()) {
    case Koushin::actionObjectIsBuiling:
      if(parameters.value(0) == "current")
	players << ((Koushin::Building*)owner)->getTown()->getOwner();
      break;
    case Koushin::actionObjectIsField:
      if(parameters.value(0) == "current")
	players << ((Koushin::Field*)owner)->getTown()->getOwner();
      break;
    case Koushin::actionObjectIsTown:
      if(parameters.value(0) == "current")
	players << ((Koushin::Town*)owner)->getOwner();
      break;
    case Koushin::actionObjectIsPlayer:
      if(parameters.value(0) == "current")
	players << (Koushin::Player*)owner;
      break;
    default:
      kDebug() << "Unknown action object type";
  }
  ///@todo implement filters for players here
  return players;
}

QList<Koushin::Town* > Koushin::ActionParser::findTowns(QStringList parameters, Koushin::ActionObject* owner, QList<Koushin::Player* > players)
{
  QList<Koushin::Town* > towns;
  if(parameters.isEmpty()) parameters.insert(0, "current");
//parse parameter 0:  
  switch(owner->getActionObjectType()) {
    case Koushin::actionObjectIsBuiling:
      if(parameters.value(0) == "current")
	towns << ((Koushin::Building*)owner)->getTown();
      break;
    case Koushin::actionObjectIsField:
      if(parameters.value(0) == "current")
	towns << ((Koushin::Field*)owner)->getTown();
      break;
    case Koushin::actionObjectIsTown:
      if(parameters.value(0) == "current")
	towns << (Koushin::Town*)owner;
      break;
    case Koushin::actionObjectIsPlayer:
      if(parameters.value(0) == "current")
	kDebug() << "\"current\" does not make sense for a player as owner";
      break;
    default:
      kDebug() << "Unknown action object type";
  }
//sort out the towns which are not owned by the players in list:
  foreach(Koushin::Town* town, towns) {
    if(!players.contains(town->getOwner())) towns.removeAll(town);
  }
  ///@todo implement filters for towns here
  return towns;
}

QList< Koushin::Building* > Koushin::ActionParser::findBuildings(QStringList parameters, Koushin::ActionObject* owner, QList< Koushin::Town* > towns)
{
  QList<Koushin::Building* > buildings;
  if(parameters.isEmpty()) parameters.insert(0, "current");
//parse parameter 0:  
  switch(owner->getActionObjectType()) {
    case Koushin::actionObjectIsBuiling:
      if(parameters.value(0) == "current")
	buildings << (Koushin::Building*)owner;
      break;
    case Koushin::actionObjectIsField:
      if(parameters.value(0) == "current") {
	Koushin::Field* field = (Koushin::Field*)owner;
	if (field->getBuilding())
	  buildings << field->getBuilding();
      }
      break;
    case Koushin::actionObjectIsTown:
      if(parameters.value(0) == "current")
	kDebug() << "\"current\" does not make sense for a town as owner";
      break;
    case Koushin::actionObjectIsPlayer:
      if(parameters.value(0) == "current")
	kDebug() << "\"current\" does not make sense for a player as owner";
      break;
    default:
      kDebug() << "Unknown action object type.";
  }
    
//sort out the towns which are not owned by the players in list:
  foreach(Koushin::Building* building, buildings) {
    if(!towns.contains(building->getTown())) buildings.removeAll(building);
  }
  ///@todo implement filters for towns here
  return buildings;
}

QList<Koushin::Field* > Koushin::ActionParser::findFields(QStringList parameters, Koushin::ActionObject* owner, QList< Koushin::Town* > towns)
{
  QList<Koushin::Field* > fields;
  if(parameters.isEmpty()) parameters.insert(0, "current");
//parse parameter 0:  
  switch(owner->getActionObjectType()) {
    case Koushin::actionObjectIsBuiling:
      if(parameters.value(0) == "current")
	fields << ((Koushin::Building*)owner)->getTown()->getFieldFromBuilding((Koushin::Building*)owner);
      break;
    case Koushin::actionObjectIsField:
      if(parameters.value(0) == "current") {
	  fields << (Koushin::Field*)owner;
      }
      break;
    case Koushin::actionObjectIsTown:
      if(parameters.value(0) == "current")
	kDebug() << "\"current\" does not make sense for a town as owner";
      break;
    case Koushin::actionObjectIsPlayer:
      if(parameters.value(0) == "current")
	kDebug() << "\"current\" does not make sense for a player as owner";
      break;
    default:
      kDebug() << "Unknown action object type.";
  }
    
//sort out the towns which are not owned by the players in list:
  foreach(Koushin::Field* field, fields) {
    if(!towns.contains(field->getTown())) fields.removeAll(field);
  }
  ///@todo implement filters for towns here
  return fields;
}


void Koushin::ActionParser::createOpenFieldActions(KConfigGroup* config, Koushin::Building* building)
{
  foreach(QString name, config->groupList()) {
    KConfigGroup* taskGroup = new KConfigGroup(config->group(name));
    qreal number = taskGroup->readEntry("numberOfFields", qreal(1));
    number += (qreal)building->getLevel() * taskGroup->readEntry("numberOfFieldsPerLevel", qreal(0));
    number -= building->getNumberOfCreatedOpenFieldActions(name);
    kDebug() << "Create " << (int)number << " actions of " << taskGroup->name();
    for(int i = 0; i < (int)number; ++i) {
      building->addOpenFieldAction(name);
    }
  }
}
