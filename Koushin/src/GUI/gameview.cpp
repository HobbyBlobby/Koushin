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


#include "gameview.h"
#include "resourceinfowidget.h"
#include "constructionmenu.h"
#include "townwidget.h"
#include <QtGui/QPushButton>
#include "buildinginfowidget.h"
#include <QtGui/QGraphicsView>
#include <QResizeEvent>
#include "player.h"

KoushinGUI::GameView::GameView()
  : QWidget() //parent is not planed, because this widget shoul be the central widget showing all other widgets
  , m_resourceInfo(new KoushinGUI::ResourceInfoWidget(this))
  , m_constructionMenu(new KoushinGUI::ConstructionMenu(QMap<QString, QString>(), this))
  , m_endRoundButton(new QPushButton("End Round",this))
  , m_fieldInfo(new KoushinGUI::BuildingInfoWidget())
  , m_townView(new QGraphicsView(new QGraphicsScene, this))
  , m_player(0)
  , m_townWidget(0)
{
//   m_resourceInfo->close();
  m_fieldInfo->setParent(this);
  m_constructionMenu->close();
  m_constructionMenu->setWindowFlags(Qt::Popup);
  m_fieldInfo->close();
  m_townView->close();
}

void KoushinGUI::GameView::resizeEvent(QResizeEvent* event)
{
  QRect widgetRect = geometry();
  QRect resourceInfoRect = QRect(widgetRect.left(), widgetRect.top(), widgetRect.width(), 50);
  QRect townRect = QRect(widgetRect.left(), resourceInfoRect.bottom(),
			 widgetRect.width(), widgetRect.height() - resourceInfoRect.height());
  QSize buttonHint = m_endRoundButton->minimumSizeHint();
  QRect buttonRect = QRect(resourceInfoRect.right() - buttonHint.width(), (resourceInfoRect.height() - buttonHint.height())/2,
			   buttonHint.width(), buttonHint.height());
  //adjust resource info widget:
  m_resourceInfo->setGeometry(resourceInfoRect);
//adjust town view:
  m_townView->setGeometry(townRect);
  m_townView->fitInView(m_townView->sceneRect(), Qt::KeepAspectRatio);
//adjust construction menu:
  m_constructionMenu->setPaintRange(QRect(mapToGlobal(townRect.topLeft()), mapToGlobal(townRect.bottomRight())));
//adjust end round button:
  m_endRoundButton->setGeometry(buttonRect);
//call original resive event of QWidget:
  QWidget::resizeEvent(event);
}

void KoushinGUI::GameView::showTownView(Koushin::Town* town)
{
  if(town) {
    if(m_townWidget) m_townView->scene()->removeItem(m_townWidget);
    m_townWidget = town->getTownWidget();
    m_townView->scene()->addItem(m_townWidget);
    m_townView->show();
  }
}

void KoushinGUI::GameView::changePlayer(Koushin::Player* player)
{
  if(!player) return; //avoid crash
///@todo disconnect events
  disconnect(m_player, SIGNAL(showConstructionMenu(Koushin::Town*,QPoint)));
  disconnect(m_player, SIGNAL(closeConstructionMenu()));
  disconnect(m_constructionMenu, SIGNAL(buildingChosen(QString)));
  disconnect(m_player, SIGNAL(showResourceInfo(Koushin::Town*)));
  disconnect(m_endRoundButton);
// change player:
  m_player = player;
///@todo connect with new player
  connect(m_player, SIGNAL(showConstructionMenu(Koushin::Town*, QPoint)), this, SLOT(showConstructionMenu(Koushin::Town*, QPoint)));
  connect(m_player, SIGNAL(closeConstructionMenu()), this, SLOT(closeConstructionMenu()));
  connect(m_constructionMenu, SIGNAL(buildingChosen(QString)), m_player, SLOT(buildingChosen(QString)));
  connect(m_player, SIGNAL(showResourceInfo(Koushin::Town*)), this, SLOT(showResourceInfo(Koushin::Town*)));
  connect(m_endRoundButton, SIGNAL(pressed()), m_player, SLOT(endRound()));
}

#include <KDebug>
void KoushinGUI::GameView::showConstructionMenu(Koushin::Town* town, QPoint point)
{
  if(town) {
    m_constructionMenu->setPossibleBuildings(town->getBuildingList());
    point = mapFieldToDisplay(point);
    m_constructionMenu->move(point.x(), point.y());
    m_constructionMenu->show();
  }
}

void KoushinGUI::GameView::closeConstructionMenu()
{
  m_constructionMenu->close();
}


void KoushinGUI::GameView::showFieldInfo(Koushin::Field* )
{

}

void KoushinGUI::GameView::showResourceInfo(Koushin::Town* town)
{
  m_resourceInfo->updateInfos(town->getResources().values());
  m_resourceInfo->show();
}

QPoint KoushinGUI::GameView::mapFieldToDisplay(QPoint fieldPos) const
{
  QPoint globalPoint = m_townView->mapFromScene(fieldPos);
  globalPoint = m_townView->mapToGlobal(globalPoint);
  return globalPoint;
}


#include "gameview.moc"