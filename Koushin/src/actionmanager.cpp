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

#include "actionmanager.h"

#include "action.h"
#include "player.h"

Koushin::ActionManager::ActionManager(Player* owner)
  : m_owner(owner)
{
  m_owner->setActionManager(this);
}

Koushin::ActionManager::~ActionManager()
{

}

void Koushin::ActionManager::executeActions()
{
  QList<int > priorityList = m_actions.uniqueKeys();
  qSort(priorityList.begin(), priorityList.end(), qGreater<int>()); //execute first actions with height priority
  for (QList<int >::const_iterator it = priorityList.begin(); it != priorityList.end(); ++it) {
    QList<Koushin::Action* > currentActions = m_actions.values(*it);
    foreach(Koushin::Action* action, currentActions) {
      action->execute();
    }
  }
}

void Koushin::ActionManager::addAction(Koushin::Action* action)
{
  m_actions.insert(action->getPriority(), action); //insert is like insertMulti for normal QMaps
}
