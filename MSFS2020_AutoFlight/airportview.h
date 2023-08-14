#pragma once

#include <QDialog>
#include <QTimer>
#include "ui_airportview.h"


class MainLogic;

class AirportView : public QDialog, public Ui::AirportViewClass
{
	Q_OBJECT

public:
	AirportView(MainLogic* mainlogic, QWidget* parent = Q_NULLPTR);
	~AirportView();
	
	QPainter* painter;
	MainLogic* mainlogic;
	IAirport* CurrentAirport = NULL;
	void paintEvent(QPaintEvent* event);
protected:
	

private slots:
	void update();
	void ChangeToDeparture();
	void ChangeToDestination();
	void Close();
};
