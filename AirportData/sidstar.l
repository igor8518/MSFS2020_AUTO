%option noyywrap

%{
#include <windows.h>
#include <winnt.h>
#include <array>
#include <vector>
#include <stdio.h>
//#include "RobotTokens.h"
#include <math.h>
//#include <sidstar_tokens.h>
//#include <stdafx.h>
#include "sidstar_1.tab.h"

// Flex использует макрос YY_DECL как основу объявления функции, сканирующей следующий токен.
// По умолчанию значение макроса YY_DECL равно `int yylex()`
// Но мы назовём функцию ScanToken.
#define YY_DECL int ScanToken()
#define fileno _fileno
#define isatty _isatty
int sign;
float lat, lon;		


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
} pfix;
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
} proute;
struct srnwsid
{
	std::string name;
	std::vector<sroute>* eroute;
	
} prnwsid;
struct ssid
{
	std::string name;
	std::vector<srnwsid>* ernws;
	std::vector<srnwsid>* etrans;
	std::vector<std::string>* ernw;
} psid;
struct ssidstar
{
	std::vector<sfix>* efixes;
	std::vector<ssid>* esids;
	std::vector<ssid>* estars;
	std::vector<ssid>* eapproaches;
} sidstar2; 
std::vector<srnwsid>* PointVector;
std::vector<sroute>* vRoute = NULL;
std::vector<sfix>* vFix = NULL;
enum MODE
{
	ESIDS,
	ESTARS,
	EAPPROACHES,
};
int mode = 0;
std::string stemp;
%}
%x fixes fix latlon ns ew degreeslat degreeslon
%x rnws rnw
%x sids sid sidtrans sidrun
%x croute fixroute routealt fixspeed trkroute hdgroute routeradial routeradialfix routeuntil trkrouteradial stars star starrnw rnwroute approach approaches trkuntilfrom
%%

\/\/.*\n 
"GATES".*\n 
"GATE".*\n 
"ENDGATES".*\n 
[ \n\t]
"FIXES" {
  sidstar2.efixes = new std::vector<sfix>(); 
  BEGIN(fixes); 
}
	<fixes>FIX {BEGIN(fix);}
		
		<fix>LATLON {BEGIN(latlon);}
			<latlon>[N|S] {if (yytext == "N") {sign = 0;} else {sign = 1;}; BEGIN(ns);}
				<ns>[0-9]+ {lat = atof(yytext);BEGIN(degreeslat);}
					<degreeslat>[0-9]+[\.]?[0-9]* {float t; yylval.string = yytext; lat = lat + t/60.0; if(!sign){lat=-lat;}; pfix.lat = lat; BEGIN(latlon);}
			<latlon>[E|W] {if (yytext == "E") {sign = 0;} else {sign = 1;}; BEGIN(ew);}
				<ew>[0-9]+ {lon = atof(yytext);BEGIN(degreeslon);}
					<degreeslon>[0-9]+[\.]?[0-9]* {float t; t = atof(yytext); lon = lon + t/60.0; if(!sign){lon=-lon;}; pfix.lon = lon; printf(" lat: %f, lon: %f\n",lat,lon); sidstar2.efixes->push_back(pfix); BEGIN(fixes);}
		<fix>[A-Za-z0-9]+ {pfix.name = std::string(yytext); printf("Fix point: %s with ",yytext);}
<fixes>"ENDFIXES" {BEGIN(0);}

RNWS {printf("Used runways: "); BEGIN(rnws);}
	<rnws>RNW {BEGIN(rnw);}
	<rnw>[0-9A-Za-z]+ {printf("%s, ", yytext); BEGIN(rnws);}
		<rnws>ENDRNWS {BEGIN(0); printf("\b\b\n");};
. {printf("\nWith out FIX %s\n",yytext);}

