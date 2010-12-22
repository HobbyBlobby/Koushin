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
// #include "citizenaction.h"
// #include "townaction.h"
// #include "playeraction.h"
#include "actionobject.h"
#include "town.h"
#include "actionmanager.h"
#include "player.h"

Koushin::Action::Action()
  : m_config(0)
  , m_status(Koushin::actionNotStarted)
{

}

Koushin::Action::~Action()
{

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

bool Koushin::Action::addRequirement(Koushin::Action* action, bool positiv)
{
  if(getRecursiveRequirements(action).contains(this)) {
    kDebug() << "Can not insert requirement: it requires itself. This would cause a endless loop in action manager.";
    return false;
  }
  m_requirements.insert(action, positiv);
  if(!action->getDependencies().keys().contains(action)) {
    action->setAsRequirementFor(this, positiv);
  }
  return true;
}

bool Koushin::Action::setAsRequirementFor(Koushin::Action* action, bool positiv)
{
  //this function is only used by addRequirement
  m_requirementFor.insert(action, positiv);
  return true;
}

QList< Koushin::Action* > Koushin::Action::getRecursiveRequirements(Koushin::Action* action)
{
  QList<Action* > allRequirements(action->getRequirements().keys());
  foreach(Koushin::Action* action, action->getRequirements().keys()) {
    allRequirements << action->getRecursiveRequirements(action);
  }
  return allRequirements;
}

void Koushin::Action::resetAction()
{
  m_status = actionNotStarted;
  m_openRequirements = m_requirements.keys();
}

bool Koushin::Action::execute(bool failIfOneRecipientFailed)
{ //TODO: when a action execution is failed? ->if all recipients failed, or just one? -> make config option
  if(!m_config || !m_owner) return false;
  QString recipientLine = m_config->readEntry("recipient", QString());
  QList<Koushin::ActionObject* > recipients = Koushin::ActionParser::parseRecipient(recipientLine, m_owner);
  bool allRecipientsFailed = true;
  bool oneRecipientFailed = false;
  foreach(Koushin::ActionObject* recipient, recipients) {
    QString actionLine = m_config->readEntry("action", QString("NO_ACTION_GIVEN"));
    QPair<QString, QStringList> action = Koushin::ActionParser::separateNameAndParameters(actionLine);
    if(!possibleParametersGiven(recipient, action.first, action.second)) {
      oneRecipientFailed = true;
      continue;
    }
    if(Koushin::ActionObject::getPossibleActions().keys().contains(action.first)) {
      if(executeActionObjectAction(recipient, action))
	allRecipientsFailed = false;
      else
	oneRecipientFailed = true;
      continue;
    }
    switch(recipient->getActionObjectType()) {
      case Koushin::actionObjectIsBuiling:
	if(executeBuildingAction((Koushin::Building*)recipient, action))
	  allRecipientsFailed = false;
	else
	  oneRecipientFailed = true;
	break;
      case Koushin::actionObjectIsTown:
	if(executeTownAction((Koushin::Town*)recipient, action))
	  allRecipientsFailed = false;
	else
	  oneRecipientFailed = true;
	break;
      case Koushin::actionObjectIsPlayer:
	if(executePlayerAction((Koushin::Player*)recipient, action))
	  allRecipientsFailed = false;
	else
	  oneRecipientFailed = true;
	break;
      default:
	oneRecipientFailed = true;
    }
  }
  if(failIfOneRecipientFailed && oneRecipientFailed) return false;
  if(allRecipientsFailed) return false;
  return true;
}

bool Koushin::Action::executeTownAction(Koushin::Town* recipient, const QPair< QString, QStringList >& action)
{
  if(action.first == "increaseResource") {
    Koushin::ResourceType type = Koushin::Town::getResourceTypeFromQString(action.second[0]);
    try{
      int diff = recipient->getOwner()->getActionManager()->evalContent(action.second.value(1, QString("0")));
      return recipient->changeResource(type, diff);
    }
    catch (std::exception & e) {
      kDebug() << "Can not calculate " << action.second[1] << ". Reason: " << e.what();
      return false;
    }
  }
  else if(action.first == "decreaseResource") {
    Koushin::ResourceType type = Koushin::Town::getResourceTypeFromQString(action.second[0]);
    try{
      int diff = recipient->getOwner()->getActionManager()->evalContent(action.second.value(1, QString("0")));
      return recipient->changeResource(type, -diff);
    }
    catch (std::exception & e) {
      kDebug() << "Can not calculate " << action.second[1] << ". Reason: " << e.what();
      return false;
    }
  }
  else if(action.first == "setResourceCapacity") {
    Koushin::ResourceType type = Koushin::Town::getResourceTypeFromQString(action.second[0]);
    try{
      int value = recipient->getOwner()->getActionManager()->evalContent(action.second.value(1, QString("0")));
      return recipient->setResourceCapacity(type, value);
    }
    catch (std::exception & e) {
      kDebug() << "Can not calculate " << action.second[1] << ". Reason: " << e.what();
      return false;
    }
  } else {
    kDebug() << "Unknown action " << action.first;
  }
  return false; //action not parsed -> return false

}

bool Koushin::Action::executePlayerAction(Koushin::Player* recipient, const QPair< QString, QStringList >& action)
{
  if(action.first == "setGlobalTo") {
    return recipient->getActionManager()->setGlobalParameterContent(action.second[0], action.second[1]);
  } 
  else if(action.first == "addToGlobal") {
    return recipient->getActionManager()->addContentToGlobalParameter(action.second[0], action.second[1]);
  } 
  else {
    kDebug() << "Unknown action " << action.first;
  }
  return false;
}

bool Koushin::Action::executeBuildingAction(Koushin::Building* recipient, const QPair< QString, QStringList >& action)
{
  kDebug() << "No building actions specified.";
  return false;
}

bool Koushin::Action::executeActionObjectAction(Koushin::ActionObject* recipient, const QPair< QString, QStringList >& action)
{
  if(action.first == "setLocalTo") {
    if(action.second[2] == "true")
      return recipient->setLocalTo(action.second[0], action.second[1], true);
    else
      return recipient->setLocalTo(action.second[0], action.second[1], false);
  }
  else if(action.first == "addToLocal") {
    if(action.second[2] == "true")
      return recipient->addToLocal(action.second[0], action.second[1], true);
    else
      return recipient->addToLocal(action.second[0], action.second[1], false);
  }
  else {
    kDebug() << "Unknown action " << action.first;
  }
  return false;
}


bool Koushin::Action::possibleParametersGiven(Koushin::ActionObject* recipient, QString actionName, QStringList parameters)
{
  if(!recipient || !recipient->getPossibleActions().contains(actionName)) {
    kDebug() << "Action is not in list of possible action";
    return false;
  }
  Koushin::ActionProperties properties = recipient->getPossibleActions().value(actionName);
  if (parameters.length() != properties.parameterTypes.length()) {
    kDebug() << "Wrong parameter Number: Expected = " << properties.parameterTypes.length() << "; Given = " << parameters.length();
    return false;
  }
  ///This part is not necessary: in execution the error is caught
/*  for (int i = 0; i <= properties.parameterTypes.length(); ++i) {
    if (properties.parameterTypes.value(i) == "int") {
      try{
	manager->evalContent(parameters.value(i));
      }
      catch (std::exception& e) {
	kDebug() << "Parameter " << parameters.value(i) << " can not be calculated";
	return false;
      }
    }
  }*/
  return true;
}
