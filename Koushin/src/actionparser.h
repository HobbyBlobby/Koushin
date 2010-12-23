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
   * This class handles all string interpretation work. It also check, if the number of given parameter are possible and if the given action name is known. Therefor it use the \c getPossibleActions() function provided by the associated Action class.
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
      static QList<Action* > createActionsFromConfig(const KConfigGroup& tasksGroup, ActionObject* newOwner);
      static QPair< QString, QStringList > separateNameAndParameters(QString string);
      static QList<ActionObject* > parseRecipient(const QString& configLine, ActionObject* owner);
      static void addRequirementsToActions(QStringList conditionStrings, QMap<QString, Action* > actions);
      static bool parseGlobals(const KConfigGroup& parameterList, ActionManager* manager);
      static QList<Player* > findPlayers(QStringList parameters, ActionObject* owner);
      static QList<Town* > findTowns(QStringList parameters, ActionObject* owner, QList<Player* > players);
      static QList<Building* > findBuildings(QStringList parameters, ActionObject* owner, QList<Town* > towns);
  };
}

#endif // ACTIONPARSER_H
