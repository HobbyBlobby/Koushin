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
  enum actionObjectType{
    actionObjectNotSpecified = 0,
    actionObjectIsBuiling,
    actionObjectIsTown,
    actionObjectIsPlayer,
    actionObjectNumber
  };

  class ActionProperties {
    public:
      ActionProperties(QStringList parameterTypes = QStringList(), QString description = "")
	: parameterTypes(parameterTypes)
	, description(description) {}
      QStringList parameterTypes;
      QString description;
  };

  class ActionObject
  {
    public:
      ActionObject();
//       virtual ~ActionObject();
      
      virtual const actionObjectType getActionObjectType() = 0; // each class using this class has to return his type
      static const QMap<QString, ActionProperties> getPossibleActions();
      
      bool setLocalTo(QString name, QString content, bool replaceGlobal = false);
      bool addToLocal(QString name, QString content, bool replaceGlobal = false);
      virtual const QString getLocal(QString name, QString additionalContent = QString()) = 0;
      
    protected:
      QMap<QString, QString> m_globalAdditions;
      QMap<QString, QString> m_globalReplacements;
  };
}
#endif // ACTIONOBJECT_H
