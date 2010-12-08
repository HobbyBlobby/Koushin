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

#include "actionparser.h"
#include "citizenaction.h"

#include <QRegExp>
#include <QString>

Koushin::ActionParser::ActionParser()
  : m_action(0)
{

}

Koushin::ActionParser::~ActionParser()
{

}

#include <kdebug.h>
Koushin::Action* Koushin::ActionParser::parseConfig(const KConfigGroup& config)
{
  if (!config.hasKey("recipient")) {
    kDebug() << "Config for " << config.name() << " doesn't contain a recipient. Can't parse this entry";
    return 0;
  }
  QString recipient = config.readEntry("recipient", QString());
  if (recipient.contains(QRegExp("citizen[(.*)$]{0,1}"))) {
    kDebug() << recipient << " is citizen"; 
    QString actionName = config.readEntry("action", QString());
    m_action = new Koushin::CitizenAction(selectRandomCitizen(), actionName);
  } else {
    m_action = new CitizenAction;
  }
  return m_action;
}

Koushin::Citizen* Koushin::ActionParser::selectRandomCitizen()
{
  //currentPlayer->Towns->TownOwingThatBuilding->Population->ListOfCitizen->RandomCitizen
  //Temporary Solution: create a new citizen
  return (new Citizen(0));
}


// Koushin::Action* Koushin::ActionParser::addLine(QString line) {
//   
//   if (line.startsWith("use ")) {
//   } else if (line.startsWith("repeat ")) {
//     parseRepeatLine(line);
//   } else if (line.startsWith("do ")) {
//     parseTaskLine(line);
//   } else {
//     kDebug() << "Can't parse line: " << line;
//   }
//   return m_action;
// }
// 
// Koushin::Action* Koushin::ActionParser::parseTaskLine(QString line) {
//   line.remove(QRegExp("^do "));
//   kDebug() << line;
//   return m_action;
// }
// 
// Koushin::Action* Koushin::ActionParser::parseRepeatLine(QString line) {
//   line.remove(QRegExp("^repeat "));
//   return m_action;
// }
