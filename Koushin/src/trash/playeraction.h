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


#ifndef PLAYERACTION_H
#define PLAYERACTION_H

#include "action.h"

#include <QMap>
#include <QString>

namespace Koushin {
  class Player;
  class PlayerAction : public Action {
    public:
      PlayerAction(Player* recipient);
      virtual ~PlayerAction();
      static QMap<QString, ActionProperties> getPossibleActions();
      actionType getActionType() {return Koushin::actionForPlayer;}
      bool execute();
    private:  
      Player* m_recipient;
      QList<Player* > m_recipients;
  };
}
#endif // PLAYERACTION_H
