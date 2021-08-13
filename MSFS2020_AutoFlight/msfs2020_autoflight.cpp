#include "msfs2020_autoflight.h"

MSFS2020_AutoFlight::MSFS2020_AutoFlight(QObject* planesWork, QWidget* parent) : QMainWindow(parent), ui(new Ui::MSFS2020_AutoFlightClass)
{
  ui->setupUi(this);
  this->planesWork = planesWork;

  int propsCount = planesWork->metaObject()->propertyCount();
  ui->cb1->addItem("");
  ui->cb2->addItem("");
  ui->cb3->addItem("");
  for (int i = 1; i < propsCount; i++) {
    const char* name = planesWork->metaObject()->property(i).name();
    if (planesWork->property(("source" + std::string(name)).c_str()).isValid() &&
      planesWork->property(("min" + std::string(name)).c_str()).isValid() &&
      planesWork->property(("max" + std::string(name)).c_str()).isValid()) {
      ui->cb1->addItem(planesWork->metaObject()->property(i).name());
    }
    ui->cb2->addItem(planesWork->metaObject()->property(i).name());
    ui->cb3->addItem(planesWork->metaObject()->property(i).name());
  }
  //ui->cb1->addItem(this->props.);
  ui->Graph->addGraph(); // blue line
  ui->Graph->graph(0)->setPen(QPen(QColor(40, 110, 255)));
  ui->Graph->addGraph(); // red line
  ui->Graph->graph(1)->setPen(QPen(QColor(255, 110, 40)));
  ui->Graph->addGraph(); // green line
  ui->Graph->graph(2)->setPen(QPen(QColor(110, 255, 40)));

  //QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
  timeTicker = QSharedPointer<QCPAxisTickerTime>::create();
  timeTicker->setTimeFormat("%h:%m:%s");
  ui->Graph->xAxis->setTicker(timeTicker);
  ui->Graph->axisRect()->setupFullAxesBox();
  ui->Graph->yAxis->setRange(-16383, 16383);

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(ui->Graph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Graph->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->Graph->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Graph->yAxis2, SLOT(setRange(QCPRange)));


  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
  //ui->Graph->graph(0)->adaptiveSampling();
  //ui->Graph->graph(1)->adaptiveSampling();
}
void MSFS2020_AutoFlight::realtimeDataSlot() {
  static QTime time(QTime::currentTime());
  // calculate two new data points:
  double key = time.elapsed() / 1000.0; // time elapsed since start of demo, in seconds
  static double lastPointKey = 0;
  if (key - lastPointKey > 0.02) // at most add point every 2 ms
  {
    // add data to lines:
    ui->vs1->setMinimum(-3277);
    ui->vs1->setMaximum(16383);
    if (!ui->cc1->checkState()) {
      if (planesWork->property(cb1Text.c_str()).isValid()) {
        double sp = planesWork->property(cb1Text.c_str()).toDouble();
        ui->vs1->setSliderPosition(sp);
      }
    }
    std::string ss = ui->cb1->itemText(ui->cb1->currentIndex()).toStdString();
    //const char* cb1text = ss.c_str();
    if (planesWork->property(cb1Text.c_str()).isValid()) {
      ui->Graph->graph(0)->addData(key, planesWork->property(cb1Text.c_str()).toDouble());
    }
    if (planesWork->property(cb2Text.c_str()).isValid()) {
      ui->Graph->graph(1)->addData(key, planesWork->property(cb2Text.c_str()).toDouble());
    }
    if (planesWork->property(cb3Text.c_str()).isValid()) {
      ui->Graph->graph(2)->addData(key, planesWork->property(cb3Text.c_str()).toDouble());
    }
    //ui->Graph->graph(0)->addData(key, qSin(key) + qrand() / (double)RAND_MAX * 16383 * qSin(key / 0.3843));
    //ui->Graph->graph(1)->addData(key, qCos(key) + qrand() / (double)RAND_MAX * 8191 * qSin(key / 0.4364));
    

    // rescale value (vertical) axis to fit the current data:
    //ui->customPlot->graph(0)->rescaleValueAxis();
    //ui->customPlot->graph(1)->rescaleValueAxis(true);
    lastPointKey = key;
  }
  // make key axis range scroll with the data (at a constant range size of 8):
  ui->Graph->xAxis->setRange(key, 8, Qt::AlignRight);
  ui->Graph->replot();

  // calculate frames per second:
  static double lastFpsKey;
  static int frameCount;
  ++frameCount;
  if (key - lastFpsKey > 2) // average fps over 2 seconds
  {
    ui->statusBar->showMessage(
      QString("%1 FPS, Total Data points: %2")
      .arg(frameCount / (key - lastFpsKey), 0, 'f', 0)
      .arg(ui->Graph->graph(0)->data()->size() + ui->Graph->graph(1)->data()->size())
      , 0);
    lastFpsKey = key;
    frameCount = 0;
  }
}
void MSFS2020_AutoFlight::on_sb1P(double val) {
  if (planesWork->property(("P" + cb1Text + cb2Text).c_str()).isValid()) {
    planesWork->setProperty(("P" + cb1Text + cb2Text).c_str(), val);
  }
}
void MSFS2020_AutoFlight::on_sb1I(double val) {
  if (planesWork->property(("I" + cb1Text + cb2Text).c_str()).isValid()) {
    planesWork->setProperty(("I" + cb1Text + cb2Text).c_str(), val);
  }
}
void MSFS2020_AutoFlight::on_sb1D(double val) {
  if (planesWork->property(("D" + cb1Text + cb2Text).c_str()).isValid()) {
    planesWork->setProperty(("D" + cb1Text + cb2Text).c_str(), val);
  }
}
void MSFS2020_AutoFlight::on_cc1(bool state) {
  if (cb1Text != "") {
    ui->vs1->setEnabled(state);
    planesWork->setProperty(("source" + cb1Text).c_str(), !state);
  }
  else {
    ui->cc1->setCheckState(Qt::Unchecked);
  }
}

