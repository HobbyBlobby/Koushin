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


#include "townaction.h"
#include "town.h"

Koushin::TownAction::TownAction(Town* recipient)
  : m_recipient(recipient)
{

}

Koushin::TownAction::~TownAction()
{

}

QMap< QString, Koushin::ActionProperties> Koushin::TownAction::getPossibleActions()
{
  QMap<QString, Koushin::ActionProperties> actions;
  actions.insert("increaseResource", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Town: Add the given parameter to the given resource. string=ResourceName, int=difference"));
  actions.insert("decreaseResource", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Town: Removes the given parameter from the given resource. string=ResourceName, int=difference"));
  actions.insert("setResourceCapacity", Koushin::ActionProperties(
    QStringList() << "string" << "int",
    "Town: Sets the capacity of a given resource to the given value. string=ResourceName, int=new value"));
  return actions;
}

void Koushin::TownAction::execute()
{
  if(m_action == "increaseResource") {
    Koushin::ResourceType type = Koushin::Town::getResourceTypeFromQString(m_parameters[0]);
    int diff = m_parameters.value(1, QString("0")).toInt();
    m_recipient->changeResource(type, diff);
  }
  if(m_action == "decreaseResource") {
    Koushin::ResourceType type = Koushin::Town::getResourceTypeFromQString(m_parameters[0]);
    int diff = m_parameters.value(1, QString("0")).toInt();
    m_recipient->changeResource(type, -diff);
  }
  if(m_action == "setResourceCapacity") {
    Koushin::ResourceType type = Koushin::Town::getResourceTypeFromQString(m_parameters[0]);
    int value = m_parameters.value(1, QString("-1")).toInt();
    if(value == -1) value = m_recipient->getResources().value(type)->maximumCapacity;
    m_recipient->setResourceCapacity(type, value);
  }
}

