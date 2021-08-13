#pragma once
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

