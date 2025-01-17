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

#include <stdexcept>
#include <QScriptEngine>
#include "building.h"

Koushin::ActionManager::ActionManager(Player* owner)
  : m_owner(owner)
{
  m_owner->setActionManager(this);
}

Koushin::ActionManager::~ActionManager()
{

}

void Koushin::ActionManager::executeActions(int currentRound)
{
  QMap<int, Koushin::Action* > actions;
  foreach(Koushin::ActionObject* object, m_actionObjects) {
    foreach(Koushin::Action* action, object->getAllActions()) {
      action->resetAction();
      actions.insert(action->getPriority(), action);
    }
  }
  kDebug() << "Number of Objects: " << m_actionObjects.size() << ". Number of Actions: " << actions.size();
  
//get all priorities and sort them descending
  QList<int > priorityList = actions.uniqueKeys();
  qSort(priorityList.begin(), priorityList.end(), qGreater<int>()); //execute first actions with height priority
//execute actions beginning with highest priority
  QList<Koushin::Action* > actionsToExecute;
  for (QList<int >::const_iterator it = priorityList.begin(); it != priorityList.end(); ++it) {
    actionsToExecute << actions.values(*it); //apend actions to ToDo-List
    kDebug() << "Execute jobs with priority = " << *it;
    foreach(Koushin::Action* action, actionsToExecute) {
      if(!action->shouldExecuteInEveryRound() && !action->getExecutionRounds().contains(currentRound)) {
	actionsToExecute.removeAll(action);
	continue;
      }
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
    kDebug() << "action " << action->getActionString() << " succeded";
  } else {
    action->setStatus(Koushin::actionFailed);
    kDebug() << "action " << action->getActionString() << " failed";
  }
  setStatusOfDependensies(action);
  return true;
}

bool Koushin::ActionManager::addActionObject(Koushin::ActionObject* object)
{
  if(m_actionObjects.contains(object)) return false;
  m_actionObjects << object;
  return true;
}

bool Koushin::ActionManager::removeActionObject(Koushin::ActionObject* object)
{
  if(!m_actionObjects.contains(object)) return false;
  m_actionObjects.removeAll(object);
  return true;
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

bool Koushin::ActionManager::addGlobalParameter(QString name, QString content)
{
  if(m_globalParameters.keys().contains(name)) {
    kDebug() << "Parameter allready in use. I do not overwright it. Use instead setGlobalTo or addToGlobal. " << name;
    return false;
  }
  if(name.contains(QRegExp("![A-Za-z0-9]*"))) {
    kDebug() << "Use only letters and numbers for parameter names: " << name;
    return false;
  }
  //test for endless loops
  if(expandParameter(content, name) == "NOT_VALID")
  {
    kDebug() << "Content of " << name << " is not expandable";
    return false;
  }
  m_globalParameters.insert(name, content);
  return true;
}

int Koushin::ActionManager::evalContent ( QString content, QString parameter )
{
  content = expandParameter(content, parameter);
  if (content == "NOT_VALID") {
    kDebug() << "Can not  expand parameter: " << parameter;
    throw std::runtime_error(QString("Parameter not expandable:" + parameter).toAscii().constData());
    return 0;
  }
  if(QScriptEngine::checkSyntax(content).state() != QScriptSyntaxCheckResult::Valid) {
    kDebug() << "Sorry, but can not calculate string: " << content;
    throw std::runtime_error(QString("Parameter can not calculated:" + parameter).toAscii().constData());
  }
  QScriptEngine calc;
  return calc.evaluate(content).toInteger();
}


int Koushin::ActionManager::evalParameter(QString parameter)
{
  //Do not write expanded content to m_globalParameters: later manipulation of values should be possible
//   kDebug() << "Expand parameter: " << parameter << " = " << m_globalParameters.value(parameter);
  return evalContent(m_globalParameters.value(parameter), parameter);
}

QString Koushin::ActionManager::expandParameter(QString line, QString name)
{
  QString part;
  int replacePart = 1;
  while((part = line.section("#", replacePart, replacePart)) != "") {
    if (line.contains("#" + name + "#")) {
      kDebug() << "Found recursion." << line << " contains " << name;
      return QString("NO_VALID");
    }
    if(m_globalParameters.keys().contains(part)) {
      line.replace("#" + part + "#", expandParameter(m_globalParameters.value(part), part));
    } else {
      replacePart += 2; //jump to next part => skip unknown Parameter
    }
  }
  return line;
}

bool Koushin::ActionManager::addContentToGlobalParameter ( QString name, QString content )
{
  if(!m_globalParameters.keys().contains(name)) {
    kDebug() << "Global not known: " << name;
    return false;
  }
  m_globalParameters.insert(name, m_globalParameters.value(name) + content);
  kDebug() << "new content is " << m_globalParameters.value(name);
  return true;
}

bool Koushin::ActionManager::setGlobalParameterContent ( QString name, QString content )
{
  if(!m_globalParameters.keys().contains(name)) {
    kDebug() << "Global not known: " << name;
    return false;
  }
  m_globalParameters.insert(name, content); //overwrites content, because it is not a QMultiMap
  return true;
}

void Koushin::ActionManager::resetGlobalParameters()
{
  m_globalParameters.clear();
  foreach(Koushin::ActionObject* object, m_actionObjects) {
    if(object->getActionObjectType() == Koushin::actionObjectIsBuilding)
      Koushin::ActionParser::parseGlobals(((Koushin::Building*)object)->getConfig()->group("tasks").group("globals"), this);
  }
}
