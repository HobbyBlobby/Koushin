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
      /**
       * @brief Constructor
       *
       * @param owner The owner is user to parse the recipient. Only with the owner it is possible to find the "current" player or town. Possible owners are in the moment only \class Building s.
       **/
      ActionParser(ActionOwner* owner);
      virtual ~ActionParser();
      
      /**
       * @brief This function organizes the main work.
       *Organizing the main work means, that this function read the appropriate keys from the KConfigGroup \c config and gives it to the correct functions. It is necessary to keep the right order of parsing. For example it is not possible to parse the action before the recipient, because only with a recipient the Action can be created.
       * 
       * @param config This KConfigGroup should contain another group called \b tasks. 
       * 
       * @return :Action* > The returned Action contains all parsed configurations given by \c config.
       **/
      QList<Action* > parseConfig(const KConfigGroup& config);

  private:
    /**
     * @brief This function finds the right recipient for the Action. 
     * Using the owner it is possible to find the correct recipient. Depending on the owner type the result varies. For example for a player as owner type (e.g. for magic rituals [not implemented yet]) the string \c{town(current)} does not make any sense.
     *
     * @param configLine The relevant line from the configuration file.
     * 
     * @return bool The status of parsing the recipient. \c false means, it is not possible to find a reasonable recipient.
     **/
    bool parseRecipient(const QString& configLine);
    /**
     * @brief This function parses the action name.
     * The string is divided into his name and his parameters. This function also tests, if the name is in the list of possible actions (provided by the corresponding Action class) and if the number of parameters is correct.
     * \todo move the priority into an extra function, and insert call into \c parseConfig().
     *
     * @param actionString The name line from the configuration file containing the action name.
     * 
     * @param priority The priority of the tasks. The standard value given by the \c parseConfig() function is 10. A higher priority causes a earlier execution. \b Warning: the execution order is influenced by the conditions.
     * 
     * @return bool The status of parsing. \c false means, it is not possible to parse action (e.g. the parameter number is given or the action name is unknown).
     **/
    bool parseAction(const QString& actionString, int priority);
    /**
     * @brief The function parses the global variables.
     * The global variables (here called \b globals) provide a system to influence other actions. Before you use globals, it is necessary to register the globals in the ActionManager (provided by the \c owner). This function parses the configuration groups called \c globals, given in the configuration file like here:
     * \code
     * [tasks][globals]
     * name=5
     * otherName=10+#name#
     * \endcode
     *To use later the globals you have to enclose the global within two hash keys, like <tt>\#name\#</tt>.
     * 
     * @param parameterList The list contains the whole <tt>[globals]</tt> group. The left site of the equals sign becomes the name of the global, the right site is stored as content. The content can also use other globals.
     * 
     * @return bool The status of parsing. There is nothing to check, so it will always return \c true.
     **/
    bool parseGlobals(const KConfigGroup& parameterList);
    /**
     * @brief This function searches a reasonable player.
     * The behaviour of this function depends on the given owner type. In that moment only the Building is implemented as ActionOwner. For this type the player will result from the the owner of the Town the Building belongs to.
     * Later it should by possible to influence other players (e.q. with a spell or technology).
     *
     * @param parameter As parameter you can give special commands to influence the finding process. Now only a empty string or \c current is known (both do the same).
     * 
     * @return bool The status of parsing. Of it is possible to find a player then the function return \c true, else \c false.
     **/
    bool findPlayer(const QString& parameter);
    /**
     * @brief This function searches a reasonable town.
     * Later it should by possible to influence other players (e.q. with a spell or technology).
     *
     * @param parameter As parameter you can give special commands to influence the finding process. Now only a empty string or \c current is known (both do the same).
     * 
     * @return bool The status of parsing. Of it is possible to find a player then the function return \c true, else \c false.
     **/
    bool findTown(const QString& parameter);
    /**
     * @brief This functions parses the conditions for actions.
     * It is possible to add conditions to actions. Theses conditions can influence the order of executing actions. Independent from the priority the actions waits until the requirements are executed. You can decide if the actions is executed if the requirement succeeded of failed.
     *
     * @param conditionStrings The list contains all lines of the configurations group <tt>[conditions]</tt>. The left side of the equals sign is used as name for the conditions (just for the developer) and the right site contains the condition.
     * 
     * @param actions The QMap contains pairs of action name and a pointer to the Action object. With this map it is easier to find Actions by name.
     * 
     * @return void
     **/
    void addRequirementsToActions(QStringList conditionStrings, QMap<QString, Action* > actions);
      
    /**
     * @brief This functions separates a function name and his parameters.
     *
     * @param string This string normally has the form <tt>functionName(para1, para2)<tt>.
     * 
     * @return QPair< QString, QStringList > The QPair contains the function name and a list of all parameters.
     **/
    QPair<QString, QStringList > separateNameAndParameters(QString string);
    /**
     * @brief This functions checks if the given parameter are possible.
     * Now this function checks only if the number of parameters are like expected and the function is defined.
     * \todo try to evaluate the parameter
     * 
     *
     * @param actionName With this action name the ActionProperties are received. These properties contain the number and type of parameter expected.
     * 
     * @param parameters The parameters which should be used to call the action \c actionName.
     * 
     * @return bool If the parameter are possible the function return \c true, else \c false.
     **/
    bool possibleParametersGiven(QString actionName, QStringList parameters);
      
    Action* m_action;
      Player* m_player;
      Town* m_town;
      ActionOwner* m_owner;
  };
}

#endif // ACTIONPARSER_H
