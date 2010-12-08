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

#include <QDebug>

#ifndef CITIZEN_H
#define CITIZEN_H

namespace Koushin {
  enum citizenCondition {
    citizenIsHealthy = 0x0,
    citizenIsOld = 0x1,
    citizenIsIll = 0x2,
    citizenIsHungry = 0x4
  };
  Q_DECLARE_FLAGS(citizenConditions, citizenCondition)
  
  class Citizen
  {
    public:
      Citizen(int bornInRound);
      virtual ~Citizen();
      
      citizenConditions addCondition(citizenCondition condition) {return m_condition |= condition;}
      citizenConditions removeCondition(citizenCondition condition) {return m_condition ^= condition;}
      citizenConditions getCondition() {return m_condition;}
    private:
      citizenConditions m_condition;
      int m_birthround;
  };
}

#endif // CITIZEN_H
