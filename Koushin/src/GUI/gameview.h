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


#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QtGui/qwidget.h>

class QGraphicsView;
class QPushButton;
namespace Koushin {
class Town;
class Field;
class Player;
}

namespace KoushinGUI {

class TownWidget;


class BuildingInfoWidget;


class ConstructionMenu;


class ResourceInfoWidget;

  class GameView : public QWidget {
    Q_OBJECT
    public:
      GameView();
    public Q_SLOTS:
      void showResourceInfo(Koushin::Town*);
      void showConstructionMenu(Koushin::Town*, QPoint);
      void closeConstructionMenu();
      void showFieldInfo(Koushin::Field* field);
      void showTownView(::Koushin::Town*);
      void changePlayer(::Koushin::Player*); //update the connections to the given player
    protected:
      virtual void resizeEvent(QResizeEvent* event);
      void mouseMoveEvent(QMouseEvent* event);
      void mousePressEvent(QMouseEvent* event);
      void mouseReleaseEvent(QMouseEvent* event);
    private:
      QPoint mapFieldToDisplay(QPoint fieldPos) const;
      
      ResourceInfoWidget* m_resourceInfo;
      ConstructionMenu* m_constructionMenu;
      QPushButton* m_endRoundButton;
      BuildingInfoWidget* m_fieldInfo;
      QGraphicsView* m_townView;
      QRectF m_focusRect;
      QPointF m_oldPos;
      //old properties: needed for deleting this objects
      ::Koushin::Player* m_player;
      TownWidget* m_townWidget;
  };
}

#endif // GAMEVIEW_H
