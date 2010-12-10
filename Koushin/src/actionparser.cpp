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
#include "citizenaction.h"
#include "player.h"
#include "playeraction.h"
#include "townaction.h"

#include <QRegExp>
#include <QString>

Koushin::ActionParser::ActionParser(ActionOwner* owner)
  : m_action(0)
  , m_player(0)
  , m_town(0)
  , m_owner(owner)
{

}

Koushin::ActionParser::~ActionParser()
{
}

#include <kdebug.h>
Koushin::Action* Koushin::ActionParser::parseConfig(const KConfigGroup& config)
{
  if (!config.hasKey("recipient")) {
    kDebug() << "Config for " << config.name() << " doesn't contain a recipient. Can't parse this entry";
    return 0;
  }
  QString recipient = config.readEntry("recipient", QString());
  if (!parseRecipient(recipient)) {
    kDebug() << "Can't parse recipient: " << recipient;
    return 0;
  }
  QString actionString = config.readEntry("action", QString());
  if (!parseAction(actionString)) {
    kDebug() << "Can't parse action: " << actionString;
    return 0;
  }
  return m_action;
}

bool Koushin::ActionParser::parseRecipient(const QString& configLine)
{
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
	kDebug() << "Create PlayerAction";
	m_action = new Koushin::PlayerAction(m_player);
      } else if (object == "town") {
	kDebug() << "Create TonwAction";
	m_action = new Koushin::TownAction(m_town);
      } else {
	kDebug() << "Unknown recipient: can't create action";
	return 0;
      }
    }
  }
  return 1;
}

bool Koushin::ActionParser::parseAction(const QString& actionString)
{
  if (!m_action) {
    kDebug() << "Parse recipient before action";
    return 0;
  }
  //Seperate action name and parameters
  QPair<QString, QStringList> pair = separateNameAndParameters(actionString);
  QString actionName = pair.first;
  QStringList parameters = pair.second;
  
  if (m_owner->getActionOwnerType() == Koushin::actionOwnerIsBuilding) {
    if (!m_action->getPossibleActions().contains(actionName)) {
      kDebug() << "Action " << actionName << " unknown.";
      return 0;
    }
    m_action->addAction(actionName);
    if (!possibleParametersGiven(actionName, parameters)) {
      kDebug() << "Given Parameters fit not in action definition";
      return 0;
    }
    m_action->addParameters(parameters);
    return 1;
  } else if (m_owner->getActionOwnerType() == Koushin::actionOwnerIsSpell) {
    kDebug() << "Spells not implemented yet. Sorry!";
    return 0;
  } else {
    return 0;
  }
}

bool Koushin::ActionParser::findPlayer(const QString& parameter)
{
  if (parameter.isEmpty()) return 0;
  if (parameter == "current" || parameter == "") {//current is default, of no parameter is given
    m_player = ((Building*)m_owner)->getTown()->getOwner();
    return 1;
  }
  return 0;
}

bool Koushin::ActionParser::findTown(const QString& parameter)
{
  if (parameter == "current" || parameter == "") {//current is default, of no parameter is given
    switch(m_owner->getActionOwnerType()) {
      case Koushin::actionOwnerIsBuilding:
	m_town = ((Building*)m_owner)->getTown();
	return 1;
      case Koushin::actionOwnerIsSpell:
	kDebug() << "Spells is still not handeled. Sorry.";
	return 0;
      default:
	kDebug() << "The ActionOwner is not kwown.";
	return 0;
    }
  } else if (parameter == "random") {
    switch(m_owner->getActionOwnerType()) {
      case Koushin::actionOwnerIsBuilding:
	if (!m_player) { //select as default the player who owns the building
	  m_player = ((Building*)m_owner)->getTown()->getOwner();
	}
	m_player->getTowns().value(qrand() % m_player->getTowns().size());
	return 1;
      case Koushin::actionOwnerIsSpell:
	kDebug() << "Spells is still not handeled. Sorry.";
	return 0;
      default:
	kDebug() << "The ActionOwner is not kwown.";
	return 0;
    }
  } else {
      kDebug() << "The given parameter is unknown for towns: " << parameter;
      return 0;
    }
}

QPair< QString, QStringList > Koushin::ActionParser::separateNameAndParameters(QString string)
{
  QString actionName = string.split("(")[0];
  string.remove(0, string.indexOf("(")+1);   //remove all untill/inclusive the first "("
  string.remove(string.length()-1, 1); //remove last char, should be ")"
  QStringList parameters = string.split(",");
  if (string.isEmpty()) parameters = QStringList();
  return QPair<QString, QStringList>(actionName, parameters);
}

bool Koushin::ActionParser::possibleParametersGiven(QString actionName, QStringList parameters)
{
  if(!m_action || !m_action->getPossibleActions().contains(actionName)) {
    kDebug() << "Action is not in list of possible action";
    return 0;
  }
  Koushin::ActionProperties properties = m_action->getPossibleActions().value(actionName);
  if (parameters.length() != properties.parameterTypes.length()) {
    kDebug() << "Wrong parameter Number: Expected = " << properties.parameterTypes.length() << "; Given = " << parameters.length();
    return 0;
  }
  for (int i = 0; i <= properties.parameterTypes.length(); ++i) {
    if (properties.parameterTypes.value(i) == "int") {
      bool succes;
      parameters.value(i).toInt(&succes);
      if (!succes){
	kDebug() << "Integer value expected: Can't generate Integer from " << properties.parameterTypes.value(i);
	return 0;
      }
    }
  }
  return 1;
}
