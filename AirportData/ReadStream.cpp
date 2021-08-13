#include "stdafx.h"
#include "ReadStream.h"


ReadStream::ReadStream(std::string filename)
{
	f.open(filename,std::ios::in|std::ios::binary);
	if (!f.is_open()) 
		std::wcout << "Файл не открыт";
}


ReadStream::~ReadStream()
{
	if (f.is_open())
		f.close();
}


HRESULT ReadStream::Read(PVOID64 buffer, INT64 seek, INT64 count)
{
	if (!f.is_open()) 
		return -1;
	f.seekg(seek, std::ios::beg);
	if (f.eof()) return 0;
	//char *buffer = new char[count];
	try
	{
		f.read((char*)buffer, count);
		int tell = f.tellg();
		if ((tell - seek) != count)
		{
			return -1;
		}
		if (tell < 0x0)
		{
			return -1;
		}
	}
	catch(...)
	{
		std::wcout << "ERROR";
	}
	//return buffer;
	return NOERROR;
}

ReadStreamText::ReadStreamText(std::string filename)
{
	f.open(filename, std::ios::in);
	if (!f.is_open())
		std::wcout << "Файл не открыт";
}


ReadStreamText::~ReadStreamText()
{
	if (f.is_open())
		f.close();
}


HRESULT ReadStreamText::Read(std::string* buffer, INT64 line)
{
	if (!f.is_open())
		return -1;
	//f.seekg(seek, std::ios::beg);
	if (f.eof()) return f.eof();
	//char *buffer = new char[count];
	try
	{
		char str[1024];
		f >> str;
		
		*buffer = std::string(str);
	}
	catch (...)
	{
		std::wcout << "ERROR";
		return -1;
	}
	//return buffer;
	return NOERROR;
}

HRESULT ReadStreamText::ReadLn(std::string* buffer, INT64 line)
{
	if (!f.is_open())
		return -1;
	//f.seekg(seek, std::ios::beg);
	if (f.eof()) return f.eof();
	//char *buffer = new char[count];
	try
	{
		char str[1024];
		//f >> str;
		std::getline(f, *buffer);

		//*buffer = std::string(str);
	}
	catch (...)
	{
		std::wcout << "ERROR";
		return -1;
	}
	//return buffer;
	return NOERROR;
}