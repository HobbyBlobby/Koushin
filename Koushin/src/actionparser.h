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
#include "citizen.h"
#include "population.h"

#include <QMultiMap>
#include <QString>

#include <KConfigGroup>

namespace Koushin {
  class ActionParser
  {
    public:
      ActionParser();
      virtual ~ActionParser();
      
      Action* parseConfig(const KConfigGroup& config);
//       void setAction(Action* action) {m_action = action;}
//       Action* addLine(QString line);
//       Action* parseRepeatLine(QString line);
//       Action* parseTaskLine(QString line);
    private:
      Citizen* selectRandomCitizen();
      
      Action* m_action;
      QMultiMap<int, Action* > m_actions;
      QMultiMap<int, QString > m_taskLines;
      QMultiMap<int, QString > m_repeatLines;
  };
}

#endif // ACTIONPARSER_H
