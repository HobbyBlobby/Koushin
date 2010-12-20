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


#include "playeraction.h"
#include "player.h"
#include "actionmanager.h"

Koushin::PlayerAction::PlayerAction(Player* recipient)
  : Koushin::Action()
  , m_recipient(recipient)
{

}

Koushin::PlayerAction::~PlayerAction()
{

}

QMap< QString, Koushin::ActionProperties> Koushin::PlayerAction::getPossibleActions()
{
  QMap<QString, Koushin::ActionProperties> actions;
  actions.insert("setGlobalTo", Koushin::ActionProperties(
    QStringList() << "string" << "string",
    "Player: overwrites a value. string=NameOfGlobal, string=NewValue "));
  actions.insert("addToGlobal", Koushin::ActionProperties(
    QStringList() << "string" << "string",
    "Player: add a string to a global. string=NameOfGlobal, string=AdditionalContent"));
  return actions;
}

bool Koushin::PlayerAction::execute()
{
  if(m_action == "setGlobalTo") {
    return m_recipient->getActionManager()->setGlobalParameterContent(m_parameters[0], m_parameters[1]);
  } else if(m_action == "addToGlobal") {
    return m_recipient->getActionManager()->addContentToGlobalParameter(m_parameters[0], m_parameters[1]);
  } else {
    kDebug() << "Unknown action " << m_action;
  }
  return false;
}
