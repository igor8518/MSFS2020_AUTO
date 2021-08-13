#pragma once
#include <vector>
#include "BGLXData.h"
class BGLXData
{
public:
	//structures
	struct FSXBGLHeader
	{
		DWORD Magic1;
		DWORD HeaderSixe;
		INT64 DateTime;
		DWORD Magic2;
		DWORD SectionsCount;
		DWORD QMID[8];
	} *PFSXHeader = NULL;

	struct FSXBGLSection
	{
		DWORD SectionType;
		DWORD SubSectionsSize;
		DWORD CountSubSections;
		DWORD OffsetSubSections;
		DWORD SizeSubsections;
	};

	struct FSXBGLSubSection
	{
		DWORD QUMIDA;
		DWORD QUMIDB;
		DWORD CountRecordsSubSection;
		DWORD OffsetRecords;
		DWORD SizeRecords;
	};

	struct FSXBGLSubSection16
	{
		DWORD QUMIDA;
		DWORD CountRecordsSubSection;
		DWORD OffsetRecords;
		DWORD SizeRecords;
	};
	struct Record
	{
		WORD ID;
		DWORD Offset;
		DWORD Size;
		WORD ParrentID;
	};
	bool NotBGLX = false;
	ReadStream* BGLXFile = NULL;
	BGLXData(std::string filename);
	~BGLXData();
	std::vector<BGLXData::Record>* GetAirportRecordHierarhy(DWORD Offset, std::vector<BGLXData::Record>* ParentRecord = NULL, WORD level = 0);
	std::vector<FSXBGLSection>* GetSection(int ID);
	std::vector<FSXBGLSubSection16>* GetSubSection16(std::vector<FSXBGLSection>* Sections);
	std::vector<FSXBGLSubSection>* GetSubSection(std::vector<FSXBGLSection>* Sections);

	static std::string DecodeICAO(DWORD CodedICAO);
	static double DecodeLat(DWORD lat);
	static double DecodeLon(DWORD lon);
};

