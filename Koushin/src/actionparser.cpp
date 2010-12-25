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
// #include "citizenaction.h"
#include "player.h"
#include "town.h"
// #include "playeraction.h"
// #include "townaction.h"

#include <QRegExp>
#include <QString>

#include <kdebug.h>
#include "action.h"

QPair< QString, QStringList > Koushin::ActionParser::separateNameAndParameters(QString string)
{
  QString actionName = string.split("(")[0];
  string.remove(0, string.indexOf("(")+1);   //remove all untill/inclusive the first "("
  string.remove(string.length()-1, 1); //remove last char, should be ")"
  QStringList parameters = string.split(",");
  if (string.isEmpty()) parameters = QStringList();
  return QPair<QString, QStringList>(actionName, parameters);
}

QList< Koushin::Action* > Koushin::ActionParser::createActionsFromConfig(const KConfigGroup& tasksGroup, ActionObject* newOwner, int currentRound)
{
  QMap<QString, Koushin::Action* > actions;
  QStringList actionNames = tasksGroup.groupList();
//Create Actions:
  foreach(QString actionName, actionNames) {
    if(actionName == "globals" || actionName == "conditions") continue;
    KConfigGroup* actionGroup = new KConfigGroup(tasksGroup.group(actionName));
    Koushin::Action* newAction = new Koushin::Action();
    newAction->setOwner(newOwner);
    newAction->setConfiguration(actionGroup);
    setRoundLimit(newAction, *actionGroup, currentRound);
    actions.insert(actionName, newAction);
  }
//write conditions to action:
  KConfigGroup conditionGroup = tasksGroup.group("conditions");
  QStringList conditionsList;
  foreach(QString condition, conditionGroup.keyList())
    conditionsList << conditionGroup.readEntryUntranslated(condition, QString());
  addRequirementsToActions(conditionsList, actions);
  
  return actions.values();
}

void Koushin::ActionParser::setRoundLimit(Koushin::Action* action, KConfigGroup config, int currentRound)
{
  if(!action) return;
  int fromRound = config.readEntry("fromRound", int(-1));
  int toRound = config.readEntry("toRound", int(-1));
  int intervall = config.readEntry("intervall", int(-1));
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
  if(!(linePart = configLine.section(QRegExp("building([A-Za-z0-9,=]*)"), 0)).isEmpty()) { //find e.g.: town(all,hasBuilding=lumbermill)
    buildings = findBuildings(Koushin::ActionParser::separateNameAndParameters(linePart).second, owner, towns);
  } else {
    buildings = findBuildings(QStringList(), owner, towns);
  }
  if(buildings.isEmpty()) {
    kDebug() << "No reasonable town found" << linePart;
    return QList<Koushin::ActionObject* >();
  }
  if(expectedObject == "building") {
    QList<Koushin::ActionObject* > objects;
    foreach(Koushin::Building* building, buildings)
      objects << (Koushin::ActionObject*)building;
    return objects;
  }

  kDebug() << "Config contains unexpected recipient type: " << expectedObject;
  return QList<Koushin::ActionObject* >();
}
  
QList< Koushin::Player* > Koushin::ActionParser::findPlayers(QStringList parameters, Koushin::ActionObject* owner)
{
  QList<Koushin::Player* > players;
  if(parameters.isEmpty()) parameters.insert(0, "current"); //set "current" as default
  
  if(parameters.value(0) == "current") {
    if(owner->getActionObjectType() == Koushin::actionObjectIsBuiling)
      players << ((Koushin::Building*)owner)->getTown()->getOwner();
    else if(owner->getActionObjectType() == Koushin::actionObjectIsTown)
      players << ((Koushin::Town*)owner)->getOwner();
    else if(owner->getActionObjectType() == Koushin::actionObjectIsPlayer)
      players << (Koushin::Player*)owner;
  }
  ///@todo implement filters for players here
  return players;
}

