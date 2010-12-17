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

#ifndef ACTION_H
#define ACTION_H

#include <QMap>
#include <QString>
#include <QList>
#include <QStringList>
#include <kdebug.h>

// this macro i will use later, but now I makes more Problems then solutions
#define PUBLIC_ACTION getFunctionName(__FUNCTION__)
#define COMMA ,
#define DEFINE_PUBLIC_ACTION(name, paramTypes, params)			\
  class name: public FunctionBase { public:				\
    name(Action* action) : m_action(action) {}				\
    bool executeFunction(paramTypes) {return m_action->name(params);}	\
    using FunctionBase::executeFunction;				\
    Action* m_action;							\
  };									

namespace Koushin {
  enum actionType {
    noActionType = 0,
    actionForPlayer,
    actionForTown,
    actionForCitizen
  };

  enum actionStatus {
    actionNotStarted = 0,
    actionSucceeded,
    actionFailed,
    actionWaitsForRequirement
  };
  
  class ActionProperties {
    public:
      ActionProperties(QStringList parameterTypes = QStringList(), QString description = "")
	: parameterTypes(parameterTypes)
	, description(description) {}
      QStringList parameterTypes;
      QString description;
  };

#ifndef FUNCTION_BASE
#define FUNCTION_BASE
  class FunctionBase {
    public:
      virtual bool executeFunction() = 0;
      virtual ~FunctionBase() = 0;
  };
//   FunctionBase::~FunctionBase() {}
#endif // FUNCTION_BASE

  class Action {
    public:
      Action();
      virtual ~Action();
      virtual bool execute() = 0;
      virtual actionType getActionType() = 0;
      QMap<QString, ActionProperties> getPossibleActions();
            
      void addAction(QString actionName) {m_action = actionName;}
      void addParameter(QString para) {m_parameters << para;}
      void addParameters(QStringList paras) {m_parameters << paras;}
      bool addRequirement(Action* action, bool positiv = true);
      bool setAsRequirementFor(Action* action, bool positiv = true);
      actionStatus getStatus() const {return m_status;}
      void setPriority(int prio) {m_priority = prio;}
      int getPriority() const {return m_priority;}
      void setStatus(actionStatus status) {m_status = status;}
      QMap<Action*, bool> getRequirements() const {return m_requirements;}
      QMap<Action*, bool> getDependencies() const {return m_requirementFor;}
      QString getActionString() const {return m_action;}
      QStringList getParameters() const {return m_parameters;}
      QList<Action* > getRecursiveRequirements(Action* action);
      void closeRequirement(Action* action) {m_openRequirements.removeAll(action);}
      bool noOpenRequirements() const {return m_openRequirements.isEmpty();}
      
      bool requirementsFinished();
      void resetAction();
    protected:
      QStringList m_parameters;
      QString m_action;
      int m_priority;
      QMap<Action*, bool > m_requirements;
      QMap<Action*, bool > m_requirementFor;
      actionStatus m_status;
      QList<Action* > m_openRequirements;
  };
//   DEFINE_PUBLIC_ACTION(getFunctionName,QString action COMMA int a,action COMMA a)
}


#endif // ACTION_H