SIDS {mode = ESIDS; sidstar2.esids = new std::vector<ssid>(); printf("SIDs: "); BEGIN(sids);psid = {"", NULL, NULL, NULL};}
	<sids,sid>SID {if(psid.name != "") {sidstar2.esids->push_back(psid);}; psid = {"", NULL, NULL, NULL}; BEGIN(sid);}
	<sid>ENDSIDS {/*PointVector->push_back(prnwsid); */prnwsid = {"", NULL}; sidstar2.esids->push_back(psid); printf("END"); BEGIN(0);}
	<sid>RNW {
		if (PointVector != NULL)
		{
			//PointVector->push_back(prnwsid); 
		}
		if(psid.ernws == NULL)
		{
			psid.ernws = new std::vector<srnwsid>();
		}; 
		PointVector = psid.ernws; 
		printf("for runway "); 
		BEGIN(sidrun);
	}
	<sid,star,approach>TRANSITION {if (PointVector != NULL){/*PointVector->push_back(prnwsid);*/}; if(psid.etrans == NULL){psid.etrans = new std::vector<srnwsid>();}; PointVector = psid.etrans; printf("with transition "); BEGIN(sidtrans);}
	<sid>[0-9A-Za-z\.]+ {psid.name = std::string(yytext); printf("SID: %s ", yytext);}
		<sidrun>[0-9A-Za-z\.]+ {prnwsid.name = std::string(yytext); stemp = prnwsid.name; prnwsid.eroute = new std::vector<sroute>(); proute = {0}; printf("%s", yytext); BEGIN(croute); }
		<sidtrans>[0-9A-Za-z\.]+ {prnwsid.name = std::string(yytext); prnwsid.eroute = new std::vector<sroute>(); proute = {0}; printf("%s", yytext); BEGIN(croute); }
STARS {mode = ESTARS; sidstar2.estars = new std::vector<ssid>(); printf("STARSs: "); BEGIN(stars);psid = {"", NULL, NULL, NULL};}	
	<stars,star>STAR {if(psid.name != "") {sidstar2.estars->push_back(psid);}; psid = {"", NULL, NULL, NULL};
		if(psid.ernws == NULL)
		{
			psid.ernws = new std::vector<srnwsid>();
		}; 
		PointVector = psid.ernws; 
		printf("for runway "); 
		BEGIN(sidrun);}
	<star>ENDSTARS {/*PointVector->push_back(prnwsid);*/ prnwsid = {"", NULL}; sidstar2.estars->push_back(psid); printf("END"); BEGIN(0);}
	<star>RNW { if (psid.ernw == NULL){psid.ernw = new std::vector<std::string>();}; BEGIN(starrnw); }
		<starrnw>[0-9A-Za-z\.]+ {psid.ernw->push_back(std::string(yytext)); BEGIN(star);};

APPROACHES {mode = EAPPROACHES; sidstar2.eapproaches = new std::vector<ssid>(); printf("APPROACHes: "); BEGIN(approaches);psid = {"", NULL, NULL, NULL};}	
	<approaches,approach>APPROACH {if(psid.name != "") {sidstar2.eapproaches->push_back(psid);}; psid = {"", NULL, NULL, NULL};
		if(psid.ernws == NULL)
		{
			psid.ernws = new std::vector<srnwsid>();
		}; 
		PointVector = psid.ernws; 
		printf("for runway "); 
		BEGIN(sidrun);}
	<approach>ENDAPPROACHES {/*PointVector->push_back(prnwsid); */prnwsid = {"", NULL}; sidstar2.eapproaches->push_back(psid); printf("END"); BEGIN(0);}
		


<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial,rnwroute>FIX {if(proute.type != NONE) {prnwsid.eroute->push_back(proute);};proute = {0}; proute.type = FIX; BEGIN(fixroute);}
<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial,rnwroute>RNW {if(proute.type != NONE) {prnwsid.eroute->push_back(proute);};proute = {0}; proute.type = RNW; BEGIN(rnwroute);}
<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial,rnwroute>TRK {if(proute.type != NONE) {prnwsid.eroute->push_back(proute);};proute = {0}; BEGIN(trkroute);}
<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial,rnwroute>HDG {if(proute.type != NONE) {prnwsid.eroute->push_back(proute);};proute = {0}; BEGIN(hdgroute);}
<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial,rnwroute>TURN[ ]LEFT[ ]DIRECT[ ]FIX {if(proute.type != NONE) {prnwsid.eroute->push_back(proute);};proute = {0}; proute.type = TURNLEFT; BEGIN(fixroute);}
<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial,rnwroute>TURN[ ]RIGHT[ ]DIRECT[ ]FIX {if(proute.type != NONE) {prnwsid.eroute->push_back(proute);};proute = {0}; proute.type = TURNRIGHT; BEGIN(fixroute);}
<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial,rnwroute>TURN[ ]LEFT[ ]DIRECT[ ]FIX[ ]OVERFLY {if(proute.type != NONE) {prnwsid.eroute->push_back(proute);};proute = {0}; proute.type = TURNLEFTOVERFLY; BEGIN(fixroute);}
<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial,rnwroute>TURN[ ]RIGHT[ ]DIRECT[ ]FIX[ ]OVERFLY {if(proute.type != NONE) {prnwsid.eroute->push_back(proute);};proute = {0}; proute.type = TURNRIGHTOVERFLY; BEGIN(fixroute);}
<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial,rnwroute>\n {
	if(proute.type != NONE) 
	{
		prnwsid.eroute->push_back(proute);
	};
	PointVector->push_back(prnwsid);
	prnwsid = {"", NULL}; 
	proute = {0}; 
	if (mode == ESIDS)
	{
		BEGIN(sid);
	}
	if (mode == ESTARS)
	{
		psid.name = stemp;
		BEGIN(star);
	}
	if (mode == EAPPROACHES)
	{
		psid.name = stemp;
		BEGIN(approach);
	}
}

