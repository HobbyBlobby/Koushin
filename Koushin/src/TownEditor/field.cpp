#include "field.h"
#include "editor.h"
#include <KDebug>

Koushin_TownEditor::EditorField::EditorField(int x, int y, int width, int height)
  : QGraphicsRectItem(x,y,width,height)
  , m_editor(0)
  , m_fieldType("plainField")
{
}


void Koushin_TownEditor::EditorField::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if(m_editor)
    m_editor->fieldClicked(this);
  QGraphicsRectItem::mousePressEvent(event);
}

QString Koushin_TownEditor::EditorField::fieldTypeToQString(Koushin_TownEditor::FieldType type)
{
  if(type == Koushin_TownEditor::plainField)
    return "plainField";
  if(type == Koushin_TownEditor::fieldWithBuilding)
    return "fieldWithBuilding";
  if(type == Koushin_TownEditor::fieldWithForest)
    return "fieldWithForest";
  if(type == Koushin_TownEditor::fieldWithRocks)
    return "fieldWithRocks";
  if(type == Koushin_TownEditor::fieldWithWater)
    return "fieldWithWater";
  if(type == Koushin_TownEditor::fieldNotUsable)
    return "fieldNotUsable";
  return "";
}

Koushin_TownEditor::FieldType Koushin_TownEditor::EditorField::QStringToFieldType(QString string)
{
  kDebug() << string;
  if(string == "fieldWithBuilding")
    return Koushin_TownEditor::fieldWithBuilding;
  if(string == "fieldWithForest")
    return Koushin_TownEditor::fieldWithForest;
  if(string == "fieldWithRocks")
    return Koushin_TownEditor::fieldWithRocks;
  if(string == "fieldWithWater")
    return Koushin_TownEditor::fieldWithWater;
  if(string == "fieldNotUsable")
    return Koushin_TownEditor::fieldNotUsable;
  return Koushin_TownEditor::plainField;
}
