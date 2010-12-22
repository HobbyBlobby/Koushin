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
#include <KConfigGroup>
#include <kconfig.h>
#include "actionparser.h"

namespace Koushin {
  class Town;
  class Player;
  class Building;
  class Town;
  class ActionManager;
  class ActionObject;

  enum actionStatus {
    actionNotStarted = 0,
    actionSucceeded,
    actionFailed,
    actionWaitsForRequirement
  };
  
  class Action {
    public:
      Action();
      virtual ~Action();
            
      actionStatus getStatus() const {return m_status;}
      int getPriority() const {return m_config->readEntry("priority", QString(10)).toInt();}
      QMap<Action*, bool> getRequirements() const {return m_requirements;}
      QMap<Action*, bool> getDependencies() const {return m_requirementFor;}
      QList<Action* > getRecursiveRequirements(Action* action);
 
      /**
       * @brief This functions reads the action name from the configuration.
       *The configuration must contain a key called \c action that holds a value like <tt>functionName(para1, para2,...)</tt>. To separate the function name from the parameters it uses the static \c separateNameAndParameters from the ActionParser.
       * 
       * @return QString The name of the action
       **/
      QString getActionString() const {return ActionParser::separateNameAndParameters(m_config->readEntry("action", QString())).first;}
      QStringList getParameters() const {return ActionParser::separateNameAndParameters(m_config->readEntry("action", QString())).second;}
      
      /**
       * @brief This function executes the specified action.
       * At first it checks, if all necessary parameters are given. Then it uses the parser to create the right objects to the parameter. Finally it calls the belonging execution action.
       * 
       * Checking the status and the requirements is part of the ActionManager.
       *
       * @return bool The execution status. If all parameters are correct and the sub-execution gives \c true this functions returns \c true, else \c false.
       **/
      bool execute(bool failIfOneRecipientFail = false);
      /**
       * @brief Use this function to store the configuration.
       *
       * @param config The configuration part of the <tt>[tasks][taskname]</tt> group.
       * @return void
       **/
      void setConfiguration(KConfigGroup* config) {m_config = config;}
      void setOwner(ActionObject* owner) {m_owner = owner;}
      ActionObject* getOwner() {return m_owner;}
      void setStatus(actionStatus status) {m_status = status;}
      bool addRequirement(Action* action, bool positiv = true);
      
      void closeRequirement(Action* action) {m_openRequirements.removeAll(action);}
      bool noOpenRequirements() const {return m_openRequirements.isEmpty();}

      bool requirementsFinished();
      void resetAction();
    protected:
      KConfigGroup* m_config;
      actionStatus m_status;
      ActionObject* m_owner; //need this information to parse recipient using "current"
      QMap<Action*, bool > m_requirements; ///keep this system -> action pointers can not change in runtime!
      QMap<Action*, bool > m_requirementFor;
      QList<Action* > m_openRequirements;
      
    private:
      bool executePlayerAction(Player* recipient, const QPair<QString, QStringList>& action);
      bool executeTownAction(Town* recipient, const QPair<QString, QStringList>& action);
      bool executeBuildingAction(Building* recipient, const QPair<QString, QStringList>& action);
      bool executeActionObjectAction(ActionObject* recipient, const QPair<QString, QStringList>& action);
      bool setAsRequirementFor(Action* action, bool positiv = true);
      bool possibleParametersGiven(ActionObject* recipient, QString actionName, QStringList parameters);
  };
}


#endif // ACTION_H
