#pragma once
class WriteStream
{
private:
	//��������� ���� - ����
	std::ofstream f;
public:
	WriteStream(std::string filename);
	~WriteStream();
	HRESULT Write(LPWSTR buffer, int count);
	HRESULT WriteLn(std::string buffer);
	
};

