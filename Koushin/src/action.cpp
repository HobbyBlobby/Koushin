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
#include "actionobject.h"
#include "town.h"
#include "actionmanager.h"
#include "player.h"
#include "building.h"
#include "field.h"
#include <QMetaClassInfo>

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
{
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

    //assaign these values when you use the new QMetaMethod call:
    QStringList parameterTypes;
    Koushin::ActionManager* manager = 0;
    //asseignment changes with recipients, so use this switch:
    switch(recipient->getActionObjectType()) {
      case Koushin::actionObjectIsBuilding:
	parameterTypes = ((Koushin::Building*)recipient)->getPossibleActions().value(action.first).parameterTypes;
	manager = ((Koushin::Building*)recipient)->getTown()->getOwner()->getActionManager();
	break;
      case Koushin::actionObjectIsTown:
	parameterTypes = ((Koushin::Town*)recipient)->getPossibleActions().value(action.first).parameterTypes;
	manager = ((Koushin::Town*)recipient)->getOwner()->getActionManager();
	break;
      case Koushin::actionObjectIsPlayer:
	parameterTypes = ((Koushin::Player*)recipient)->getPossibleActions().value(action.first).parameterTypes;
	manager = ((Koushin::Player*)recipient)->getActionManager();
	break;
      case Koushin::actionObjectIsField:
	parameterTypes = ((Koushin::Field*)recipient)->getPossibleActions().value(action.first).parameterTypes;
	manager = ((Koushin::Field*)recipient)->getTown()->getOwner()->getActionManager();
	break;
      default:
	oneRecipientFailed = true;
    } // switch(recipient->getActionObjectType())
    if(manager) {
      //find right method:
      int index = recipient->metaObject()->indexOfMethod(QString(action.first + "(" + parameterTypes.join(",") + ")").toLatin1());
      QMetaMethod method = recipient->metaObject()->method(index);
      //create QGenericArguments from strings:
      QList<QGenericArgument> args;
      for(int i = 0; i < 10; ++i) {
	QString type = parameterTypes.value(i);
	if(type.isEmpty())
	  args.insert(i, QGenericArgument());
	else if (type == "ResourceType") {
	  kDebug() << "Insert ResourceType = " << action.second.value(i);
	  args.insert(i, Q_ARG(Koushin::ResourceType, Koushin::Town::getResourceTypeFromQString(action.second.value(i))));
	}
	else if (type == "int") {
	  try{
	    int value = manager->evalContent(action.second.value(i));
	    kDebug() << "Insert int = " << value;
	    args.insert(i, Q_ARG(int, value));
	  }
	  catch (std::exception & e) {
	    kDebug() << "Can not calculate " << action.second.value(i) << ". Reason: " << e.what();
	    args.insert(i, QGenericArgument());
	  }
	}
	else if(type == "QString") {
	  kDebug() << "Insert QString = " << action.second.value(i);
	  args.insert(i, Q_ARG(QString, *(new QString(action.second.value(i)))));
	  kDebug() << args.at(i).data();
	}
	else {
	  kDebug() << "Unknown parameter type: " << type;
	  args.insert(i, QGenericArgument());
	}
      }
      //execute the functions:
      bool rtnValue = false;
      method.invoke(recipient, Qt::DirectConnection, Q_RETURN_ARG(bool, rtnValue),
		    args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);
      if(rtnValue)
	allRecipientsFailed = false;
      else
	oneRecipientFailed = true;
    } // if(manager)
  } // loop over recipients
  if(failIfOneRecipientFailed && oneRecipientFailed) return false;
  if(allRecipientsFailed) return false;
  return true;
}

bool Koushin::Action::possibleParametersGiven(Koushin::ActionObject* recipient, QString actionName, QStringList parameters)
{
  QMap<QString, Koushin::ActionProperties> possibleActions;
  if(recipient->getActionObjectType() == Koushin::actionObjectIsBuilding)
    possibleActions = Koushin::Building().getPossibleActions();
  if(recipient->getActionObjectType() == Koushin::actionObjectIsField)
    possibleActions = Koushin::Field().getPossibleActions();
  if(recipient->getActionObjectType() == Koushin::actionObjectIsTown)
    possibleActions = Koushin::Town().getPossibleActions();
  if(recipient->getActionObjectType() == Koushin::actionObjectIsPlayer)
    possibleActions = Koushin::Player().getPossibleActions();

  
  if(!recipient || !possibleActions.contains(actionName)) {
    kDebug() << "Action is not in list of possible actions: " << actionName;
    return false;
  }
  Koushin::ActionProperties properties = possibleActions.value(actionName);
  if (parameters.length() != properties.parameterTypes.length()) {
    kDebug() << "Wrong parameter Number: Expected = " << properties.parameterTypes.length() << "; Given = " << parameters.length();
    return false;
  }
  return true;
}
