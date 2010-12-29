#ifndef KOUSHIN_TOWNEDITOR_FIELD_H
#define KOUSHIN_TOWNEDITOR_FIELD_H

#include <qgraphicsitem.h>
#include <KDebug>

namespace Koushin_TownEditor {
  enum FieldType {
    plainField = 0,
    fieldWithForest,
    fieldWithRocks,
    fieldWithWater,
    fieldWithBuilding,
    fieldNotUsable,
    numberOfFieldTypes
  };

  class Editor;
  class EditorField : public QGraphicsRectItem {
    public:
      EditorField(int x, int y, int width, int height);
      Editor* m_editor;
      QString m_fieldType;
      void mousePressEvent(QGraphicsSceneMouseEvent* event);
      static QString fieldTypeToQString(FieldType type);
      static FieldType QStringToFieldType(QString string);
  };
}

#endif //KOUSHIN_TOWNEDITOR_FIELD_H