void MSFS2020_AutoFlight::on_cb1curr(QString text) {
  if (cb1Text != "") {
    planesWork->setProperty(("source" + cb1Text).c_str(), true);
  }
  ui->vs1->setEnabled(false);
  ui->cc1->setCheckState(Qt::Unchecked);
  cb1Text = text.toStdString();
  ui->sb1P->setValue(planesWork->property(("P" + cb1Text + cb2Text).c_str()).toDouble());
  ui->sb1I->setValue(planesWork->property(("I" + cb1Text + cb2Text).c_str()).toDouble());
  ui->sb1D->setValue(planesWork->property(("D" + cb1Text + cb2Text).c_str()).toDouble());
  ui->sb1P->setEnabled(planesWork->property(("P" + cb1Text + cb2Text).c_str()).isValid());
  ui->sb1I->setEnabled(planesWork->property(("I" + cb1Text + cb2Text).c_str()).isValid());
  ui->sb1D->setEnabled(planesWork->property(("D" + cb1Text + cb2Text).c_str()).isValid());
}
void MSFS2020_AutoFlight::on_cb2curr(QString text) {
  cb2Text = text.toStdString();
  ui->sb1P->setValue(planesWork->property(("P" + cb1Text + cb2Text).c_str()).toDouble());
  ui->sb1I->setValue(planesWork->property(("I" + cb1Text + cb2Text).c_str()).toDouble());
  ui->sb1D->setValue(planesWork->property(("D" + cb1Text + cb2Text).c_str()).toDouble());
  ui->sb1P->setEnabled(planesWork->property(("P" + cb1Text + cb2Text).c_str()).isValid());
  ui->sb1I->setEnabled(planesWork->property(("I" + cb1Text + cb2Text).c_str()).isValid());
  ui->sb1D->setEnabled(planesWork->property(("D" + cb1Text + cb2Text).c_str()).isValid());
}
void MSFS2020_AutoFlight::on_cb3curr(QString text) {
  cb3Text = text.toStdString();
}

void MSFS2020_AutoFlight::on_vs1(int pos) {
  planesWork->setProperty(cb1Text.c_str(), pos);
}


MSFS2020_AutoFlight::~MSFS2020_AutoFlight()
{
  delete ui;
}

Ui::MSFS2020_AutoFlightClass* MSFS2020_AutoFlight::GetUi()
{
    return ui;
}

void MSFS2020_AutoFlight::on_ButtonConnect()
{
  emit Connect();
}

void MSFS2020_AutoFlight::AppendListView(QString s) {
  
  ui->listWidget->addItem(s);
}
void MSFS2020_AutoFlight::ButtonModify(QPushButton* button, QString text, QString style) {
  if (text != "") {
    button->setText(text);
  }
  if (style != "") {
    button->setStyleSheet(style);
  }
}