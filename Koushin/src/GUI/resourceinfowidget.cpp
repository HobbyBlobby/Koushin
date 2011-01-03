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


#include "resourceinfowidget.h"
#include <QLabel>
#include <KDebug>
#include <QResizeEvent>

KoushinGUI::ResourceInfoWidget::ResourceInfoWidget(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , m_label(new QLabel("Resources:\tWood: 0\tStone: 0\tRice: 0", this))
{
//   setGeometry(0,0,m_label->minimumSizeHint().width(), m_label->minimumSizeHint().height());
}

KoushinGUI::ResourceInfoWidget::~ResourceInfoWidget()
{

}

void KoushinGUI::ResourceInfoWidget::updateInfos(QList< Koushin::Resource* > resources)
{
  QString newText("Resources:\n");
  foreach(Koushin::Resource* res, resources) {
    if(res->type == Koushin::ResourceWood)
      newText += "Wood: " + QString("%1").arg(res->amount) + "\t";
    if(res->type == Koushin::ResourceStone)
      newText += "Stone: " + QString("%1").arg(res->amount) + "\t";
    if(res->type == Koushin::ResourceRice)
      newText += "Rice: " + QString("%1").arg(res->amount) + "\t";
  }
  m_label->setText(newText);
//   m_label->update();
//   updateGeometry();
//   setGeometry(0,0,m_label->sizeHint().width(), m_label->sizeHint().height());
}

void KoushinGUI::ResourceInfoWidget::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);
}

