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

KoushinGUI::ConstructionMenu::ConstructionMenu(QMap<QString, QString> buildings, QWidget* parent)
  : QWidget(parent)
  , m_list(new QListWidget(this))
  , m_buildings(buildings)
  , m_okButton(new QPushButton("Construct selected building", this))
{
  m_okButton->setDisabled(true);
  foreach(QString name, buildings.keys()) {
    m_list->addItem(name);
  }
  
  connect(m_list, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(listitemSelected(QListWidgetItem*)));
  connect(m_okButton, SIGNAL(clicked(bool)), this, SLOT(okButtonClicked()));
}

KoushinGUI::ConstructionMenu::~ConstructionMenu()
{

}

void KoushinGUI::ConstructionMenu::resizeEvent(QResizeEvent* event)
{
  setBackgroundRole(QPalette::Window);
  QRect parentRect = parentWidget()->geometry();
  setGeometry(parentRect.width()/4, 0, parentRect.width()/2, parentRect.height()/2);
  QSize size = geometry().size();
  kDebug() << size;
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


#include "constructionmenu.moc"