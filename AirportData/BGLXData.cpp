#include "stdafx.h"
#include "BGLXData.h"


BGLXData::BGLXData(std::string filename)
{
	BGLXFile = new ReadStream(filename);
	PFSXHeader = new FSXBGLHeader;
	BGLXFile->Read(PFSXHeader, 0, sizeof(*PFSXHeader));
	if (PFSXHeader->Magic1 != 0x19920201)
	{
		NotBGLX = true;
		return;
	}
	if (PFSXHeader->Magic2 != 0x08051803)
	{
		NotBGLX = true;
		return;
	}
	NotBGLX = false;
}



BGLXData::~BGLXData()
{
	if (PFSXHeader)
	{
		delete PFSXHeader;
		PFSXHeader = NULL;
	}
	if (BGLXFile)
	{
		delete BGLXFile;
		BGLXFile = NULL;
	}
}

std::vector<BGLXData::Record>* BGLXData::GetAirportRecordHierarhy(DWORD Offset, std::vector<BGLXData::Record>* ParentRecord, WORD level)
{
	int parentOffset;
	int parentSize;
	DWORD CurrentOffset = Offset;
	BGLData* PBGLData = new BGLData;
	std::vector<Record>* Records;
	if (ParentRecord)
	{
		Records = ParentRecord;
		parentOffset = ParentRecord->at(ParentRecord->size() - 1).Offset;
		parentSize = ParentRecord->at(ParentRecord->size() - 1).Size;
	}
	else
	{
		Records = new std::vector<Record>();
		
	}
	do
	{
		BGLXFile->Read(PBGLData, CurrentOffset, sizeof(*PBGLData));
		Records->push_back({ PBGLData->ID, CurrentOffset, PBGLData->SizeOfRecord, level });
		switch (PBGLData->ID)
		{
		case 0x03:
		{
			CurrentOffset = CurrentOffset + sizeof(AirportInfoFS9);
			GetAirportRecordHierarhy(CurrentOffset, Records, level + 1);
			break;
		}
		case 0x3C:
		{
			CurrentOffset = CurrentOffset + sizeof(AirportInfo);
			GetAirportRecordHierarhy(CurrentOffset, Records, level + 1);
			break;
		}
		case 0xAB:
		{
			CurrentOffset = CurrentOffset + sizeof(AirportInfoP3D);
			GetAirportRecordHierarhy(CurrentOffset, Records, level + 1);
			break;
		}
		case 0x56:
		{
			CurrentOffset = CurrentOffset + sizeof(AirportInfoMSFS);
			GetAirportRecordHierarhy(CurrentOffset, Records, level + 1);
			break;
		}
		case 0xce:
		{
			GetAirportRecordHierarhy(CurrentOffset + sizeof(RunwayInfoMSFS), Records, level + 1);
			CurrentOffset = CurrentOffset + PBGLData->SizeOfRecord;
			break;
		}
		default:
		{
			CurrentOffset = CurrentOffset + PBGLData->SizeOfRecord;
			break;
		}	
		}
		if (!ParentRecord)
		{
			break;
		}
		else
		{

		}
	} while (parentOffset + parentSize > CurrentOffset);
	return Records;
}



std::vector<BGLXData::FSXBGLSection>* BGLXData::GetSection(int ID)
{
	if (NotBGLX)
	{
		return NULL;
	}
	FSXBGLSection* Section = new FSXBGLSection;
	std::vector<BGLXData::FSXBGLSection>* BGLSections = new std::vector<BGLXData::FSXBGLSection>();
	for (DWORD i = 0; i < PFSXHeader->SectionsCount; i++)
	{
		
		BGLXFile->Read(Section, PFSXHeader->HeaderSixe + i*sizeof(*Section), sizeof(*Section));
		if (Section->SectionType == ID)
		{
			BGLSections->push_back(*Section);
		}	
	}
	if (BGLSections->size() == 0)
	{
		delete BGLSections;
		BGLSections = NULL;
	}
	delete Section;
	Section = NULL;
	return BGLSections;
}

std::vector<BGLXData::FSXBGLSubSection>* BGLXData::GetSubSection(std::vector<FSXBGLSection>* Sections)
{
	std::vector<BGLXData::FSXBGLSubSection>* BGLSubSections = new std::vector<BGLXData::FSXBGLSubSection>();
	for (int i = 0; i < Sections->size(); i++)
	{
		for (DWORD k = 0; k < Sections->at(i).CountSubSections; k++)
		{
			FSXBGLSubSection SubSection20;
			FSXBGLSubSection16 SubSection16;
			if (Sections->at(i).SubSectionsSize == 1)
			{

				BGLXFile->Read(&SubSection16, Sections->at(i).OffsetSubSections + k* sizeof(SubSection16), sizeof(SubSection16));
				SubSection20.CountRecordsSubSection = SubSection16.CountRecordsSubSection;
				SubSection20.OffsetRecords = SubSection16.OffsetRecords;
				SubSection20.QUMIDA = SubSection16.QUMIDA;
				SubSection20.QUMIDB = 0;
				SubSection20.SizeRecords = SubSection16.SizeRecords;
				BGLSubSections->push_back(SubSection20);

			}
			else
			{
				BGLXFile->Read(&SubSection20, Sections->at(i).OffsetSubSections + k * sizeof(SubSection20), sizeof(SubSection20));
				BGLSubSections->push_back(SubSection20);

			}
		}
	}
	if (BGLSubSections->size() == 0)
	{
		delete BGLSubSections;
		BGLSubSections = NULL;
	}
	return BGLSubSections;
}

std::vector<BGLXData::FSXBGLSubSection16>* BGLXData::GetSubSection16(std::vector<FSXBGLSection>* Sections)
{
	return nullptr;
}

std::string BGLXData::DecodeICAO(DWORD CodedICAO)
{
	if (CodedICAO)
	{
		CodedICAO = CodedICAO >> 5;
	}
	std::string ICAO;
	char oneCodedChar;
	char CodedChars[10] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0' , '\0' , '\0' , '\0' };
	int i = 0;
	while (CodedICAO > 37)
	{
		oneCodedChar = CodedICAO % 38;
		CodedChars[i] = oneCodedChar;
		i++;
		CodedICAO = (CodedICAO - oneCodedChar) / 38;
		if (CodedICAO < 38)
		{
			oneCodedChar = (char)CodedICAO;
			CodedChars[i] = oneCodedChar;
		}
	}

	// The first coded char in the list is the last one computed in the while loop 

	for (int j = i; j >= 0; j--)
	{
		if (CodedChars[j] == 0)
		{
			return ICAO;
		}
		else if (CodedChars[j] > 1 && CodedChars[j] < 12)
		{
			// digit 0-9
			ICAO = ICAO + char('0' + (CodedChars[j] - 2));
		}
		else
		{
			// letter
			ICAO = ICAO + char('A' + (CodedChars[j] - 12));
		}
	}
	return ICAO;
}


double BGLXData::DecodeLat(DWORD lat)
{

	return  90.0 - lat * (180.0 / (2 * 0x10000000));
}

double BGLXData::DecodeLon(DWORD lon)
{

	return lon * (360.0 / (3 * 0x10000000)) - 180.0;
}
