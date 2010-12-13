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
#include "player.h"
#include <kdebug.h>
#include "town.h"
#include "GUI/townwidget.h"
#include "GUI/constructionmenu.h"
#include <QGraphicsScene>
#include <KConfigGroup>
#include <kconfig.h>
#include "actionparser.h"
#include "actionmanager.h"

Koushin::Player::Player()
  : m_actionManager(0)
  , m_buildingLot(QPoint(0,0))
  , m_constructionMenu(0)
{

}

Koushin::Player::~Player()
{

}

void Koushin::Player::townClicked(QPoint point) //create member with active town
{
  if(!m_townList.isEmpty() && !m_constructionMenu) {
    m_buildingLot = point;
    m_constructionMenu = new KoushinGUI::ConstructionMenu(m_townList.first()->getPossibleBuildings(m_listOfAllBuildings));
    QRect sceneRect = m_townList.first()->getTownWidget()->scene()->sceneRect().toRect();
    m_constructionMenu->setGeometry(sceneRect.width()/4, 0, sceneRect.width()/2, sceneRect.height()*3/4);
    m_townList.first()->getTownWidget()->scene()->addWidget(m_constructionMenu);
    connect(m_constructionMenu, SIGNAL(buildingChosen(QString)), this, SLOT(buildingChosen(QString)));
  }
}

void Koushin::Player::buildingChosen(QString buildingConfig)
{
  if(!m_actionManager) {
    kDebug() << "No ActionManger set. Can't create Building without a manager.";
    return; 
  }
  Koushin::Building* newBuilding = new Koushin::Building(m_townList.first());
  KConfig config(buildingConfig);
  KConfigGroup tasksGroup(&config, "tasks");
  QStringList taskConfigList = tasksGroup.groupList();
  for (QStringList::const_iterator it = taskConfigList.begin(); it != taskConfigList.end(); ++it) {
    KConfigGroup task(&config, *it);
    Koushin::ActionParser parser(newBuilding);
    Koushin::Action* action = parser.parseConfig(task);
    m_actionManager->addAction(action);
  }
  m_townList.first()->addBuilding(newBuilding, m_buildingLot);
  m_townList.first()->getTownWidget()->drawBuildings(m_townList.first()->getBuildings());

//   m_townList.first()->getTownWidget()->scene()->(m_constructionMenu);
//   delete(m_constructionMenu);
  //TODO: find a way to delete the widget
  m_constructionMenu = 0;
}

#include "player.moc"