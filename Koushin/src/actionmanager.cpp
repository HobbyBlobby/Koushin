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
#include <kdebug.h>

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
//reset all action before executing
  foreach(Action* action, m_actions.values()) { 
    action->resetAction();
  }
//get all priorities and sort them descending
  QList<int > priorityList = m_actions.uniqueKeys();
  qSort(priorityList.begin(), priorityList.end(), qGreater<int>()); //execute first actions with height priority
//execute actions beginning with highest priority
  QList<Koushin::Action* > actionsToExecute;
  for (QList<int >::const_iterator it = priorityList.begin(); it != priorityList.end(); ++it) {
    actionsToExecute << m_actions.values(*it); //apend actions to ToDo-List
    kDebug() << "Execute jobs with priority = " << *it;
    foreach(Koushin::Action* action, actionsToExecute) {
      if(executeAction(action)) {
	actionsToExecute.removeAll(action); //action executed, remove from list (and all [not possible] duplicates)
      }
    }
  }
  //some dependencies are not execute, but the list of priorities is empty (e.g. some actions requiere actions with lowest priority)
  while(!actionsToExecute.isEmpty()) { //make list empty
    foreach(Koushin::Action* action, actionsToExecute) {
      if(executeAction(action)) {
	actionsToExecute.removeAll(action);
      }
    }
  }
}

//false does not mean the action is failed, false means the action can not be executed because of missing requirements
bool Koushin::ActionManager::executeAction(Koushin::Action* action)
{
  if(action->getStatus() == Koushin::actionFailed) {
    return true; //the action can not be execute, but it is not necessary to try it again
  }
  if(action->getStatus() != Koushin::actionNotStarted) {
    return false; //action is no ready, skip at this moment
  }
  if(!action->requirementsFinished()) {
    action->setStatus(Koushin::actionWaitsForRequirement);
    setStatusOfDependensies(action);
    return false;
  }
  kDebug() << "Execute action " << action->getActionString() << " with " << action->getParameters();
  if(action->execute()) {
    action->setStatus(Koushin::actionSucceeded);
  } else {
    action->setStatus(Koushin::actionFailed);
  }
  setStatusOfDependensies(action);
  return true;
}


void Koushin::ActionManager::addAction(Koushin::Action* action)
{
  m_actions.insert(action->getPriority(), action); //insert is like insertMulti for normal QMaps
}

void Koushin::ActionManager::addAction(QList< Koushin::Action* > actions)
{
  foreach(Koushin::Action* action, actions)
    addAction(action);
}

void Koushin::ActionManager::setStatusOfDependensies(Koushin::Action* action)
{
  foreach(Koushin::Action* dependency, action->getDependencies().keys()) {
    if(action->getStatus() == Koushin::actionFailed && action->getDependencies().value(dependency) == true) {
      dependency->setStatus(Koushin::actionFailed);
    }
    if(action->getStatus() == Koushin::actionFailed && action->getDependencies().value(dependency) == false) {
      dependency->closeRequirement(action);
      if(dependency->noOpenRequirements())
	dependency->setStatus(Koushin::actionNotStarted);
    }
    if(action->getStatus() == Koushin::actionSucceeded && action->getDependencies().value(dependency) == false) {
      dependency->setStatus(Koushin::actionFailed);
    }
    if(action->getStatus() == Koushin::actionSucceeded && action->getDependencies().value(dependency) == true) {
      dependency->closeRequirement(action);
      if(dependency->noOpenRequirements())
	dependency->setStatus(Koushin::actionNotStarted);
    }
    if(action->getStatus() == Koushin::actionNotStarted || action->getStatus() == Koushin::actionWaitsForRequirement) {
      dependency->setStatus(Koushin::actionWaitsForRequirement);
    }
    setStatusOfDependensies(dependency);
  }
}
