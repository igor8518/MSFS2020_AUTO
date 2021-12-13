#pragma once
#include <cstdio>
#include <fstream>
#include <tchar.h>
#include <iostream>
#include <string>
#include <cstring>
#include <Windows.h>
class WriteStream
{
private:
	//Приватное поле - файл
	std::ofstream f;
public:
	WriteStream(std::string filename);
	~WriteStream();
	HRESULT Write(LPWSTR buffer, int count);
	HRESULT WriteLn(std::string buffer);
	
};

