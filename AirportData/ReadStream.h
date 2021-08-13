#pragma once
//#include "stdafx.h"
class ReadStream
{
private:
	//Приватное поле - файл
	std::ifstream f;
public:
	ReadStream(std::string filename);
	~ReadStream();
	HRESULT Read(PVOID64 buffer, INT64 seek, INT64 count);
};

class ReadStreamText
{
private:
	//Приватное поле - файл
	std::ifstream f;
public:
	ReadStreamText(std::string filename);
	~ReadStreamText();
	HRESULT Read(std::string * buffer, INT64 line);
	HRESULT ReadLn(std::string* buffer, INT64 line);
};

