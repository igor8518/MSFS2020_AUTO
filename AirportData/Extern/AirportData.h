#pragma once
#include "AirportData.h"
#include <SimConnect.h>
#include <vector>

//#include "Airport.h"
#if !defined(AIRPORTAPI)
#define AIRPORTAPI extern "C" IAirportData* __stdcall
#endif


#pragma pack(push, 1)

class DATA_RUNWAY {
	public:
		double		sLatitude;
		double		sLongitude;
		double		sHeading;
		double		eLatitude;
		double		eLongitude;
		double		eHeading;
		double    alt;
		std::string Name;
};

class BGLData {
	public:
		WORD	ID;
		DWORD	SizeOfRecord;
};

class AirportInfo: public BGLData {
	public:
		BYTE	RunwayCount;
		BYTE	COMCount;
		BYTE	StartCount;
		BYTE	ApproachCount;
		BYTE	ApronCount;
		BYTE	HelipadCount;
		DWORD	Lon;
		DWORD	Lat;
		DWORD	Alt;
		DWORD	TLon;
		DWORD	TLat;
		DWORD	TAlt;
		FLOAT	MagDeg;
		DWORD	ICAO;
		DWORD	Region;
		DWORD	FuelType;
		BYTE	Unk1;
		BYTE	TrafficScalar;
		WORD	Unk2;
};

class AirportInfoMSFS : public BGLData {
public:
	BYTE	RunwayCount;
	BYTE	COMCount;
	BYTE	StartCount;
	BYTE	ApproachCount;
	BYTE	ApronCount;
	BYTE	HelipadCount;
	DWORD	Lon;
	DWORD	Lat;
	DWORD	Alt;
	DWORD	TLon;
	DWORD	TLat;
	DWORD	TAlt;
	FLOAT	MagDeg;
	DWORD	ICAO;
	DWORD	Region;
	DWORD	FuelType;
	BYTE	TrafficScalar;
	BYTE Unk1;
	BYTE	Flags1;
	BYTE departureCt;
	BYTE onlyAddIfReplace;
	BYTE arrivalCt;
	BYTE	Unk2;
	BYTE applyFlatten;
	WORD apronCt;
	WORD paintedLineCt;
	WORD paintedPolygonCt;
	WORD paintedHatchedAreaCt;
};

class AirportInfoP3D : public BGLData {
public:
	BYTE	RunwayCount;
	BYTE	COMCount;
	BYTE	StartCount;
	BYTE	ApproachCount;
	BYTE	ApronCount;
	BYTE	HelipadCount;
	DWORD	Lon;
	DWORD	Lat;
	DWORD	Alt;
	DWORD	TLon;
	DWORD	TLat;
	DWORD	TAlt;
	FLOAT	MagDeg;
	DWORD	ICAO;
	DWORD	Region;
	DWORD	FuelType;
	BYTE	Unk1;
	BYTE	TrafficScalar;
	WORD	Unk2;
	DWORD Unk3;
};

class AirportInfoFS9 : public BGLData {
	public:
		BYTE	RunwayCount;
		BYTE	COMCount;
		BYTE	StartCount;
		BYTE	ApproachCount;
		BYTE	ApronCount;
		BYTE	HelipadCount;
		DWORD	Lon;
		DWORD	Lat;
		DWORD	Alt;
		DWORD	TLon;
		DWORD	TLat;
		DWORD	TAlt;
		FLOAT	MagDeg;
		DWORD	ICAO;
		DWORD	Region;
		DWORD	FuelType;
};

class TaxiwayPoints {
	public:
		BYTE	Type;
		BYTE	Flag;
		WORD	Unk1;
		DWORD	Lon;
		DWORD	Lat;
};
class TaxiwayPointsP3D {
public:
	BYTE	Type;
	BYTE	Flag;
	WORD	Unk1;
	DWORD	Lon;
	DWORD	Lat;
	DWORD Alt;
};

class C0x3E {
public:
	BYTE Unk[78];
};

class TaxiwayPointsHeadrer : public BGLData {
	public:
		WORD	CountTaxiwayPoints;
};

class TaxiwayPointsHeadrerP3D : public BGLData {
public:
	WORD	CountTaxiwayPoints;
};

class TaxiwayParksCodes {
	public:
		char	AirlineDesignator[4];
};

