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


#include "action.h"
#include "citizenaction.h"
#include "townaction.h"
#include "playeraction.h"

Koushin::Action::Action()
  : m_priority(100)
{

}

Koushin::Action::~Action()
{

}

QMap< QString, Koushin::ActionProperties > Koushin::Action::getPossibleActions()
{
  return Koushin::CitizenAction::getPossibleActions().
      unite(Koushin::PlayerAction::getPossibleActions().
      unite(Koushin::TownAction::getPossibleActions()));
}

bool Koushin::Action::requirementsFinished()
{
  QList<Koushin::Action* > actions = m_requirements.keys();
  for(QList<Action* >::const_iterator it = actions.begin(); it != actions.end(); ++it) {
    if(((*it)->getStatus() == Koushin::actionSucceeded) & (m_requirements.value(*it))) continue;
    if(((*it)->getStatus() == Koushin::actionFailed) & (!m_requirements.value(*it))) continue;
    return false;
  }
  return true;
}

// bool Koushin::Action::getFunctionName(QString function, int test)
// {
//   kDebug() << function;
//   return 1;
// }
