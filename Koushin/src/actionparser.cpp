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
#include "building.h"
#include "citizenaction.h"
#include "playeraction.h"
#include "townaction.h"

#include <QRegExp>
#include <QString>

Koushin::ActionParser::ActionParser(ActionOwner* owner, ActionManager* manager)
  : m_action(0)
  , m_manager(manager)
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
  return m_action;
}

bool Koushin::ActionParser::parseRecipient(const QString& configLine)
{
  QStringList recipient = configLine.split("->");
  for (QStringList::const_iterator it = recipient.begin(); it != recipient.end();) {
    QString object = it->split("(")[0];
    QString parameter = *it;
    parameter.remove(0, it->indexOf("(")+1);   //remove all untill/inclusive the first "("
    parameter.remove(parameter.length()-1, 1); //remove last char, should be ")"
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
	m_action = new Koushin::PlayerAction(m_player;
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

bool Koushin::ActionParser::findPlayer(const QString& parameter)
{
  if (parameter.isEmpty()) return 0;
  if (parameter == "current" || parameter == "") {//current is default, of no parameter is given
    m_player = m_manager->getOwner();
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
