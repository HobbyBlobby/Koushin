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


#include "constructionmenu.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QResizeEvent>
#include <QPushButton>
#include <KDebug>
#include <qgraphicsscene.h>
#include <QGraphicsView>

KoushinGUI::ConstructionMenu::ConstructionMenu(QMap<QString, QString> buildings, QWidget* parent)
  : QWidget(parent)
  , m_list(new QListWidget(this))
  , m_okButton(new QPushButton("Construct selected building", this))
  , m_paintRange(QRect())
{
  m_okButton->setDisabled(true);
  setPossibleBuildings(buildings);
  
  connect(m_list, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(listitemSelected(QListWidgetItem*)));
  connect(m_okButton, SIGNAL(clicked(bool)), this, SLOT(okButtonClicked()));
}

KoushinGUI::ConstructionMenu::~ConstructionMenu()
{

}

void KoushinGUI::ConstructionMenu::setPossibleBuildings(QMap< QString, QString > possibleBuildings)
{
  m_buildings = possibleBuildings;
  m_list->clear();
  foreach(QString name, possibleBuildings.keys()) {
    m_list->addItem(name);
  }
}

void KoushinGUI::ConstructionMenu::resizeEvent(QResizeEvent* event)
{
  QSize size = event->size();
  QSize buttonSize = m_okButton->minimumSizeHint();
  m_okButton->setGeometry((size.width() - buttonSize.width())/2, size.height()*3/4, buttonSize.width(), buttonSize.height());;
  m_list->resize(size.width(), size.height()*3/4);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QWidget::resizeEvent(event);
}

void KoushinGUI::ConstructionMenu::listitemSelected(QListWidgetItem* item)
{
  if(item->text() != "")
    m_okButton->setEnabled(true);
}

void KoushinGUI::ConstructionMenu::okButtonClicked()
{
  emit buildingChosen(m_buildings.value(m_list->selectedItems().first()->text()));
}

void KoushinGUI::ConstructionMenu::setPaintRange(QRect rect)
{
  m_paintRange = rect;
}

void KoushinGUI::ConstructionMenu::showEvent(QShowEvent* event)
{
  QRect newRect = QRect(pos(), QSize(180,120));
  if(newRect.right() > m_paintRange.right()) {
    newRect.moveRight(m_paintRange.right());
  }
  if(newRect.bottom() > m_paintRange.bottom()) {
    newRect.moveBottom(m_paintRange.bottom());
  }
  setGeometry(newRect);
  QWidget::showEvent(event);
}


#include "constructionmenu.moc"