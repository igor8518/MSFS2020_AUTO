#include "ExQThread.h"

ExQThread::ExQThread(QString name, QObject* util) : QThread(), Name(name), utils(util)
{
  connect(this, SIGNAL(Log(QString)), utils, SLOT(AddLog(QString)));
  Log("Thread: " + Name + " create");
}

QString ExQThread::GetName()
{
  return Name;
}

void ExQThread::exit()
{
  Log("Thread: " + Name + " stop");
  QThread::exit();
}

void ExQThread::run() {
  Log("Thread: " + Name + " start");
  QThread::run();
	}
