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


#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QMultiMap>
#include <QString>

namespace Koushin {
  class Action;
  class Player;
  class ActionManager {
    public:
      ActionManager(Player* owner);
      virtual ~ActionManager();
      
      void addAction(Action* action);
      void addAction(QList<Action* > actions);
      void executeActions();
      bool executeAction(Action* action);
      void setStatusOfDependensies(Action* failedAction);
      Player* getOwner() {return m_owner;}
      bool addGlobalParameter(QString name, QString content);
      QString getGlobalParameterValue(QString name) const {return m_globalParameters.value(name);}
      int evalParameter(QString parameter);
      int evalContent(QString content, QString parameter = "NOT_IN_LIST");
    private:
      QString expandParameter( QString line, QString name);
      
      QMultiMap<int, Action* > m_actions;
      Player* m_owner;
      QMap<QString, QString> m_globalParameters; //map with name and content
  };
}

#endif // ACTIONMANAGER_H
