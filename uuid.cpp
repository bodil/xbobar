#include "uuid.h"

qulonglong counter;

namespace UUID {

  QString uuid() {
    return QString::number(++counter);
  }

};
