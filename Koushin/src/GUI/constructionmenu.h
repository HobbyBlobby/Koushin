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


#ifndef CONSTRUCTIONMENU_H
#define CONSTRUCTIONMENU_H

#include <QMap>
#include <QString>
#include <QWidget>
#include <QList>
#include <qevent.h>

class QListWidgetItem;
class QListWidget;
class QResizeEvent;
class QPushButton;
namespace KoushinGUI {
  class ConstructionMenu : public QWidget {
    Q_OBJECT
    public:
      ConstructionMenu(QMap<QString, QString> possibleBuildings, QWidget* parent = 0);
      virtual ~ConstructionMenu();
      void setPossibleBuildings(QMap<QString, QString> possibleBuildings);
      void setPaintRange(QRect rect);
    protected:
      void resizeEvent(QResizeEvent* event);
      void showEvent(QShowEvent* event);
      
    Q_SIGNALS:
      void buildingChosen(QString buildingConfig);
    public Q_SLOTS:
      void listitemSelected(QListWidgetItem* item);
      void okButtonClicked();
    private:
      QListWidget* m_list;
      QMap<QString, QString> m_buildings; //this map contains building name and the config file (/absolutePath/fileName.config)
      QPushButton* m_okButton;
      QRect m_paintRange;
  };
}

#endif // CONSTRUCTIONMENU_H
