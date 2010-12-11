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


#ifndef PLAYER_H
#define PLAYER_H

#include <QList>
#include <QObject>
#include <QPoint>

namespace Koushin {
  class ActionManager;
  class Town;
  class Player : public QObject {
    Q_OBJECT
    public:
      Player();
      virtual ~Player();
      QList<Town* > getTowns() {return m_townList;}
      ActionManager* getActionManager() const {return m_actionManager;}
      void setActionManager(ActionManager* manager) {m_actionManager = manager;}
      void addTown(Town* town) {m_townList << town;}
      
    public Q_SLOTS:
      void townClicked(QPoint point);
    private:
      QList<Town* > m_townList;
      ActionManager* m_actionManager;
  };
}

#endif // PLAYER_H
