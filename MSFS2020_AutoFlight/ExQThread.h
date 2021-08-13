#pragma once
#include <qthread.h>
#include "headers.h"
class ExQThread : public QThread {
  Q_OBJECT
public: 
  ExQThread(QString name = "", QObject* util = NULL);
  QString GetName();
  void exit();
private:
  QString Name;
  QObject* utils;
signals:
  void Log(QString s);
protected:
  virtual void run();
};

