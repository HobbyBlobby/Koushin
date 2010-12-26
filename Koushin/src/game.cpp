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

#include "game.h"
#include "player.h"
#include "actionmanager.h"

Koushin::Game::Game()
  : m_currentPlayer(0)
  , m_currentRound(0)
{
}

Koushin::Game::~Game()
{}

void Koushin::Game::startRound()
{
  if(m_players.isEmpty()) return;
  if(m_currentPlayer != m_players.last())
    m_currentPlayer = m_players.value(m_players.indexOf(m_currentPlayer) + 1);
  else
    m_currentPlayer = m_players.first();
  ++m_currentRound;
  m_currentPlayer->getActionManager()->executeActions(m_currentRound);
  m_currentPlayer->getResourceInfoWidget()->updateInfos(m_currentPlayer->getTowns().first()->getResources().values());
}

void Koushin::Game::endRound()
{
  ///@todo prepare requests for other players
  startRound();
}

bool Koushin::Game::playernameAlreadyExists(QString name)
{
  foreach(::Koushin::Player* player, m_players)
    if(player->getName() == name) return true;
  return false;
}


void Koushin::Game::shufflePlayers()
{
  QList<Player* > tmpList;
  for(int i = m_players.length() - 1; i >= 0; --i)
    tmpList << m_players.takeAt(qrand() % i);
  m_players = tmpList;
}

#include "game.moc"
