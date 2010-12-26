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


#ifndef GAME_H
#define GAME_H

#include "QObject"
#include "player.h"

namespace Koushin {

  class Game : public QObject {
    Q_OBJECT
    public:
      Game();
      virtual ~Game();
      
      void addPlayer(QString name) {if(!playernameAlreadyExists(name)) m_players << new Player(name, this);}
      const QList<Player* > getPlayers() const {return m_players;}
      const Player* getCurrentPlayer() const {return m_currentPlayer;}
      const int getCurrentRound() const {return m_currentRound;}
      
      void endRound();
      void startRound();
      
    private:
      void shufflePlayers();
      bool playernameAlreadyExists(QString name);
      
      QList<Player* > m_players;
      Player* m_currentPlayer;
      int m_currentRound;
  };
}

#endif // GAME_H
