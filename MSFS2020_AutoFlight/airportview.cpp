#include <QPainter>
#include <QPaintEvent>
#include "mainlogic.h"
#include "AirportData.h"

#include "airportview.h"



AirportView::AirportView(MainLogic* mainlogic, QWidget* parent)
	: QDialog(parent), mainlogic(mainlogic)
{
	
	CurrentAirport = mainlogic->Departure;
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
	QTimer* TPaint = new QTimer(this);
	connect(TPaint, SIGNAL(timeout()), this, SLOT(Update()));

	TPaint->start(1);
}

AirportView::~AirportView()
{}

void AirportView::update() {
	std::vector<TaxiwayPaths> types;
	painter = new QPainter(this);
	
	int width = this->width();
	int height = this->height();
	
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
		double dLon = AAI->DLon;
		painter->translate(QPoint(10.0 / deltaLat, 10.0));
		width = (width) / deltaLat;
		double rcRate = (width - (20.0 / deltaLat)) / (height - 20.0);
		double AirRate = dLon / dLat;
		double drawRate;

		//deltaLat = 1;
		if (AirRate <= rcRate) {
			drawRate = (height - 20.0) / dLat;
		}
		else {
			drawRate = (width - (20.0 / deltaLat)) / dLon;
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

				painter->drawLine(QLineF(
					(((SPLon - MinLon) * drawRate * deltaLat) + (((dLon * drawRate) - width) / 2.0)),
					(height - ((SPLat - MinLat) * drawRate) + (((dLat * drawRate) - height) / 2.0)),
					(((EPLon - MinLon) * drawRate * deltaLat) + (((dLon * drawRate) - width) / 2.0)),
					(height - ((EPLat - MinLat) * drawRate) + (((dLat * drawRate) - height) / 2.0))));
			}
			/*else if (((TPath.Type & 0xf) == 0x7) || ((TPath.Type & 0xff) == 0x66) || ((TPath.Type & 0xff) == 0x46) || ((TPath.Type & 0xff) == 0x39)) {
				painter->drawLine(QLineF(
					(((SPLon - MinLon) * drawRate * deltaLat) + (((dLon * drawRate) - width) / 2.0)),
					(height - ((SPLat - MinLat) * drawRate) + (((dLat * drawRate) - height) / 2.0)),
					(((EPLon - MinLon) * drawRate * deltaLat) + (((dLon * drawRate) - width) / 2.0)),
					(height - ((EPLat - MinLat) * drawRate) + (((dLat * drawRate) - height) / 2.0))));

			}*/
			else {
				types.push_back(TPath);
			}
		}
		
		if (mainlogic->DeparturePath.size() >= 2) {
			for (int i = 0; i < mainlogic->DeparturePath.size() - 1; i++) {
				QColor color;
				if (mainlogic->DeparturePath[i].Type != 11) {
					color.setRgba(QColor("green").rgba());
				}
				else {
					color.setRgba(QColor("red").rgba());
				}
				QPen apen = QPen(color);
				apen.setWidth(5);
				painter->setPen(apen);
				painter->drawLine(QLineF(
					(((mainlogic->DeparturePath[i].Lon - MinLon) * drawRate * deltaLat) + (((dLon * drawRate) - width) / 2.0)),
					(height - ((mainlogic->DeparturePath[i].Lat - MinLat) * drawRate) + (((dLat * drawRate) - height) / 2.0)),
					(((mainlogic->DeparturePath[i + 1].Lon - MinLon) * drawRate * deltaLat) + (((dLon * drawRate) - width) / 2.0)),
					(height - ((mainlogic->DeparturePath[i + 1].Lat - MinLat) * drawRate) + (((dLat * drawRate) - height) / 2.0))));
			}
		}
	}

	painter->end();
}

void AirportView::paintEvent(QPaintEvent*)
{
	update();

}

void AirportView::ChangeToDeparture() {
	CurrentAirport = mainlogic->Departure;
	this->update();
}

void AirportView::ChangeToDestination() {
	CurrentAirport = mainlogic->Destination;
	this->update();
}

void AirportView::Close() {
	this->close();
}


