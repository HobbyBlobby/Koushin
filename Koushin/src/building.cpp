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


#include "building.h"
#include <qpainter.h>
#include <KDebug>
#include <KConfigGroup>
#include <kconfig.h>
#include "actionparser.h"
#include <QMetaClassInfo>

Koushin::Building::Building(Town* town, KConfig* config)
  : m_town(town)
  , m_name("NoName")
  , m_level(1)
  , m_field(0)
  , m_config(config)
{
}

Koushin::Building::~Building()
{

}

const QMap< QString, Koushin::ActionProperties > Koushin::Building::getPossibleActions()
{
  QMap<QString, Koushin::ActionProperties> actions;
  for(int i = 0; i < metaObject()->methodCount(); ++i) {
    QPair<QString, QStringList> function = Koushin::ActionParser::separateNameAndParameters(metaObject()->method(i).signature());
    Koushin::ActionProperties prop(function.second, QString());
    actions.insert(function.first, prop);
  }
  adjustActionProperties(actions);
  kDebug() << actions.keys();
  return actions;
}

const QString Koushin::Building::getLocal(QString name, QString additionalContent)
{
  if(m_globalReplacements.contains(name))
    return m_globalReplacements.value(name);
  if(m_globalAdditions.contains(name))
    additionalContent += m_globalAdditions.value(name);
  return m_town->getLocal(name, additionalContent);
}

void Koushin::Building::addOpenFieldAction(QString groupName)
{
  m_openFieldActions << groupName;
  m_createdOpenFieldActions.insert(groupName, getNumberOfCreatedOpenFieldActions(groupName) + 1);
}

void Koushin::Building::removeOpenFieldAction(QString groupName)
{
  //remove only one: the same object is inserted more then one times to save memory
  m_openFieldActions.removeOne(groupName);
}

void Koushin::Building::select()
{
  m_field->markField(Qt::gray);
  foreach(Koushin::Field* field, m_usedFields)
    field->markField(Qt::gray);
}

void Koushin::Building::unselect()
{
  m_field->unmarkField();
  foreach(Koushin::Field* field, m_usedFields)
    field->unmarkField();
}

