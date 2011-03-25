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


#ifndef ACTIONOBJECT_H
#define ACTIONOBJECT_H
#include <QString>
#include <QStringList>
#include <QMap>

/**
 * @brief This class provides like a QObject the possibility to receive or send actions.
 * This class can not used direct.
 **/
namespace Koushin {

  class Action;
  /**
   * @brief This enum provides the information about the type of the derived class.
   * Often the base class ActionObject is stored or returned. Before a cast to the derived class test the type. Each derived class should have his onw entry in this enum, so do not forget to add it here.
   **/
  enum actionObjectType{
    actionObjectNotSpecified = 0,
    actionObjectIsBuilding,
    actionObjectIsTown,
    actionObjectIsPlayer,
    actionObjectIsField,
    actionObjectNumber
  };

  /**
   * @brief A little helper class to store informations about a function for an Action.
   **/
  class ActionProperties {
    public:
      ActionProperties(QStringList parameterTypes = QStringList(), QString description = "")
	: parameterTypes(parameterTypes)
	, description(description)
	, activated(false) {}
      void activate(QString desc) {
	activated = true;
	description = desc;
      }
      QStringList parameterTypes;
      QString description;
      bool activated;
  };

  /**
   * @brief The basis class for all objects receiving of sending an Action.
   * To derive from this class you have to implmenent the following functions:
   * @code
   * virtual const actionObjectType getActionObjectType()
   * virtual const QString getLocal(QString name, QString additionalContent = QString())
   * @endcode
   * The the documentations belonging to this functions for details.
   **/
  class ActionObject : public QObject {
    Q_OBJECT
    public:
      ActionObject();
      
      /**
       * @brief This function helps to find out the derived class.
       *
       * @return actionObjectType Each derived class should have a entry in the enum actionObjectType.
       **/
      virtual const actionObjectType getActionObjectType() = 0; // each class using this class has to return his type
      /**
       * @brief The Action can only be executed if this function provides the function name and the right parameters.
       *
       * @return QMap<QString, ActionProperties > The QMap contains the name and the corresponding properties.
       **/
      static const QMap<QString, ActionProperties> getPossibleActions();
      
      QList<Action* > getAllActions();
      bool insertAction(Action* action);
      bool deleteAction(Action* action);
      
      /**
       * @brief This function replaces a local with a new value.
       * Each derived class can have his own local parameter (here locals). Every local can either be added the the global or can replace the global.
       *
       * @param name The name of the local.
       * 
       * @param content The new content for the local.
       * 
       * @param replaceGlobal Decide, if the local replaces the global (true) or adds content to a global (false). Default is false.
       * 
       * @return bool Like every action this function has to return a bool, indicating the succes of the execution.
       **/
      bool setLocalTo(QString name, QString content, bool replaceGlobal = false);
      /**
       * @brief This function add content to a local.
       * Each derived class can have his own local parameter (here locals). Every local can either be added the the global or can replace the global.
       *
       * @param name The name of the local.
       * 
       * @param content The additional content for the local.
       * 
       * @param replaceGlobal Decide, if the local replaces the global (true) or adds content to a global (false). Default is false.
       * 
       * @return bool Like every action this function has to return a bool, indicating the succes of the execution.
       **/
      bool addToLocal(QString name, QString content, bool replaceGlobal = false);
      /**
       * @brief Returns the local parameter.
       * This final function should decide, if the parameter a global replacement or an additional content. If the parameter is an additional content add the content from the next "heigher" class (e.g. Town is the next heigher class for Building). As example the implementation from the building:
       * @code
       *   if(m_globalReplacements.contains(name))
       *     return m_globalReplacements.value(name);
       *   if(m_globalAdditions.contains(name))
       *     additionalContent += m_globalAdditions.value(name);
       *   return m_town->getLocal(name, additionalContent);
       * @endcode
       *
       * @param name The name of the local.
       * 
       * @param additionalContent The optional additional content from a lower class. Default is QString().
       * 
       * @return const QString The content of the local.
       **/
      virtual const QString getLocal(QString name, QString additionalContent = QString()) = 0;
      QMap< QString, ActionProperties > adjustActionProperties(QMap< QString, Koushin::ActionProperties >& actions);
      
    protected:
      QMap<QString, QString> m_globalAdditions;
      QMap<QString, QString> m_globalReplacements;
      QList<Action* > m_actions;
  };
}
#endif // ACTIONOBJECT_H
