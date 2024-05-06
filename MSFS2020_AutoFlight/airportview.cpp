#include <QPainter>
#include <QPaintEvent>
#include "mainlogic.h"
#include "AirportData.h"
#include "utils.h"
#include "airportview.h"



AirportView::AirportView(MainLogic* mainlogic, QWidget* parent)
	: QDialog(parent), mainlogic(mainlogic)
{
	
	CurrentAirport = mainlogic->Departure;
	
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
	connect(BClose, SIGNAL(clicked()), this, SLOT(close()));
	connect(BDeparture, SIGNAL(clicked()), this, SLOT(ChangeToDeparture()));
	connect(BDestination, SIGNAL(clicked()), this, SLOT(ChangeToDestination()));
	ChangeToDeparture();
}

AirportView::~AirportView()
{}

void AirportView::Update() {
	std::vector<TaxiwayPaths> types;
	painter = new QPainter(this);
	
	double width = this->width();
	double height = this->height();
	
	painter->setRenderHint(QPainter::Antialiasing);
	if (CurrentAirport != NULL) {

		AddAirportInfo* AAI = CurrentAirport->PAddAirportInformation;
		AirportInfo* AI = CurrentAirport->PAirportInformation;
		std::vector<TaxiwayParks>* TParks = CurrentAirport->PTaxiwayParks;
		std::vector<TaxiwayPoints>* TPoints = CurrentAirport->PTaxiwayPoints;
		std::vector<TaxiwayPaths>* TPaths = CurrentAirport->PTaxiwayPaths;

		double MinLat = AAI->MinLat;
		double MinLon = AAI->MinLon;
		double MaxLat = AAI->MaxLat;
		double MaxLon = AAI->MaxLon;
		double Lat = Utils::DecodeLat(AI->Lat);
		double Lon = Utils::DecodeLon(AI->Lon);
		double dLat = AAI->DLat;
		double deltaLat = cos(Utils::DegToRad(Lat));
		//deltaLat = 1;
		double dLon = AAI->DLon;
		//painter->translate(QPoint(10.0 / deltaLat, 10.0));
		width = (width - 20.0)/ deltaLat;
		height = height - 20.0;
		double rcRate  = (width  / height);
		double AirRate = dLon / dLat;
		double drawRate;

		
		if (AirRate <= rcRate) {
			drawRate = height / dLat;
		}
		else {
			drawRate = width/ dLon;
			//drawRate = ((20.0 / deltaLat) - width) / dLon;
		}

		for (int i = 0; i < TPaths->size(); i++) {
			if ((TPaths->at(i).Type & 0xf) == 0x1) {
				QColor color(0, 0, 255, 255);
				QPen apen = QPen(color);
				apen.setWidth(4);
				painter->setPen(apen);
			}
			else if ((TPaths->at(i).Type & 0xf) == 0x2) {
				QColor color(0, 0, 0, 255);
				QPen apen = QPen(color);
				apen.setWidth(4);
				painter->setPen(apen);
			}
			else if ((TPaths->at(i).Type & 0xf) == 0x3) {
				QColor color(255, 0, 0, 255);
				QPen apen = QPen(color);
				apen.setWidth(2);
				painter->setPen(apen);
			}
			else if ((TPaths->at(i).Type & 0xf) == 0x4) {
				QColor color(0, 255, 0, 255);
				QPen apen = QPen(color);
				apen.setWidth(4);
				painter->setPen(apen);
			}
			else if ((TPaths->at(i).Type & 0xf) == 0x7) {
				QColor color(0, 255, 255, 255);
				QPen apen = QPen(color);
				apen.setWidth(4);
				painter->setPen(apen);
			}
			else if ((TPaths->at(i).Type & 0xf) == 0x6) {
				QColor color(255, 127, 255, 255);
				QPen apen = QPen(color);
				apen.setWidth(4);
				painter->setPen(apen);
			}
			else if ((TPaths->at(i).Type & 0xf) == 0x9) {
				QColor color(127, 127, 127, 255);
				QPen apen = QPen(color);
				apen.setWidth(4);
				painter->setPen(apen);
			}
			else {
				QColor color(255, 255, 255, 255);
				QPen apen = QPen(color);
				apen.setWidth(4);
				painter->setPen(apen);
			}
			TaxiwayPaths TPath = TPaths->at(i);
			double SPLat = Utils::DecodeLat(TPoints->at(TPath.IndexStartPoint).Lat);
			double SPLon = Utils::DecodeLon(TPoints->at(TPath.IndexStartPoint).Lon);
			double EPLat = 0.0;
			double EPLon = 0.0;
			if ((TPath.Type & 0xf) == 0x3) {
				EPLat = Utils::DecodeLat(TParks->at(TPath.End).Lat);
				EPLon = Utils::DecodeLon(TParks->at(TPath.End).Lon);
			}
			else {
				EPLat = Utils::DecodeLat(TPoints->at(TPath.End).Lat);
				EPLon = Utils::DecodeLon(TPoints->at(TPath.End).Lon);
			}

			if (((TPath.Type & 0xf) >= 0x1) && ((TPath.Type & 0xf) <= 0x4)) {

				double A1 = (SPLon - MinLon) * drawRate * deltaLat;
				double A2 = (EPLon - MinLon) * drawRate * deltaLat;
				double B  = dLon * drawRate;
				double C  = (- ((dLon * drawRate)) / 2.0 + (width) / 2.0 ) * deltaLat;

				double X1 = A1 + C + 10;
				double Y1 = height - ((SPLat - MinLat) * drawRate			) + (((dLat * drawRate)) / 2.0) - height / 2.0 + 10;
				double X2 = A2 + C + 10;
				double Y2 = height - ((EPLat - MinLat) * drawRate			) + (((dLat * drawRate)) / 2.0) - height / 2.0 + 10;

				painter->drawLine(QLineF(X1, Y1, X2, Y2));
			}
			/*else if (((TPath.Type & 0xf) == 0x7) || ((TPath.Type & 0xff) == 0x66) || ((TPath.Type & 0xff) == 0x46) || ((TPath.Type & 0xff) == 0x39)) {
				double A1 = (SPLon - MinLon) * drawRate * deltaLat;
				double A2 = (EPLon - MinLon) * drawRate * deltaLat;
				double B  = dLon * drawRate;
				double C  = (- ((dLon * drawRate)) / 2.0 + (width) / 2.0 ) * deltaLat;

				double X1 = A1 + C + 10;
				double Y1 = height - ((SPLat - MinLat) * drawRate			) + (((dLat * drawRate)) / 2.0) - height / 2.0 + 10;
				double X2 = A2 + C + 10;
				double Y2 = height - ((EPLat - MinLat) * drawRate			) + (((dLat * drawRate)) / 2.0) - height / 2.0 + 10;

				painter->drawLine(QLineF(X1, Y1, X2, Y2));

			}*/
			else {
				types.push_back(TPath);
			}
		}
		
		if (mainlogic->DeparturePath.size() >= 2) {
			for (int i = 0; i < mainlogic->DeparturePath.size() - 1; i++) {
				QColor color;
				if (mainlogic->DeparturePath[i].Type == 11) {
					color.setRgba(QColor("red").rgba());
				}
				else if (mainlogic->DeparturePath[i].Type == 12){
					color.setRgba(QColor("purple").rgba());
				}
				else {
					color.setRgba(QColor("green").rgba());
				}
				QPen apen = QPen(color);
				apen.setWidth(5);
				painter->setPen(apen);

				double A1 = (mainlogic->DeparturePath[i].Lon - MinLon) * drawRate * deltaLat;
				double A2 = (mainlogic->DeparturePath[i + 1].Lon - MinLon) * drawRate * deltaLat;
				double B = dLon * drawRate;
				double C = (-((dLon * drawRate)) / 2.0 + (width) / 2.0) * deltaLat;

				double X1 = A1 + C + 10;
				double Y1 = height - ((mainlogic->DeparturePath[i].Lat - MinLat) * drawRate) + (((dLat * drawRate)) / 2.0) - height / 2.0 + 10;
				double X2 = A2 + C + 10;
				double Y2 = height - ((mainlogic->DeparturePath[i + 1].Lat - MinLat) * drawRate) + (((dLat * drawRate)) / 2.0) - height / 2.0 + 10;

				painter->drawLine(QLineF(X1, Y1, X2, Y2));
			}
		}
	}

	painter->end();
	delete painter;
}

void AirportView::paintEvent(QPaintEvent*)
{
	Update();

}

void AirportView::FillPointLists(IAirport* airport) {
	CBFrom->clear();
	CBTo->clear();
	for (int i = 0; i < airport->PTaxiwayParks->size(); i++) {
		std::string ParkName = airport->GetParkName(&airport->PTaxiwayParks->at(i));
		CBFrom->addItem(ParkName.c_str());
	}
	for (int i = 0; i < airport->POneWayRunways->size(); i++) {
		std::string ParkName = airport->POneWayRunways->at(i).Name;
		CBTo->addItem(ParkName.c_str());
	}
}

void AirportView::ChangeToDeparture() {
	CurrentAirport = mainlogic->Departure;
	FillPointLists(CurrentAirport);
	label->setText(Utils::DecodeICAO(CurrentAirport->PAirportInformation->ICAO).c_str());
	this->update();
}

void AirportView::ChangeToDestination() {
	CurrentAirport = mainlogic->Destination;
	FillPointLists(CurrentAirport);
	label->setText(Utils::DecodeICAO(CurrentAirport->PAirportInformation->ICAO).c_str());
	this->update();
}

void AirportView::Close() {
	this->close();
}


