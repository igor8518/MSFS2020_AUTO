#pragma once
#include "BGLXData.h"
#include "AirportData.h"
#include "sidstar.h"
//#include <FlexLexer.h>




class Airport : public IAirport {
	public:
		// Constructors

		Airport(BGLXData * BGLX, DWORD AirportDataOffset, IAirportData * AirportData);
		Airport(std::string BGLFile, DWORD AirportDataOffset, IAirportData * AirportData);
		~Airport();

		// Methods

		HRESULT							GetTaxiwayInformation();
		HRESULT							GetRunwayInformation();
		HRESULT							GetSIDSTAR();

	private:
		// Initialized vars
    
		std::vector<BGLXData::Record>*	Records			= 0;
		bool							mode			= false;
		IAirportData*					AirportData		= 0;
		BGLXData*						BGLX			= 0;

		// Vars
		
		DWORD							AirportDataOffset;

		// Methods

		std::string						GetTypeNameFromPoint(sroute point);
		HRESULT							GetAirportData(bool full);
		HRESULT							GetFixes(ReadStreamText * SIDSTARFile);
		HRESULT							GetSids(ReadStreamText * SIDSTARFile);
		
		
};
