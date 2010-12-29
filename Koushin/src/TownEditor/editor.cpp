#include "editor.h"
#include "field.h"
#include <KDebug>
#include <kconfig.h>
#include <QListWidget>
#include <QString>

Koushin_TownEditor::Editor::Editor()
  : m_fieldSize(1)
  , m_type("plainField")
{
}

void Koushin_TownEditor::Editor::fieldClicked(Koushin_TownEditor::EditorField* field)
{
  int x = field->boundingRect().topLeft().x() / m_fieldSize;
  int y = field->boundingRect().topLeft().y() / m_fieldSize;
  QString xString = QString("%1").arg(QString::number(x), 3, QLatin1Char('0'));
  QString yString = QString("%1").arg(QString::number(y), 3, QLatin1Char('0'));
  kDebug() << "Field clicked at " << xString << "," << yString;
  if(m_type == "plainField")
    field->setBrush(QBrush(Qt::white));
  if(m_type == "fieldWithForest")
    field->setBrush(QBrush(Qt::green));
  if(m_type == "fieldWithRocks")
    field->setBrush(QBrush(Qt::black));
  if(m_type == "fieldWithWater")
    field->setBrush(QBrush(Qt::blue));
  if(m_type == "fieldNotUsable")
    field->setBrush(QBrush(Qt::yellow));
  field->update(field->boundingRect());
  if(m_config) {
    foreach(QString groupName, m_fieldTypes) {
      if(groupName == m_type) {
	KConfigGroup group = m_config->group(groupName);
	group.writeEntry("field"+xString+yString, QString::number(x) + "," + QString::number(y));
	group.config()->sync();
      } else {
	KConfigGroup group = m_config->group(groupName);
	group.deleteEntry("field"+xString+yString);
	group.config()->sync();
      }
    }
  }
}

void Koushin_TownEditor::Editor::fieldTypeChanged(QListWidgetItem* item)
{
  m_type = item->text();
}


#include "editor.moc"