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

#include <KDebug>
class NonInteractiveView : public QGraphicsView {
public:
  NonInteractiveView(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent) {}
protected:
  void mousePressEvent(QMouseEvent* event) {event->ignore();}
  void mouseMoveEvent(QMouseEvent* event) {event->ignore();}
  void mouseReleaseEvent(QMouseEvent* event) {event->ignore();}
};

KoushinGUI::GameView::GameView()
  : QWidget() //parent is not planed, because this widget shoul be the central widget showing all other widgets
  , m_resourceInfo(new KoushinGUI::ResourceInfoWidget(this))
  , m_constructionMenu(new KoushinGUI::ConstructionMenu(QMap<QString, QString>(), this))
  , m_endRoundButton(new QPushButton("End Round",this))
  , m_fieldInfo(new KoushinGUI::BuildingInfoWidget())
  , m_townView(new NonInteractiveView(new QGraphicsScene(this), this))
  , m_player(0)
  , m_townWidget(0)
  , m_focusRect(3, 3, 6, 6)
{
//   m_resourceInfo->close();
  m_fieldInfo->setParent(this);
  m_constructionMenu->close();
  m_constructionMenu->setWindowFlags(Qt::Popup);
//   m_fieldInfo->close();
  m_townView->close();
}

void KoushinGUI::GameView::resizeEvent(QResizeEvent* event)
{
  QRect widgetRect = QRect(QPoint(0,0), event->size());
  QRect resourceInfoRect = QRect(widgetRect.left(), widgetRect.top(), widgetRect.width() * 2.0/3.0, 50);
  QRect fieldInfoRect = QRect(resourceInfoRect.right(), widgetRect.top(),
			      widgetRect.width() - resourceInfoRect.width(), resourceInfoRect.height());
  QRect townRect = QRect(widgetRect.left(), resourceInfoRect.bottom(),
			 widgetRect.width(), widgetRect.height() - resourceInfoRect.height());
  QSize buttonHint = m_endRoundButton->minimumSizeHint();
  QRect buttonRect = QRect(resourceInfoRect.right() - buttonHint.width(), (resourceInfoRect.height() - buttonHint.height())/2,
			   buttonHint.width(), buttonHint.height());
//adjust resource info widget:
  m_resourceInfo->setGeometry(resourceInfoRect);
//adjust field info widget:
  m_fieldInfo->setGeometry(fieldInfoRect);
//adjust town view:
//   int zoom = 150; //per cent
//   QRect zoomRect(0, 0, m_townView->sceneRect().width()*100/zoom, m_townView->sceneRect().height()*100/zoom);
  m_townView->setGeometry(townRect);
  m_townView->fitInView(m_focusRect, Qt::KeepAspectRatio);
//adjust construction menu:
  m_constructionMenu->setPaintRange(QRect(mapToGlobal(townRect.topLeft()), mapToGlobal(townRect.bottomRight())));
//adjust end round button:
  m_endRoundButton->setGeometry(buttonRect);
//call original resive event of QWidget:
  QWidget::resizeEvent(event);
}

void KoushinGUI::GameView::changePlayer(Koushin::Player* player)
{
  if(!player) return; //avoid crash
///@todo disconnect events
  if(m_player) {
    disconnect(m_player, SIGNAL(showConstructionMenu(Koushin::Town*,QPoint)));
    disconnect(m_player, SIGNAL(closeConstructionMenu()));
    disconnect(m_constructionMenu, SIGNAL(buildingChosen(QString)));
    disconnect(m_player, SIGNAL(showResourceInfo(Koushin::Town*)));
    disconnect(m_endRoundButton);
    disconnect(m_player, SIGNAL(showFieldInfo(Koushin::Field*)));
    disconnect(m_fieldInfo);
  }
// change player:
  m_player = player;
///@todo connect with new player
  connect(m_player, SIGNAL(showConstructionMenu(Koushin::Town*, QPoint)), this, SLOT(showConstructionMenu(Koushin::Town*, QPoint)));
  connect(m_player, SIGNAL(closeConstructionMenu()), this, SLOT(closeConstructionMenu()));
  connect(m_constructionMenu, SIGNAL(buildingChosen(QString)), m_player, SLOT(buildingChosen(QString)));
  connect(m_player, SIGNAL(showResourceInfo(Koushin::Town*)), this, SLOT(showResourceInfo(Koushin::Town*)));
  connect(m_endRoundButton, SIGNAL(pressed()), m_player, SLOT(endRound()));
  connect(m_player, SIGNAL(showFieldInfo(Koushin::Field*)), this, SLOT(showFieldInfo(Koushin::Field*)));
  connect(m_fieldInfo, SIGNAL(fieldActionSelected(QListWidgetItem*)), m_player, SLOT(fieldActionSelected(QListWidgetItem*)));
  
  m_player->getTowns().first()->getTownWidget()->setParent(this);
}

