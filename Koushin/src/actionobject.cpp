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


#include "actionobject.h"

Koushin::ActionObject::ActionObject()
{

}


bool Koushin::ActionObject::addToLocal(QString name, QString content, bool replaceGlobal)
{
  if(replaceGlobal)
    m_globalReplacements.insert(name, m_globalReplacements.value(name) + content);
  else
    m_globalAdditions.insert(name, m_globalAdditions.value(name) + content);
  return true;
}

bool Koushin::ActionObject::setLocalTo(QString name, QString content, bool replaceGlobal)
{
  if(replaceGlobal)
    m_globalReplacements.insert(name, content);
  else
    m_globalAdditions.insert(name, content);
  return true;
}

const QMap< QString, Koushin::ActionProperties > Koushin::ActionObject::getPossibleActions()
{
  QMap<QString, Koushin::ActionProperties> actions;
  actions.insert("setLocalTo", Koushin::ActionProperties(
    QStringList() << "string" << "string" << "bool",
    "ActionObject: overwrites a value only for specified ActionObject. string=NameOfGlobal, string=NewValue, bool=the locals replaces the global "));
  actions.insert("addToLocal", Koushin::ActionProperties(
    QStringList() << "string" << "string" << "bool",
    "ActionObject: add a string to a local. string=NameOfGlobal, string=AdditionalContent, bool=the locals replaces the global"));
  return actions;
}

