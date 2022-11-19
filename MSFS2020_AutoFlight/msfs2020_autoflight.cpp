#include "msfs2020_autoflight.h"
#include "Structs.h"


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
  //QBrush BackColor;
  //BackColor.setColor(QColor(240,240,240));
  //ui->Graph2->setBackground(Qt::transparent);
  //ui->cb1->addItem(this->props.);
  QLinearGradient plotGradient;

  plotGradient.setStart(0, 0);

  plotGradient.setFinalStop(0, 350);

  plotGradient.setColorAt(0, QColor(240, 240, 240));

  plotGradient.setColorAt(1, QColor(240, 240, 240));

  ui->Graph2->setBackground(plotGradient);
  ui->Graph->setBackground(plotGradient);
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

  ui->Graph2->xAxis->setRange(0, 400);
  ui->Graph2->axisRect()->setupFullAxesBox();
  ui->Graph2->yAxis->setRange(0, 40000);
  pos = new QCPItemEllipse(ui->Graph2);
  
  //ui->Graph->graph(0)->adaptiveSampling();
  //ui->Graph->graph(1)->adaptiveSampling();

  //Profile draw
  /*ui->Graph2->addGraph(); // blue line
  ui->Graph2->graph(0)->setPen(QPen(QColor(40, 110, 255)));
  ui->Graph2->addGraph(); // red line
  ui->Graph2->graph(1)->setPen(QPen(QColor(255, 110, 40)));
  ui->Graph2->addGraph(); // green line
  ui->Graph2->graph(2)->setPen(QPen(QColor(110, 255, 40)));*/

  //ui->Graph2->xAxis->setTicker(timeTicker);
  

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(ui->Graph2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Graph2->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->Graph2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Graph2->yAxis2, SLOT(setRange(QCPRange)));


  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
  //connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(on_ChangeRow(QItemSelection, QItemSelection)));
}
void MSFS2020_AutoFlight::realtimeDataSlot() {
	static QTime time(QTime::currentTime());
	// calculate two new data points:
	double key = time.elapsed() / 1000.0; // time elapsed since start of demo, in seconds
	static double lastPointKey = 0;

	if (key - lastPointKey > 0.02) // at most add point every 20 ms
	{
		// add data to lines:
		ui->vs1->setMinimum(-16383);
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
		//QItemSelectionModel* st = ui->tableView->selectionModel();

		// rescale value (vertical) axis to fit the current data:
		//ui->customPlot->graph(0)->rescaleValueAxis();
		//ui->customPlot->graph(1)->rescaleValueAxis(true);
		lastPointKey = key;
	}
	// make key axis range scroll with the data (at a constant range size of 80):
	ui->Graph->xAxis->setRange(key, 80, Qt::AlignRight);
	ui->Graph->replot();
	ui->Graph2->replot();
	ui->tableView->repaint();
	/*if (mainLogic && mainLogic->data) {
		double flyPoint = mainLogic->data->GData.FLAPS_HANDLE_INDEX;
		static double lastFlyPoint = 0;
		if (flyPoint - lastFlyPoint > 0.1) { // at most add point every 2 ms

		}
	}*/

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

void MSFS2020_AutoFlight::PlotCircle(double dist, double alt, double common) {
	pos->topLeft->setCoords({ dist - 1, alt + 300 });
	pos->bottomRight->setCoords({ dist + 1, alt - 300 });
	if (common > 0) {
		ui->Graph2->xAxis->setRange(0, common);
	}
}

void MSFS2020_AutoFlight::PlotConstraints(std::vector<sWayPoint>* Legs, int startIndex, int endIndex, int currentIndex) {
	double CommonDistance = 0;
	for (int i = startIndex; i <= endIndex; i++) {
		/*if ((i - startIndex) >= dnArrow->size()) {
			dnArrow->push_back(new QCPItemLine(ui->Graph2));
		}
		if ((i - startIndex) >= upArrow->size()) {
			upArrow->push_back(new QCPItemLine(ui->Graph2));
		}
		if (Legs->at(i).EAltitudeHi > 0) {
			dnArrow->at(i - startIndex)->start->setCoords(Legs->at(i).CommonDistance + Legs->at(i).RealDistance, Legs->at(i).EAltitudeHi + 500);
			dnArrow->at(i - startIndex)->end->setCoords(Legs->at(i).CommonDistance + Legs->at(i).RealDistance, Legs->at(i).EAltitudeHi);
			dnArrow->at(i - startIndex)->setHead(QCPLineEnding::esSpikeArrow);
		}
		if (Legs->at(i).EAltitudeLo > 0) {
			upArrow->at(i - startIndex)->start->setCoords(Legs->at(i).CommonDistance + Legs->at(i).RealDistance, Legs->at(i).EAltitudeLo - 500);
			upArrow->at(i - startIndex)->end->setCoords(Legs->at(i).CommonDistance + Legs->at(i).RealDistance, Legs->at(i).EAltitudeLo);
			upArrow->at(i - startIndex)->setHead(QCPLineEnding::esSpikeArrow);
		}
		if ((i - startIndex) >= flightPoints->size()) {
			flightPoints->push_back(new QCPItemLine(ui->Graph2));
		}
		flightPoints->at(i - startIndex)->start->setCoords(Legs->at(i).CommonDistance + Legs->at(i).RealDistance, 0);
		flightPoints->at(i - startIndex)->end->setCoords(Legs->at(i).CommonDistance + Legs->at(i).RealDistance, 40000);
		CommonDistance = CommonDistance + Legs->at(i).Distance + Legs->at(i).RealDistance;*/
		if ((i - startIndex) >= dnArrow->size()) {
			dnArrow->push_back(new QCPItemLine(ui->Graph2));
		}
		if ((i - startIndex) >= upArrow->size()) {
			upArrow->push_back(new QCPItemLine(ui->Graph2));
		}
		if (Legs->at(i).EAltitudeHi > 0) {
			dnArrow->at(i - startIndex)->start->setCoords(Legs->at(i).CommonDistance, Legs->at(i).EAltitudeHi + 500);
			dnArrow->at(i - startIndex)->end->setCoords(Legs->at(i).CommonDistance, Legs->at(i).EAltitudeHi);
			dnArrow->at(i - startIndex)->setHead(QCPLineEnding::esSpikeArrow);
		}
		if (Legs->at(i).EAltitudeLo > 0) {
			upArrow->at(i - startIndex)->start->setCoords(Legs->at(i).CommonDistance, Legs->at(i).EAltitudeLo - 500);
			upArrow->at(i - startIndex)->end->setCoords(Legs->at(i).CommonDistance, Legs->at(i).EAltitudeLo);
			upArrow->at(i - startIndex)->setHead(QCPLineEnding::esSpikeArrow);
		}
		if ((i - startIndex) >= flightPoints->size()) {
			flightPoints->push_back(new QCPItemLine(ui->Graph2));
		}
		flightPoints->at(i - startIndex)->start->setCoords(Legs->at(i).CommonDistance, 0);
		flightPoints->at(i - startIndex)->end->setCoords(Legs->at(i).CommonDistance, 40000);
	}
	//ui->Graph2->xAxis->setRange(0, CommonDistance);
	//ui->Graph2->replot();
}

void MSFS2020_AutoFlight::PlotPoints(std::vector<sWayPoint>* Legs, int startIndex, int endIndex) {
	int CommonDistance = 0;
	for (int i = startIndex; i <= endIndex; i++) {
		if ((i - startIndex) >= flightPoints->size()) {
			flightPoints->push_back(new QCPItemLine(ui->Graph2));
		}
		flightPoints->at(i - startIndex)->start->setCoords(Legs->at(i).CommonDistance + Legs->at(i).RealDistance, 0);
		flightPoints->at(i - startIndex)->end->setCoords(Legs->at(i).CommonDistance + Legs->at(i).RealDistance, 40000);
	}
	
	//ui->Graph2->replot();
}

/*void MSFS2020_AutoFlight::PlotProbePath() {

}*/

void MSFS2020_AutoFlight::PlotRealPath(double flyPoint, double planeAlt, double commonDistance) {
	if (ui->Graph2->graphCount() == 0) {
		ui->Graph2->addGraph();
		ui->Graph->graph(0)->setPen(QPen(QColor(40, 110, 255)));
	}
	ui->Graph2->graph(0)->addData(flyPoint, planeAlt);
	pos->topLeft->setCoords({ flyPoint - 1, planeAlt + 300 });
	pos->bottomRight->setCoords({ flyPoint + 1, planeAlt - 300 });
	if (commonDistance > 0) {
		ui->Graph2->xAxis->setRange(0, commonDistance);
	}
	//ui->Graph2->replot();
}

void MSFS2020_AutoFlight::SelectRow(int Row) {
	ui->tableView->selectRow(Row);
}

void MSFS2020_AutoFlight::on_ChangeRow(QItemSelection Row, QItemSelection prev) {
	emit ChangeCurrentLegIndex(Row.indexes().at(0).row() - 1);	
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

void MSFS2020_AutoFlight::on_PMDGButton()
{
	emit PMDGSend(ui->switchID->value());
}

void MSFS2020_AutoFlight::on_startButton()
{
	emit StartSend();
}

void MSFS2020_AutoFlight::AppendListView(QString s) {
  
  ui->listWidget->addItem(s);
  ui->listWidget->scrollToBottom();
}
void MSFS2020_AutoFlight::ButtonModify(QPushButton* button, QString text, QString style) {
  if (text != "") {
    button->setText(text);
  }
  if (style != "") {
    button->setStyleSheet(style);
  }
}
void MSFS2020_AutoFlight::ConnectButtonEnabled(bool enabled) {
	ui->ConnectButton->setEnabled(enabled);
}
void MSFS2020_AutoFlight::StartButtonEnabled(bool enabled) {
	ui->startButton->setEnabled(enabled);
}