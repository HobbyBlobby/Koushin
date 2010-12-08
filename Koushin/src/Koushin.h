#ifndef Koushin_H
#define Koushin_H

#include "QObject"

namespace Koushin {
  class Koushin : public QObject
  {
  Q_OBJECT
  public:
      Koushin();
      virtual ~Koushin();
  };
}

#endif // Koushin_H