class TaxiwayParks {
	public:
		DWORD								TaxiParkInfo;
		FLOAT								Radius;
		FLOAT								Heading;
		FLOAT								TeeOffset1;
		FLOAT								TeeOffset2;
		FLOAT								TeeOffset3;
		FLOAT								TeeOffset4;
		DWORD								Lon;
		DWORD								Lat;
		std::vector<TaxiwayParksCodes>*		PTaxiwayParksCodes;
};

class TaxiwayParksMSFS {
public:
	DWORD								TaxiParkInfo;
	FLOAT								Radius;
	FLOAT								Heading;
	FLOAT								TeeOffset1;
	FLOAT								TeeOffset2;
	FLOAT								TeeOffset3;
	FLOAT								TeeOffset4;
	DWORD								Lon;
	DWORD								Lat;
	DWORD								Unk[5];
	std::vector<TaxiwayParksCodes>* PTaxiwayParksCodes;
};


class TaxiwayParksP3D {
public:
	DWORD								TaxiParkInfo;
	FLOAT								Radius;
	FLOAT								Heading;
	FLOAT								TeeOffset1;
	FLOAT								TeeOffset2;
	FLOAT								TeeOffset3;
	FLOAT								TeeOffset4;
	DWORD								Lon;
	DWORD								Lat;
	DWORD								Alt;
	std::vector<TaxiwayParksCodes>* PTaxiwayParksCodes;
};

class TaxiwayParksFS2004 {
	public:
		DWORD								TaxiParkInfo;
		FLOAT								Radius;
		FLOAT								Heading;
		DWORD								Lon;
		DWORD								Lat;
		std::vector<TaxiwayParksCodes>*		PTaxiwayParksCodes;
};


class TaxiwayParksHeader : public BGLData {
	public:
		WORD	CountTaxiwayParks;
};
class TaxiwayParksHeaderP3D : public BGLData {
public:
	WORD	CountTaxiwayParks;
};
class TaxiwayParksHeaderMSFS : public BGLData {
public:
	WORD	CountTaxiwayParks;
};

class TaxiwayPaths {
	public:
		WORD	IndexStartPoint;
		WORD	IndexEndPoint;
		BYTE	Type;
		BYTE	TaxiNameIndex;
		BYTE	Dashed;
		BYTE	Surface;
		FLOAT	Width;
		FLOAT	WeightLimit;
		DWORD	Unk1;
		DWORD End;
};

class TaxiwayPathsMSFS {
public:
	WORD	IndexStartPoint;
	WORD	LegacyEnd;
	BYTE	Type;
	BYTE	TaxiNameIndex;
	BYTE	Dashed;
	BYTE	Surface;
	FLOAT	Width;
	DWORD	WeightLimit;
	DWORD Unk1;
	DWORD	Unk2;
	BYTE GUID[16];
	DWORD Coloration;
	BYTE Unk3;
	BYTE Flags;
	WORD	IndexEndPoint;
};

class TaxiwayPathsHeader : public BGLData {
	public:
		WORD	CountTaxiwayParks;
};

class FIXX {
	public:
		std::string		Name;
		double			Lat;
		double			Lon;
};

class SRNW {
	public:
		std::string		Name;
};

class WayPointA {
	public:
		std::string		TypeName;
		DWORD			TYPE;
		//std::string  TYPE;
		double			TRK;
		std::string		FIXName;
		double			ALT;
		double			ALT2;
		double			SPEED;
		double			RADIAL;
		double			LegTime;
		double			LegDist;
};

class TRANSITION {
	public:
		std::string					Name;
		std::vector<WayPointA>*		Legs;
};

class Points {
	public:
		std::string					Name;
		std::vector<std::string>*	Runways;
		std::vector<WayPointA>*		Legs;
		std::vector<TRANSITION>*	TRANSITIONS;
};

class SIDSTAR {
	public:
		std::vector<FIXX>*			FIXES;
		std::vector<SRNW>*			RNWS;
		std::vector<Points>*		SIDS;
		std::vector<Points>*		STARS;
		std::vector<Points>*		APPROACHES;
};

class TaxiwayNames {
	public:
		char	TaxiName[8];
};

class TaxiwayNamesHeader : public BGLData {
	public:
		WORD	CountTaxiwayNames;
};

class RunwayPaths {
	public:
		WORD				Runway;
		std::string			RunwayName1;
		double Deg1 = 0.0;
		double R1Dist = 0.0;
		std::string			RunwayName2;
		double Deg2 = 0.0;
		double R2Dist = 0.0;
		std::vector<WORD>	Path;
		double Lenght = 0.0;
};

