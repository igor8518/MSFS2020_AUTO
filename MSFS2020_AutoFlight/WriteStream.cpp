//#include "stdafx.h"
#include "WriteStream.h"


WriteStream::WriteStream(std::string filename)
{
	f.open(filename);
	if (!f.is_open())
		std::cout << "Файл не открыт";
}


WriteStream::~WriteStream()
{
	
	if (!f.is_open())
		std::cout << "Файл не открыт";
	else
		f.close();
}

HRESULT WriteStream::Write(LPWSTR buffer, int count)
{
	if (!f.is_open())
		return -1;
	try
	{
		f.write((char*)buffer, count);
	}
	catch (...)
	{
		std::wcout << "ERROR";
	}
	//return buffer;
	return NOERROR;
}

HRESULT WriteStream::WriteLn(std::string buffer)
{
	if (!f.is_open())
		return -1;
	//f.seekg(seek, std::ios::beg);
	//if (f.eof()) return f.eof();
	//char *buffer = new char[count];
	try
	{
		char str[1024];
		//f >> str;
		f << buffer;

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