QList<Koushin::Town* > Koushin::ActionParser::findTowns(QStringList parameters, Koushin::ActionObject* owner, QList<Koushin::Player* > players)
{
  QList<Koushin::Town* > towns;
  if(parameters.isEmpty()) parameters.insert(0, "current");
//parse parameter 0:  
  if(parameters.value(0) == "current") {
    if(owner->getActionObjectType() == Koushin::actionObjectIsBuiling)
      towns << ((Koushin::Building*)owner)->getTown();
    else if(owner->getActionObjectType() == Koushin::actionObjectIsTown)
      towns << (Koushin::Town*)owner;
    else if(owner->getActionObjectType() == Koushin::actionObjectIsPlayer)
      kDebug() << "\"current\" does not make sense for a player as owner";
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
  if(parameters.value(0) == "current") {
    if(owner->getActionObjectType() == Koushin::actionObjectIsBuiling)
      buildings << (Koushin::Building*)owner;
    else if(owner->getActionObjectType() == Koushin::actionObjectIsTown)
      kDebug() << "\"current\" does not make sense for a town as owner";
    else if(owner->getActionObjectType() == Koushin::actionObjectIsPlayer)
      kDebug() << "\"current\" does not make sense for a player as owner";
  }
//sort out the towns which are not owned by the players in list:
  foreach(Koushin::Building* building, buildings) {
    if(!towns.contains(building->getTown())) buildings.removeAll(building);
  }
  ///@todo implement filters for towns here
  return buildings;
}

/*  
  QStringList recipient = configLine.split("->");
  for (QStringList::const_iterator it = recipient.begin(); it != recipient.end();) {
    QPair<QString, QStringList> pair =  separateNameAndParameters(*it);
    QString object = pair.first;
    QString parameter = pair.second.value(0, "");
    if (object == "player" && !findPlayer(parameter)) {
      kDebug() << "No reasonable player found" << parameter;
      return 0;
    }
    if (object == "town" && !findTown(parameter)) {
      kDebug() << "No reasonable town found" << parameter;
      return 0;
    }
    if (++it == recipient.end()) { //create right actionClass
      if (object == "player") {
	m_action = new Koushin::PlayerAction(m_player);
      } else if (object == "town") {
	m_action = new Koushin::TownAction(m_town);
      } else {
	kDebug() << "Unknown recipient: can't create action";
	return 0;
      }
    }
  }
  return 1;*/

// Koushin::ActionParser::ActionParser(ActionOwner* owner)
//   : m_action(0)
//   , m_player(0)
//   , m_town(0)
//   , m_owner(owner)
// {
//   if(m_owner->getActionOwnerType() == Koushin::actionOwnerIsBuilding) {
//     m_player = ((Koushin::Building*)m_owner)->getTown()->getOwner();
//   } else {
//     kDebug() << "Actions not owned by a building is not implemented yet.";
//   }
// }
// 
// Koushin::ActionParser::~ActionParser()
// {
// }

// QList<Koushin::Action* > Koushin::ActionParser::parseConfig(const KConfigGroup& config)
// {
//   QMap<QString, Koushin::Action*> actions;
//   QStringList taskConfigList = config.groupList();
//   parseGlobals(KConfigGroup(&config, "globals"));
// 
//   for (QStringList::const_iterator it = taskConfigList.begin(); it != taskConfigList.end(); ++it) {
//     if (*it == "conditions" || *it == "globals") continue;
//     m_action = 0;
//     KConfigGroup task(&config, *it);
//     if (!task.hasKey("recipient")) {
//       kDebug() << "Config for " << task.name() << " doesn't contain a recipient. Can't parse this entry";
//       continue;
//     }
//     QString recipient = task.readEntry("recipient", QString());
//     if (!parseRecipient(recipient)) {
//       kDebug() << "Can't parse recipient: " << recipient;
//       continue;
//     }
//     QString actionString = task.readEntry("action", QString());
//     int priority = task.readEntry("priority", int(10));
//     if (!parseAction(actionString, priority)) {
//       kDebug() << "Can't parse action: " << actionString;
//       continue;
//     }
//     kDebug() << "Create Action " << *it << " for " << recipient << " to do " << actionString;
//     actions.insert(*it, m_action);
//   }
//   //add Requirements after initializing all actions
//   KConfigGroup conditions(&config, "conditions");
//   QStringList conditionStrings;
//   foreach(QString condition, conditions.keyList())
//     conditionStrings << conditions.readEntry(condition, QString());
//   addRequirementsToActions(conditionStrings, actions);
//   
//   return actions.values();
// }


// bool Koushin::ActionParser::parseAction(const QString& actionString, int priority)
// {
//   if (!m_action) {
//     kDebug() << "Parse recipient before action";
//     return 0;
//   }
//   //Seperate action name and parameters
//   QPair<QString, QStringList> pair = separateNameAndParameters(actionString);
//   QString actionName = pair.first;
//   QStringList parameters = pair.second;
//   
//   if (m_owner->getActionOwnerType() == Koushin::actionOwnerIsBuilding) {
//     if (!m_action->getPossibleActions().contains(actionName)) {
//       kDebug() << "Action " << actionName << " unknown.";
//       return 0;
//     }
//     m_action->addAction(actionName);
//     if (!possibleParametersGiven(actionName, parameters)) {
//       kDebug() << "Given Parameters fit not in action definition";
//       return 0;
//     }
//     m_action->addParameters(parameters);
//   } else if (m_owner->getActionOwnerType() == Koushin::actionOwnerIsSpell) {
//     kDebug() << "Spells not implemented yet. Sorry!";
//     return 0;
//   } else {
//     return 0;
//   }
//   m_action->setPriority(priority);
//   return 1;
// }



// bool Koushin::ActionParser::findPlayer(const QString& parameter)
// {
//   if (parameter == "current" || parameter == "") {//current is default, of no parameter is given
//     switch(m_owner->getActionOwnerType()) {
//       case Koushin::actionOwnerIsBuilding:
// 	m_player = ((Building*)m_owner)->getTown()->getOwner();
// 	return true;
//       default:
// 	kDebug() << "The ActionOwner is not kwown.";
// 	return false;
//     }
//   }
//   return 0;
// }

// bool Koushin::ActionParser::findTown(const QString& parameter)
// {
//   if (parameter == "current" || parameter == "") {//current is default, of no parameter is given
//     switch(m_owner->getActionOwnerType()) {
//       case Koushin::actionOwnerIsBuilding:
// 	m_town = ((Building*)m_owner)->getTown();
// 	return true;
//       case Koushin::actionOwnerIsSpell:
// 	kDebug() << "Spells is still not handeled. Sorry.";
// 	return false;
//       default:
// 	kDebug() << "The ActionOwner is not kwown.";
// 	return false;
//     }
//   } else if (parameter == "random") {
//     switch(m_owner->getActionOwnerType()) {
//       case Koushin::actionOwnerIsBuilding:
// 	if (!m_player) { //select as default the player who owns the building
// 	  m_player = ((Building*)m_owner)->getTown()->getOwner();
// 	}
// 	m_player->getTowns().value(qrand() % m_player->getTowns().size());
// 	return 1;
//       case Koushin::actionOwnerIsSpell:
// 	kDebug() << "Spells is still not handeled. Sorry.";
// 	return 0;
//       default:
// 	kDebug() << "The ActionOwner is not kwown.";
// 	return 0;
//     }
//   } else {
//       kDebug() << "The given parameter is unknown for towns: " << parameter;
//       return 0;
//     }
// }

