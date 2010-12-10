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

#include <KConfigGroup>

namespace Koushin {
  class Action;
  class ActionOwner;
  class Player;
  class Town;
  class ActionParser {
    public:
      ActionParser(ActionOwner* owner);
      virtual ~ActionParser();
      
      Action* parseConfig(const KConfigGroup& config);

  private:
      bool parseRecipient(const QString& configLine);
      bool parseAction(const QString& actionString);
      bool findPlayer(const QString& parameter);
      bool findTown(const QString& parameter);
      
      QPair<QString, QStringList > separateNameAndParameters(QString string);
      bool possibleParametersGiven(QString actionName, QStringList parameters);
      
      Action* m_action;
      Player* m_player;
      Town* m_town;
      ActionOwner* m_owner;
  };
}

#endif // ACTIONPARSER_H
