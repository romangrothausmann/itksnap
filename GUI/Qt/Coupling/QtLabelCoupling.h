#ifndef QTLABELCOUPLING_H
#define QTLABELCOUPLING_H

#include "QtWidgetCoupling.h"
#include <iostream>
#include <iomanip>
#include <QLabel>

template <class TAtomic>
class DefaultWidgetValueTraits<TAtomic, QLabel>
    : public WidgetValueTraitsBase<TAtomic, QLabel *>
{
public:

  virtual TAtomic GetValue(QLabel *w)
  {
    std::istringstream iss(w->text().toStdString());
    TAtomic value;
    iss >> value;
    return value;
  }

  virtual void SetValue(QLabel *w, const TAtomic &value)
  {
    std::ostringstream oss;
    oss << value;
    w->setText(oss.str().c_str());
  }

  virtual void SetValueToNull(QLabel *w)
  {
    w->setText("");
  }

  virtual const char *GetSignal()
  {
    return NULL;
  }
};


#endif // QTLABELCOUPLING_H
