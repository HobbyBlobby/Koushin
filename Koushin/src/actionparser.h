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
    License along with this library; if not, write to
    the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef ACTIONPARSER_H
#define ACTIONPARSER_H

#include <QMultiMap>
#include <QPair>
#include <QString>
#include <QList>

#include <KConfigGroup>

class KConfig;
namespace Koushin {
  class Building;
  class Player;
  class ActionObject;
  class ActionManager;
  class Action;
  class ActionOwner;
  class Player;
  class Town;
  /**
   * @class ActionParser
   * @brief This class interprets the tasks configuration and creates a appropriate Action.
   * 
   * This class handles all string interpretation work. It is pure static. So nothing is stored here.
   * 
   * The configuration should contain a group with:
   * \code
   * [tasks][globals]
   * VarName=10
   * Var2=100-#VarName#
   * 
   * [tasks][conditions]
   * myCondition=task1->task2
   * 
   * [tasks][task1]
   * recipient=player(current)
   * action=setGlobalTo(VarName, 50)
   * priority=20
   * 
   * [tasks][task2]
   * recipient=town()
   * action=doSomething(para1, #Var2#)
   * 
   * \endcode
   * In this example two actions are defined: \c task1 and \c task2. \c task2 use the \b global \c Var2 to calculate his second parameter. \c Var2 itself contains the global \c VarName. At parsing time the value of \c Var2 is <tt>100-10=90</tt>. 
   * 
   * The condition \c myConditioin means, that \c task2 is only executed, if \c task1 succeeded. But \c task2 overwrite the global with the value \c 50. So at run time \c task2 is called with <tt>Var2 = 100-50=50</tt>, not with \c 90.
   * 
   **/
  class ActionParser {
    public:
      /**
       * @brief This function creats all Action objects given in the "tasks" configuration group.
       * This function also adds all conditions to the Action. So the Action is ready to use.
       *
       * @param tasksGroup This configuration group should contain all actions, so pass the whole "tasks" group to this function.
       * 
       * @param newOwner The owner must be ActionObject and is neseccary to find the propper recipient.
       * 
       * @param currentRound The round the Action is created.
       * 
       * @return QList<Action* > The list containing all actions from the configuration.
       **/
      static QList<Action* > createActionsFromConfig(const KConfigGroup& tasksGroup, ActionObject* newOwner, int currentRound);
      /**
       * @brief This function finds the recipients.
       * It is possible to have multiple recipients, e.g. all town containing a port.
       *
       * @param configLine The configuration line discribing the recipient/s.
       * 
       * @param owner Because the owner is not stored in this static class you have to give the owner again, to find the appropriate recipient.
       * 
       * @return QList<ActionObject* > The list of all recipients. This list can be empty.
       **/
      static QList<ActionObject* > parseRecipient(const QString& configLine, ActionObject* owner);
      /**
       * @brief This function just adds all given parameters to the manager.
       * @todo remove this function, because here is nothing to parse
       *
       * @param parameterList List of all parameters.
       * 
       * @param manager The manager who stores the parameters.
       * 
       * @return bool Returns true when function succeeded (everytime true).
       **/
      static bool parseGlobals(const KConfigGroup& parameterList, ActionManager* manager);
      /**
       * @brief This function tries to find the wanted player for the recipient.
       * \n If the owner is a Building the following parameters are knwon:
       * - current (standard) -- the player constructed the Building
       *
       * @todo reorder the test: first check the owner type, then check the parameter.
       * 
       * @param parameters The list of the parameters to choose the player.
       * 
       * @param owner For some parameters it is nessecary to know the owner (like for "current").
       * 
       * @return QList<:Player* > All players fitting in the given parameters.
       **/
      static QList<Player* > findPlayers(QStringList parameters, ActionObject* owner);
      /**
       * @brief This function tries to find the wanted town for the recipient.
       * \n If the owner is a Building the following parameters are knwon:
       * - current (standard) -- the town the Building belongs to
       *
       * @todo reorder the test: first check the owner type, then check the parameter.
       * 
       * @param parameters The list of the parameters to choose the town.
       * 
       * @param owner For some parameters it is nessecary to know the owner (like for "current").
       * 
       * @return QList<:Town* > All towns fitting in the given parameters.
       **/
      static QList<Town* > findTowns(QStringList parameters, ActionObject* owner, QList<Player* > players);
      /**
       * @brief This function tries to find the wanted building for the recipient.
       * \n If the owner is a Building the following parameters are knwon:
       * - current (standard) -- the building itself
       *
       * @todo reorder the test: first check the owner type, then check the parameter.
       * 
       * @param parameters The list of the parameters to choose the building.
       * 
       * @param owner For some parameters it is nessecary to know the owner (like for "current").
       * 
       * @return QList<:Player* > All buildings fitting in the given parameters.
       **/
      static QList<Building* > findBuildings(QStringList parameters, ActionObject* owner, QList<Town* > towns);
      /**
       * @brief This functions separates a function name and his parameter.
       *
       * @param string The line to separate in the form "functionName(para1, para2,...)".
       * 
       * @return QPair< QString, QStringList > The name a all parameters as list.
       **/
      static QPair< QString, QStringList > separateNameAndParameters(QString string);
      /**
       * @brief This function adds all requirements to the actions.
       *
       * @param conditionStrings The list of all conditions.
       * 
       * @param actions All actions as a pair of name and pointer.
       * 
       * @return void
       **/
      static void addRequirementsToActions(QStringList conditionStrings, QMap<QString, Action* > actions);
      /**
       * @brief This function reads the round limits and prepares the Action.
       *
       * @param action The configuration of this Action will be changes.
       * 
       * @param config From this configuration group the parameters are read.
       * 
       * @param currentRount the current round to calculate the right round limitations
       * 
       * @return void
       **/
      static void setRoundLimit(Action* action, KConfigGroup config, int currentRound);
      /**
       * @brief This function creates a list of open field actions.
       * The actions are read from the \c fieldtasks group. If the building level is zero, the start number of free fields is used, else the number resulting from <tt>(int)(level*newFieldsPerLevel+startNumber)-stillCreatedActions</tt> is used.
       * 
       * This function also sets the right number of the created actions for the building and adds the open field action to the list of the building.
       *
       * @param config The whole configuration for the building. The whole configuration is nessecary, because the number of actiones can be found in the \c general group.
       * 
       * @return void
       **/
      static void createOpenFieldActions(KConfigGroup* config, Koushin::Building* building);
  };
}

#endif // ACTIONPARSER_H
