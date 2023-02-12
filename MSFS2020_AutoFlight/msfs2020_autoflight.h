#pragma once


#include "ui_msfs2020_autoflight.h"
#include "structs.h"

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
	void on_startButton();
  void AppendListView(QString s);
  void on_ButtonConnect();
  void on_PMDGButton();
  
  void ButtonModify(QPushButton* button, QString text, QString style);
  void ConnectButtonEnabled(bool enabled);
  void StartButtonEnabled(bool enabled);
  void realtimeDataSlot();
  void PlotCircle(double dist, double alt, double common);
  void PlotConstraints(std::vector<sWayPoint>* Legs, int startIndex, int endIndex, int currentIndex);
  void PlotPoints(std::vector<sWayPoint>* Legs, int startIndex, int endIndex);
  void PlotRealPath(double flyPoint, double planeAlt, double commonDistance);
  void SelectRow(int Row);
  void on_ChangeRow(QItemSelection Row, QItemSelection prev);
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
  void ChangeCurrentLegIndex(int currentLegIndex);
  void PMDGSend(DWORD val);
  void StartSend();
private:
  QObject* planesWork;
  QCPItemEllipse* pos;
  int GraphNums = 0;
  double AxisThrottle = 0;
  QCPGraph* Graphic;
  std::vector<QCPItemLine*>* dnArrow = new std::vector<QCPItemLine*>();
  std::vector<QCPItemLine*>* upArrow = new std::vector<QCPItemLine*>();
  std::vector<QCPItemLine*>* flightPoints = new std::vector<QCPItemLine*>();
  QTimer dataTimer;
  Ui::MSFS2020_AutoFlightClass* ui;  //Переделать в Q_PROPERTY
  QSharedPointer<QCPAxisTickerTime> timeTicker;

  std::string cb1Text = "";
  std::string cb2Text = "";
  std::string cb3Text = "";
};