void KoushinGUI::GameView::showTownView(Koushin::Town* town)
{
  kDebug() << "Draw town";
  if(town) {
    if(m_townWidget) m_townView->scene()->removeItem(m_townWidget);
    m_townWidget = town->getTownWidget();
    m_townView->scene()->addItem(m_townWidget);
    m_townView->show();
  }
}

#include <KDebug>
#include <field.h>
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


void KoushinGUI::GameView::showFieldInfo(Koushin::Field* field)
{
  if(field->getBuilding()) {
    m_fieldInfo->setBuilding(field->getBuilding());
    m_fieldInfo->repaint();
  }
  m_fieldInfo->show();
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

void KoushinGUI::GameView::mouseMoveEvent(QMouseEvent* event)
{
  qreal scale = m_townView->geometry().width() / m_focusRect.width();
  QPointF shift = QPointF(event->posF() - m_oldPos) / scale;
  QPointF centerPoint = m_focusRect.center() - shift;
//   QRectF newFocus(m_focusRect.left()-shift.x(), m_focusRect.top()-shift.y(), m_focusRect.width(), m_focusRect.height());
//   if((newFocus.left() < 0) || (newFocus.right() > m_townView->sceneRect().right()))
//     m_oldPos.setX(event->posF().x());
//   if(newFocus.right() > m_townView->sceneRect().right())
//     newFocus.moveRight(m_townView->sceneRect().right());
//   if((newFocus.top() < 0) || (newFocus.bottom() > m_townView->sceneRect().bottom()))
//     m_oldPos.setY(event->posF().y());
//   if(newFocus.bottom() > m_townView->sceneRect().bottom())
//     newFocus.moveBottom(m_townView->sceneRect().bottom());
//   kDebug() << "Move: " << m_townView->geometry() << " und " << m_townView->sceneRect() << " = " << shift << " mit " << scale;
    m_townView->centerOn(centerPoint);
    if((centerPoint.x() - m_focusRect.width()/2 < 0) || (centerPoint.x() + m_focusRect.width()/2 > m_focusRect.width())) {
      m_oldPos.setX(m_oldPos.x() + shift.x());
    }
    if((centerPoint.y() - m_focusRect.height()/2 < 0) || (centerPoint.y() + m_focusRect.height()/2 > m_focusRect.height())) {
      m_oldPos.setY(m_oldPos.y() + shift.y());
    }
}

void KoushinGUI::GameView::mousePressEvent(QMouseEvent* event)
{
  m_oldPos = event->posF();
  //adjust focus rect, because with Qt::KeepAspectRation the rect is different from wanted rect
  m_focusRect = m_townView->mapToScene(m_townView->viewport()->geometry()).boundingRect();
}
void KoushinGUI::GameView::mouseReleaseEvent(QMouseEvent* event)
{
  qreal scale = m_townView->geometry().width() / m_focusRect.width();
  if(m_oldPos == event->posF()) {
    QPoint townPoint = m_townView->mapToScene(QPointF(event->posF() + pos() - m_townView->pos()).toPoint()).toPoint();
    m_player->fieldClicked(m_player->getTowns().first()->getFieldFromPoint(townPoint));
  } else {
    m_focusRect = m_townView->mapToScene(m_townView->viewport()->geometry()).boundingRect();
  }
}


#include "gameview.moc"