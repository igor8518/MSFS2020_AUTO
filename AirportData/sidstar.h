#pragma once
#include <vector>
enum TYPEROUTE
{
	NONE,
	FIX,
	FIXOVERFY,
	HDGINTERCEPTTOFIX,
	HDGINTERCEPTFROMFIX,
	TRKINTERCEPTTOFIX,
	TRKINTERCEPTFROMFIX,
	TURNRIGHT,
	TURNLEFT,
	TURNRIGHTOVERFLY,
	TURNLEFTOVERFLY,
	TRKUNTIL,
	TRKUNTILFROMFIX,
	HDGUNTIL,
	HDGVECTORS,
	RNW,
	TRKVECTORS,

};
enum ALT
{
	AT,
	ATORABOVE,
	ATORBELOW,
	ABOVEBELOW,
};

struct sfix
{
	std::string name;
	float lat;
	float lon;
};
struct sroute
{
	int type;
	int alttype;
	float alt;
	float alt2;
	float speed;
	float radial;
	float trk;
	float hdg;
	float dist;
	float legtime;
	std::string name;
};
struct srnwsid
{
	std::string name;
	std::vector<sroute>* eroute;

};
struct ssid
{
	std::string name;
	//std::vector<srnwsid>* mroute;
	std::vector<srnwsid>* ernws;
	std::vector<srnwsid>* etrans;
	std::vector<std::string>* ernw;
};
struct ssidstar
{
	std::vector<sfix>* efixes;
	std::vector<ssid>* esids;
	std::vector<ssid>* estars;
	std::vector<ssid>* eapproaches;
};
enum MODE
{
	ESIDS,
	ESTARS,
	EAPPROACHES,
};


class FixPoint
{
public:
  std::string name;
  float lat;
  float lon;
};

class RoutePoint
{
public:
  TYPEROUTE type;
  ALT alttype;
  float alt;
  float alt2;
  float speed;
  float radial;
  float trk;
  float hdg;
  float dist;
  float legtime;
  std::string name;
};

class RnwSid1
{
public:
  std::string name;
  std::vector<RoutePoint>* eroute;

};

class Sid {
public:
  std::string name;
  //std::vector<srnwsid>* mroute;
  std::vector<RnwSid1>* ernws;
  std::vector<RnwSid1>* etrans;
  std::vector<std::string>* ernw;
};


class SidStar
{
public:
  std::vector<FixPoint>* efixes;
  std::vector<Sid>* esids;
  std::vector<Sid>* estars;
  std::vector<Sid>* eapproaches;
};

