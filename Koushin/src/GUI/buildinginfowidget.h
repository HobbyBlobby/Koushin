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


#ifndef BUILDINGINFOWIDGET_H
#define BUILDINGINFOWIDGET_H

#include <QWidget>
#include <QListWidget>

class QPushButton;
namespace Koushin {
class Building;
}

namespace KoushinGUI {
  class BuildingInfoWidget : public QWidget {
    Q_OBJECT
    public:
      BuildingInfoWidget();
      
      void setBuilding(::Koushin::Building* building) {m_building = building;}
      ::Koushin::Building* getBuilding() const {return m_building;}
      QPushButton* getNextLevelButton() const {return m_nextLevelButton;}
    public Q_SLOTS:
      void repaint();
    Q_SIGNALS:
      void fieldActionSelected(QListWidgetItem* item);
      
    private:
      ::Koushin::Building* m_building;
      QListWidget m_list;
      QPushButton* m_nextLevelButton;
  };
}

#endif // BUILDINGINFOWIDGET_H
