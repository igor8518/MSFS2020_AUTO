#pragma once

#include <QMainWindow>
#include "ui_msfs2020_autoflight.h"
#include <QTIMER>
#include <QTIME>
#include <QTHREAD>
#include "qcustomplot.h"
//#include "mainlogic.h"

class MSFS2020_AutoFlight : public QMainWindow
{
  Q_OBJECT
public:
  explicit MSFS2020_AutoFlight(QObject* planesWork, QWidget *parent = Q_NULLPTR);
  //QObject* planes = NULL;
  //MainLogic* mainLogic = NULL;
  ~MSFS2020_AutoFlight();
  Ui::MSFS2020_AutoFlightClass* GetUi();
  QThread* maimThread;
private slots:
  //void SlotRangeChanged(const QCPRange &newRange);
  void AppendListView(QString s);
  void on_ButtonConnect();
  void ButtonModify(QPushButton* button, QString text, QString style);
  void realtimeDataSlot();
  void on_sb1P(double val);
  void on_sb1I(double val);
  void on_sb1D(double val);
  void on_cc1(bool state);
  void on_cb1curr(QString text);
  void on_cb2curr(QString text);
  void on_cb3curr(QString text);
  void on_vs1(int pos);
signals:
  void Connect();
private:
  QObject* planesWork;
  
  double AxisThrottle = 0;
  QCPGraph* Graphic;
  QTimer dataTimer;
  Ui::MSFS2020_AutoFlightClass* ui;  //Переделать в Q_PROPERTY
  QSharedPointer<QCPAxisTickerTime> timeTicker;

  std::string cb1Text = "";
  std::string cb2Text = "";
  std::string cb3Text = "";
};
