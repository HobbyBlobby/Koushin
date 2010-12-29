#ifndef KOUSHIN_EDITOR_H
#define KOUSHIN_EDITOR_H

#include <QObject>
#include "field.h"
#include <KConfigGroup>

class QListWidgetItem;

namespace Koushin_TownEditor {
  class Editor : public QObject {
    Q_OBJECT
    public:
      Editor();
      int m_fieldSize;
      KConfigGroup* m_config;
      QStringList m_fieldTypes;
    public Q_SLOTS:
      void fieldClicked(EditorField* field);
      void fieldTypeChanged(QListWidgetItem* item);
    private:
      QString m_type;
  };
}

#endif //KOUSHIN_EDITOR_H