<rnwroute>[0-9A-Za-z]+ {
	proute.name = std::string(yytext); 
	if (psid.ernw == NULL)
	{
		psid.ernw = new std::vector<std::string>();
	}; 
	psid.ernw->push_back(std::string(yytext)); 
}


<fixroute>OVERFLY {proute.type = FIXOVERFY; }
<fixroute>[0-9A-Za-z]+ {proute.name = std::string(yytext); BEGIN(routealt);}
<routealt>[0-9]+ {
	if (proute.alt != 0)
	{
		float t = atoi(yytext);
		if (t > proute.alt)
		{
			proute.alt2 = proute.alt;
			proute.alt = t;
		}
		else
		{
			proute.alt2 = t;
		};
	}
	else
	{
		proute.alt = atoi(yytext); 
	};
}
<routealt>[0-9]+\.[0-9]+ {proute.dist = atof(yytext); BEGIN(trkuntilfrom);}
<routealt>AT[ ]OR[ ]ABOVE {
	if (proute.alttype == ATORBELOW)
	{
		proute.alttype = ABOVEBELOW;
	}
	else
	{
		proute.alttype = ATORABOVE;
	}
	}
<routealt>AT[ ]OR[ ]BELOW {
	if (proute.alttype == ATORABOVE)
	{
		proute.alttype = ABOVEBELOW;
	}
	else
	{
		proute.alttype = ATORBELOW;
	}
	}
<routealt>SPEED {BEGIN(fixspeed); }
<fixspeed>[0-9]+ {proute.speed = atoi(yytext); }

<trkuntilfrom>FROM[ ]FIX {proute.type = TRKUNTILFROMFIX;BEGIN(routeradialfix);}

<trkroute>INTERCEPT[ ]RADIAL { BEGIN(routeradial);}
<trkroute>UNTIL {proute.type = TRKUNTIL; BEGIN(routealt); }
<trkrouteradial>TO[ ]FIX {proute.type = TRKINTERCEPTTOFIX;BEGIN(routeradialfix);}
<trkrouteradial>FROM[ ]FIX {proute.type = TRKINTERCEPTFROMFIX;BEGIN(routeradialfix);}
<routeradialfix>[0-9A-Za-z]+ {proute.name = std::string(yytext); BEGIN(routealt);}
<trkroute>[0-9]+ {proute.trk = atoi(yytext); }

<hdgroute>INTERCEPT[ ]RADIAL { BEGIN(routeradial);}
<hdgroute>VECTORS {proute.type = HDGVECTORS; BEGIN(routealt);}
<routeradial>TO[ ]FIX {proute.type = HDGINTERCEPTTOFIX;BEGIN(routeradialfix);}
<routeradial>FROM[ ]FIX {proute.type = HDGINTERCEPTFROMFIX;BEGIN(routeradialfix);}
<routeradial>[0-9]+ {proute.radial = atoi(yytext);}; 
<hdgroute>[0-9]+ {proute.hdg = atoi(yytext); }
<croute,fixroute,routealt,fixspeed,routeradial,routeradialfix,routeuntil,trkrouteradial>[A-Z] {/*MessageBox(0,L"ERR",LPWSTR(yytext),MB_OK);*/}


%%