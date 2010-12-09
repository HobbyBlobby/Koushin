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


#include "citizen.h"

Koushin::Citizen::Citizen(int bornInRound)
  : m_condition(Koushin::citizenIsHealthy)
  , m_birthround(bornInRound)
{

}

Koushin::Citizen::~Citizen()
{

}

Koushin::citizenCondition Koushin::Citizen::citizenConditionFromQString(QString condition)
{
  if (condition == "citizenIsHungry") return Koushin::citizenIsHungry;
  else if (condition == "citizenIsIll") return Koushin::citizenIsIll;
  else if (condition == "citizenIsOld") return Koushin::citizenIsOld;
  else return Koushin::citizenIsHealthy;
}

