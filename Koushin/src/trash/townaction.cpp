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


#include "townaction.h"
#include "town.h"
#include "player.h"
#include "actionmanager.h"

Koushin::TownAction::TownAction(Town* recipient)
  : m_recipient(recipient)
{
//   PUBLIC_ACTION;
//   kDebug() << ACTION_NAME;
}

Koushin::TownAction::~TownAction()
{

}

QMap< QString, Koushin::ActionProperties> Koushin::TownAction::getPossibleActions()
{
  QMap<QString, Koushin::ActionProperties> actions;
  actions.insert("increaseResource", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Town: Add the given parameter to the given resource. string=ResourceName, int=difference"));
  actions.insert("decreaseResource", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Town: Removes the given parameter from the given resource. string=ResourceName, int=difference"));
  actions.insert("setResourceCapacity", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Town: Sets the capacity of a given resource to the given value. string=ResourceName, int=new value"));
  return actions;
}

bool Koushin::TownAction::execute()
{
  if(m_action == "increaseResource") {
    Koushin::ResourceType type = Koushin::Town::getResourceTypeFromQString(m_parameters[0]);
    try{
      int diff = m_recipient->getOwner()->getActionManager()->evalContent(m_parameters.value(1, QString("0")));
      return m_recipient->changeResource(type, diff);
    }
    catch (std::exception & e) {return false;}
  }
  else if(m_action == "decreaseResource") {
    Koushin::ResourceType type = Koushin::Town::getResourceTypeFromQString(m_parameters[0]);
    try{
      int diff = m_recipient->getOwner()->getActionManager()->evalContent(m_parameters.value(1, QString("0")));
      return m_recipient->changeResource(type, -diff);
    }
    catch (std::exception & e) {return false;}
  }
  else if(m_action == "setResourceCapacity") {
    Koushin::ResourceType type = Koushin::Town::getResourceTypeFromQString(m_parameters[0]);
    try{
      int value = m_recipient->getOwner()->getActionManager()->evalContent(m_parameters.value(1, QString("0")));
      kDebug() << "####=> set Capacity to: " << value;
      return m_recipient->setResourceCapacity(type, value);
    }
    catch (std::exception & e) {return false;}
  } else {
    kDebug() << "Unknown action " << m_action;
  }
  return false; //action not parsed -> return false
}

