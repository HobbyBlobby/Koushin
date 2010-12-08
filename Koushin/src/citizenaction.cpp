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


#include "citizenaction.h"

Koushin::CitizenAction::CitizenAction(Koushin::Citizen* citizen, QString action)
  : Koushin::Action()
  , m_citizen(citizen)
{
  m_action = action;
}

Koushin::CitizenAction::~CitizenAction()
{

}

#include <kdebug.h>
void Koushin::CitizenAction::execute()
{
   if (m_action == "addCondition") {
      Koushin::citizenCondition* parameter;
      if (!m_parameters.isEmpty() && (parameter = (Koushin::citizenCondition*)m_parameters.first()))
	m_citizen->addCondition(*parameter);
    } else if (m_action == "removeCondition") {
      Koushin::citizenCondition* parameter;
      if (!m_parameters.isEmpty() && (parameter = (Koushin::citizenCondition*)m_parameters.first()))
	m_citizen->removeCondition(*parameter);
    } else {
      kDebug() << "Undefined action called!";
  }
}
