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

#ifndef ACTION_H
#define ACTION_H

#include <QList>
#include <QString>

namespace Koushin {
  class Action
  {
    public:
      Action();
      virtual ~Action();
      virtual void execute() = 0;
      
      void addParameter(void* para) {m_parameters << para;}
      void addParameters(QList<void* > paras) {m_parameters << paras;}
    protected:
      QList<void* > m_parameters;
      QString m_action;
  };
}
#endif // ACTION_H
