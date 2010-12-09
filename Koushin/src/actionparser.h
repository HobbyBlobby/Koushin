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


#ifndef ACTIONPARSER_H
#define ACTIONPARSER_H

#include "action.h"
#include "actionmanager.h"
#include "actionowner.h"
#include "citizen.h"
#include "population.h"
#include "player.h"
#include "town.h"

#include <QMultiMap>
#include <QString>

#include <KConfigGroup>

namespace Koushin {
  class ActionParser
  {
    public:
      ActionParser(ActionOwner* owner, ActionManager* manager);
      virtual ~ActionParser();
      
      Action* parseConfig(const KConfigGroup& config);

  private:
      bool parseRecipient(const QString& configLine);
      bool findPlayer(const QString& parameter);
      bool findTown(const QString& parameter);
      
      Action* m_action;
      ActionManager* m_manager;
      Player* m_player;
      Town* m_town;
      ActionOwner* m_owner;
  };
}

#endif // ACTIONPARSER_H
