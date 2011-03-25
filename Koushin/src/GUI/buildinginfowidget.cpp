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


#include "buildinginfowidget.h"
#include <building.h>
#include <KConfigGroup>
#include <KDebug>
#include <QPushButton>

KoushinGUI::BuildingInfoWidget::BuildingInfoWidget()
  : m_building(0)
  , m_list(new QListWidget(this))
  , m_nextLevelButton(new QPushButton("Level up", this))
{
  m_list.setSelectionMode(QAbstractItemView::SingleSelection);
  connect(&m_list, SIGNAL(itemActivated(QListWidgetItem*)), this, SIGNAL(fieldActionSelected(QListWidgetItem*)));
  m_nextLevelButton->hide();
}

void KoushinGUI::BuildingInfoWidget::repaint()
{
  QSize buttonSize = m_nextLevelButton->minimumSizeHint(); 
  m_list.setGeometry(0, 0, width(), height() - buttonSize.height());
  m_nextLevelButton->setGeometry(0, height() - buttonSize.height(), buttonSize.width(), buttonSize.height());
  kDebug() << "Repaint of building info";

  if(!m_building || m_building->getAge() < 0) {
    m_list.clear();
    m_nextLevelButton->hide();
    return;
  }
  QStringList itemStrings;
  foreach(QString groupName, m_building->getOpenFieldActions())
    if(!itemStrings.contains(groupName)) itemStrings << groupName;
  m_list.clear();
  m_list.addItems(itemStrings);
  m_list.show();
  m_nextLevelButton->show();
}

#include "buildinginfowidget.moc"
