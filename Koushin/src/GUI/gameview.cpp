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
  , m_endRoundButton(new QPushButton(this))
  , m_fieldInfo(new KoushinGUI::BuildingInfoWidget())
  , m_townView(new QGraphicsView(new QGraphicsScene, this))
  , m_player(0)
  , m_townWidget(0)
{
  m_resourceInfo->close();
  m_fieldInfo->setParent(this);
  m_constructionMenu->close();
  m_constructionMenu->setWindowFlags(Qt::Popup);
  m_fieldInfo->close();
  m_townView->close();
}

void KoushinGUI::GameView::resizeEvent(QResizeEvent* event)
{
  QRect widgetRect = geometry();
//adjust town view:
  m_townView->setGeometry(widgetRect);
  m_townView->fitInView(m_townView->sceneRect(), Qt::KeepAspectRatio);
//adjust construction menu:
  m_constructionMenu->setGeometry(0, 0, width()/2, height()/2); //find out, why this do not call resizeEvent()
//   m_constructionMenu->resizeEvent(new QResizeEvent(QSize(width()/2, height()/2), oldSize));
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
  m_player = player;
///@todo connect with new player
  connect(m_player, SIGNAL(showConstructionMenu(Koushin::Town*, QPoint)), this, SLOT(showConstructionMenu(Koushin::Town*, QPoint)));
}

#include <KDebug>
void KoushinGUI::GameView::showConstructionMenu(Koushin::Town* town, QPoint point)
{
  if(town) { ///@todo later use point to show menu near the cursor
    m_constructionMenu->setPossibleBuildings(town->getBuildingList());
    m_constructionMenu->move(point.x(), point.y());
    kDebug() << "show constructon menu at " << m_constructionMenu->geometry();
//     m_townView->close();
    m_constructionMenu->show();
  }
}

void KoushinGUI::GameView::showFieldInfo(Koushin::Field* )
{

}

void KoushinGUI::GameView::showResourceInfo(Koushin::Town* )
{

}

#include "gameview.moc"