class AirportList {
	public:
		DWORD			Version;
		std::string		ICAO;
		std::string	File;
		DWORD			FileOffset;
		double			lat;
		double			Lon;
		WORD			Layer;
		DWORD			mask;
};

class TPath {
	public:
		DWORD			index;
		std::string		name;
		double			Lat;
		double			Lon;
		WORD			Type;
};

class AirportPaths {
	public:
		std::vector<int>*				Starts;
		AirportInfo*					PAirportInformation		= 0;
		TaxiwayPointsHeadrer*			PHTaxiwayPoints			= 0;
		std::vector<TaxiwayPoints>*		PTaxiwayPoints			= 0;
		TaxiwayParksHeader*				PHTaxiwayParks			= 0;
		std::vector<TaxiwayParks>*		PTaxiwayParks			= 0;
		TaxiwayPathsHeader*				PHTaxiwayPaths			= 0;
		std::vector<TaxiwayPaths>*		PTaxiwayPaths			= 0;
		TaxiwayNamesHeader*				PHTaxiwayNames			= 0;
		std::vector<TaxiwayNames>*		PTaxiwayNames			= 0;
		std::vector<RunwayPaths>*		RPth;
};

#pragma pack(pop)

class IAirport
{

public:
	std::vector<int>*				Starts;
	AirportInfo*					PAirportInformation	= 0;
	TaxiwayPointsHeadrer*			PHTaxiwayPoints		= 0;
	TaxiwayPointsHeadrerP3D* PHTaxiwayPointsP3D = 0;
	std::vector<TaxiwayPoints>*		PTaxiwayPoints		= 0;
	std::vector<TaxiwayPointsP3D>* PTaxiwayPointsP3D = 0;
	TaxiwayParksHeader*				PHTaxiwayParks		= 0;
	std::vector<TaxiwayParks>*		PTaxiwayParks		= 0;
	TaxiwayParksHeaderMSFS* PHTaxiwayParksMSFS = 0;
	std::vector<TaxiwayParksMSFS>* PTaxiwayParksMSFS = 0;
	TaxiwayParksHeaderP3D* PHTaxiwayParksP3D = 0;
	std::vector<TaxiwayParksP3D>* PTaxiwayParksP3D = 0;
	TaxiwayPathsHeader*				PHTaxiwayPaths		= 0;
	std::vector<TaxiwayPaths>*		PTaxiwayPaths		= 0;
	std::vector<TaxiwayPathsMSFS>* PTaxiwayPathsMSFS = 0;
	TaxiwayNamesHeader*				PHTaxiwayNames		= 0;
	std::vector<TaxiwayNames>*		PTaxiwayNames		= 0;
	std::vector<RunwayPaths>*		RPth;
	SIDSTAR*						sidstar				= 0;
	
};

class IAirportData
{
	public: 
		virtual UINT											FillDB()																																		= 0;
		virtual int												GetProgress()																																= 0;
		virtual void											Release()																																		= 0;
		virtual AirportList								GetNearAirport(double Lat, double Lon, int index = -1)											= 0;
    virtual AirportList							  GetAirportByIcao(std::string icao)																					= 0;
		virtual std::vector<RunwayPaths>*	GetRunways()																																= 0;
		virtual std::vector<int>*					GetRunwayStart(double lat, double lon, double direction)										= 0;
		virtual std::vector<TPath>				GetPath(DWORD start, DWORD heading, double direction)												= 0;
		virtual std::vector<TPath>				GetPath1(DWORD start, DWORD heading, int end)																= 0;
		virtual DATA_RUNWAY								GetStartCoord(int index)																										= 0;
		virtual DWORD											GetNearTaxiwayPoint(double Lat, double Lon)																	= 0;
		virtual DWORD											GetNearTaxiwayPath(double Lat, double Lon, double heading)									= 0;
		virtual std::vector<TPath>				GetPathGate(DWORD path, DWORD heading, double radius, TaxiwayParks * park)	= 0;
		virtual void											SetHSim(HANDLE hSim)																												= 0;
		virtual void											DispatchProc(SIMCONNECT_RECV * pData, DWORD cbData, void * pContext)				= 0;
		virtual IAirport *								GetAirport()																																= 0;
		std::vector<double>*							d																																						= 0;
		std::vector<TPath>*								ReturnPath;
		std::string											RootSim;
	};


AIRPORTAPI GetAirportData();




