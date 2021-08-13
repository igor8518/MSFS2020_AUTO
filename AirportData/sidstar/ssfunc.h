#pragma once
#include <memory>
#include <string.h>
#include <vector>

enum TYPEROUTE
{
  NONE,
  FIX1,
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
  RNW1,
  TRKVECTORS,

};

enum ALT
{
  AT,
  ATORABOVE1,
  ATORBELOW1,
  ABOVEBELOW,
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

class Sid{
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

SidStar* gSidStar;



void* sidStar(std::vector<FixPoint>* fixes, std::vector<Sid>* sids, std::vector<Sid>* stars, std::vector<Sid>* approaches) {
  SidStar* sidstar = new SidStar();
  if (fixes) {
    sidstar->efixes = new std::vector<FixPoint>();
    sidstar->efixes->insert(sidstar->efixes->end(), fixes->begin(), fixes->end());
    free(fixes);
  }
  if (sids) {
    sidstar->esids = new std::vector<Sid>();
    sidstar->esids->insert(sidstar->esids->end(), sids->begin(), sids->end());
    free(sids);
  }
  if (stars) {
    sidstar->estars = new std::vector<Sid>();
    sidstar->estars->insert(sidstar->estars->end(), stars->begin(), stars->end());
    free(stars);
  }
  if (approaches) {
    sidstar->eapproaches = new std::vector<Sid>();
    sidstar->eapproaches->insert(sidstar->eapproaches->end(), approaches->begin(), approaches->end());
    free(approaches);
  }
  gSidStar = sidstar;
  return sidstar;
}



void* fullStar(char* starname, RoutePoint* starpoint, std::vector<RoutePoint>* starpoints, std::vector<RnwSid1>* transpoints, std::vector<std::string>* rnws) {
  //FULLSTAR: STAR NAME ROUTEPOINT SIDPOINTS TRANSITIONARRAY SIDSTARRNWARRAY
  Sid* star;
  
  star = new Sid();
  star->name = starname;
  free(starname);
  if (rnws) {
    star->ernw = new std::vector<std::string>();
    star->ernw->insert(star->ernw->end(), rnws->begin(), rnws->end());
    free(rnws);
  }
  if (transpoints) {
    star->etrans = new std::vector<RnwSid1>();
  }
  star->ernws = new std::vector<RnwSid1>();
  star->ernws->push_back({ new char(), new std::vector<RoutePoint>() });
  star->ernws->at(0).name = "";
  if (starpoint) {
    star->ernws->at(0).eroute->push_back(*starpoint);
    free(starpoint);
  }
  if (starpoints) {
    star->ernws->at(0).eroute->insert(star->ernws->at(0).eroute->end(), starpoints->begin(), starpoints->end());
    free(starpoints);
  }
  if (transpoints) {
    star->etrans->insert(star->etrans->end(), transpoints->begin(), transpoints->end());
    free(transpoints);
  }

  return star;
}



void* fullApproach(char* approachname, std::vector<RoutePoint>* approachpoints1, char* rnw, std::vector<RoutePoint>* approachpoints2, std::vector<RnwSid1>* transpoints) {
  //FULLAPPROACH: APPROACH NAME SIDPOINTS RNW RNWNUM APPROACHPOINTS TRANSITIONARRAY
  Sid* approach;
  approach = new Sid();
  approach->name = approachname;
  free(approachname);
  if (transpoints) {
    approach->etrans = new std::vector<RnwSid1>();
  }
  approach->ernws = new std::vector<RnwSid1>();
  approach->ernws->push_back({ new char(), new std::vector<RoutePoint>() });
  approach->ernws->at(0).name = "";
  if (approachpoints1) {
    approach->ernws->at(0).eroute->insert(approach->ernws->at(0).eroute->end(), approachpoints1->begin(), approachpoints1->end());
    free(approachpoints1);
  }
  if (rnw != "") {
    approach->ernws->at(0).eroute->push_back({ RNW1, AT, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, rnw});
    approach->ernws->at(0).name = rnw;
    approach->ernw = new std::vector<std::string>();
    approach->ernw->push_back(rnw);
    free(rnw);
  }
  if (approachpoints2) {
    approach->ernws->at(0).eroute->insert(approach->ernws->at(0).eroute->end(), approachpoints2->begin(), approachpoints2->end());
    free(approachpoints2);
  }
  if (transpoints) {
    approach->etrans->insert(approach->etrans->end(), transpoints->begin(), transpoints->end());
    free(transpoints);
  }

  return approach;
}



void* addSid(std::vector<Sid>* sids, Sid* sid) {
  if (!sids) {
    sids = new std::vector<Sid>();
  }
  sids->push_back(*sid);
  free(sid);
  return sids;
}

void* addStar(std::vector<Sid>* stars, Sid* star) {
  if (!stars) {
    stars = new std::vector<Sid>();
  }
  stars->push_back(*star);
  free(star);
  return stars;
}

void* addApproach(std::vector<Sid>* approaches, Sid* approach) {
  if (!approaches) {
    approaches = new std::vector<Sid>();
  }
  approaches->push_back(*approach);
  free(approach);
  return approaches;
}


void* addTransition(std::vector<RnwSid1>* transpoints, RnwSid1* transpoint) {
  if (!transpoints) {
    transpoints = new std::vector<RnwSid1>();
  }
  transpoints->push_back(*transpoint);
  free(transpoint);
  return transpoints;
}


void* fullTransition(char* transitionname, std::vector<RoutePoint>* transpoints) {
  //FULLSID: RNWPOINT SIDSTARRNWARRAY ROUTEPOINT ROUTEARRAY	
  RnwSid1* trans;
  trans = new RnwSid1();
  trans->name = transitionname;
  free(transitionname);
    if (transpoints) {
      trans->eroute = new std::vector<RoutePoint>();
      trans->eroute->insert(trans->eroute->end(), transpoints->begin(), transpoints->end());
      free(transpoints);
    }

    return trans;
}



void* tt19(char* sidname, std::vector<RoutePoint>* sid1, Sid* sid2, std::vector<RnwSid1>* trans) {
  Sid* sid;
  if (!sid1) {
    sid = new Sid();
    sid->ernws = new std::vector<RnwSid1>();
    if (sid2->ernw) {
      sid->ernw = new std::vector<std::string>();
    }
    if (sid2->etrans || trans) {
      sid->etrans = new std::vector<RnwSid1>();
    }
    sid->name = sidname;
    free(sidname);
    if (sid2) {
      sid->ernws->insert(sid->ernws->end(), sid2->ernws->begin(), sid2->ernws->end());
      if (sid2->ernw) {
        sid->ernw->insert(sid->ernw->end(), sid2->ernw->begin(), sid2->ernw->end());
        free(sid2->ernw);
      }
      if (sid2->etrans) {
        sid->etrans->insert(sid->etrans->end(), sid2->etrans->begin(), sid2->etrans->end());
        free(sid2->etrans);
      }
      if (trans) {
        sid->etrans->insert(sid->etrans->end(), trans->begin(), trans->end());
        free(trans);
      }
      free(sid2->ernws);
      free(sid2);
    }
    return sid;
  }
  else {
    sid = new Sid();
    sid->ernws = new std::vector<RnwSid1>();
    if (sid2->ernw) {
      sid->ernw = new std::vector<std::string>();
    }
    if (sid2->etrans || trans) {
      sid->etrans = new std::vector<RnwSid1>();
    }
    sid->name = sidname;
    free(sidname);
    if (sid2) {
      sid->ernws->insert(sid->ernws->end(), sid2->ernws->begin(), sid2->ernws->end());
      if (sid2->ernw) {
        sid->ernw->insert(sid->ernw->end(), sid2->ernw->begin(), sid2->ernw->end());
        free(sid2->ernw);
      }
      if (sid2->etrans) {
        sid->etrans->insert(sid->etrans->end(), sid2->etrans->begin(), sid2->etrans->end());
        free(sid2->etrans);
      }
      if (trans) {
        sid->etrans->insert(sid->etrans->end(), trans->begin(), trans->end());
        free(trans);
      }
      free(sid2->ernws);
      free(sid2);
    }
    for (int i = 0; i < sid->ernws->size(); i++)
    {
      sid->ernws->at(i).eroute->insert(sid->ernws->at(i).eroute->end(), sid1->begin(), sid1->end());
    }
    return sid;
  }
  return 0;
}


void* addFullSid(Sid* sid1, Sid* sid2) {
  if (!sid1) {
    sid1 = new Sid();
    sid1->ernws = new std::vector<RnwSid1>();
    if (sid2->ernw) {
      sid1->ernw = new std::vector<std::string>();
    }
    if (sid2->etrans) {
      sid1->etrans = new std::vector<RnwSid1>();
    }
  }
  sid1->name = sid2->name;
  sid1->ernws->insert(sid1->ernws->end(), sid2->ernws->begin(), sid2->ernws->end());
  if (sid2->ernw) {
    sid1->ernw->insert(sid1->ernw->end(), sid2->ernw->begin(), sid2->ernw->end());
    free(sid2->ernw);
  }
  if (sid2->etrans) {
    sid1->etrans->insert(sid1->etrans->end(), sid2->etrans->begin(), sid2->etrans->end());
    free(sid2->etrans);
  }
  free(sid2->ernws);
  free(sid2);
  return sid1;
}

void* tt41(Sid* sid1, Sid* sid2) {
  if (sid2) {
    if (!sid1) {
      sid1 = new Sid();
      sid1->ernws = new std::vector<RnwSid1>();
      if (sid2->ernw) {
        sid1->ernw = new std::vector<std::string>();
      }
      if (sid2->etrans) {
        sid1->etrans = new std::vector<RnwSid1>();
      }
    }
    sid1->name = sid2->name;
    sid1->ernws->insert(sid1->ernws->end(), sid2->ernws->begin(), sid2->ernws->end());
    if (sid2->ernw) {
      sid1->ernw->insert(sid1->ernw->end(), sid2->ernw->begin(), sid2->ernw->end());
      free(sid2->ernw);
    }
    if (sid2->etrans) {
      sid1->etrans->insert(sid1->etrans->end(), sid2->etrans->begin(), sid2->etrans->end());
      free(sid2->etrans);
    }
    free(sid2->ernws);
    free(sid2);
  }
  return sid1;
}

/*void* tt19(Sid* sid1, Sid* sid2) {
  if (!sid1) {
    sid1 = new Sid();
    sid1->ernws = new std::vector<RnwSid1>();
    if (sid2->ernw) {
      sid1->ernw = new std::vector<std::string>();
    }
    if (sid2->etrans) {
      sid1->etrans = new std::vector<RnwSid1>();
    }
  }
  sid1->name = sid2->name;
  sid1->ernws->insert(sid1->ernws->end(), sid2->ernws->begin(), sid2->ernws->end());
  if (sid2->ernw) {
    sid1->ernw->insert(sid1->ernw->end(), sid2->ernw->begin(), sid2->ernw->end());
  }
  if (sid2->etrans) {
    sid1->etrans->insert(sid1->etrans->end(), sid2->etrans->begin(), sid2->etrans->end());
  }
  return sid1;
}*/

void* fullSid(char* rnw, std::vector<std::string>* rnws, RoutePoint* routepoint, std::vector<RoutePoint>* routepoints) {
  //FULLSID: RNWPOINT SIDSTARRNWARRAY ROUTEPOINT ROUTEARRAY	
  Sid* sid;
  sid = new Sid();
  std::string name = rnw;
  if (rnws) {
    sid->ernw = new std::vector<std::string>();
    
    if (rnw) {
      sid->ernw->push_back(rnw);
      free(rnw);
    }
    sid->ernw->insert(sid->ernw->end(), rnws->begin(), rnws->end());
    free(rnws);
    name = "";
  
  }

  sid->ernws = new std::vector<RnwSid1>();
  sid->ernws->push_back({ new char(), new std::vector<RoutePoint>() });
  sid->ernws->at(0).name = name;
  if (routepoint) {
    sid->ernws->at(0).eroute->push_back(*routepoint);
    free(routepoint);
  }
  if (routepoints) {
    sid->ernws->at(0).eroute->insert(sid->ernws->at(0).eroute->end(), routepoints->begin(), routepoints->end());
    free(routepoints);
  }
  
  return sid;
}

void* addSSRnw(std::vector<std::string>* ssrnws, char* rnw) {
  if (!ssrnws) {
    ssrnws = new std::vector<std::string>();
  }
  ssrnws->push_back(std::string(rnw));
  free(rnw);
  return ssrnws;
}

void* fullFix(char* name, char* nsdig, double lat1, double lat2, char* wedig, double lon1, double lon2) {
  double lat, lon;
  FixPoint* fixpoint = new FixPoint();
  fixpoint->name = std::string(name);
  free(name);
  lat = lat1 + (lat2 / 60.0);
  if (strcmp(nsdig, "S") == 0) {
    lat = -lat;
  }
  lon = lon1 + (lon2 / 60.0);
  if (strcmp(wedig, "W") == 0) {
    lon = -lon;
  }
  fixpoint->lat = lat;
  fixpoint->lon = lon;
  return fixpoint;
};

void* addFix(std::vector<FixPoint>* fixarray, FixPoint* fixpoint) {
  if (!fixarray) {
    fixarray = new std::vector<FixPoint>();
  }
  fixarray->push_back(*fixpoint);
  free(fixpoint);
  return fixarray;
}

void* routePointFix(int turndirect, bool direct, bool overfly, char* namefix, double alt1, double alt2, double speed) {
  RoutePoint* routepoint = new RoutePoint();
  routepoint->alt = alt1;
  routepoint->alt2 = alt2;
  if (turndirect == 0 && !overfly) {
    routepoint->type = FIX1;
  }
  else if (turndirect == 1 && !overfly) {
    routepoint->type = TURNLEFT;
  }
  else if (turndirect == 2 && !overfly) {
    routepoint->type = TURNRIGHT;
  }
  else if (turndirect == 0 && overfly) {
    routepoint->type = FIXOVERFY;
  }
  else if (turndirect == 1 && overfly) {
    routepoint->type = TURNLEFTOVERFLY;
  }
  else if (turndirect == 2 && overfly) {
    routepoint->type = TURNRIGHTOVERFLY;
  }
  if (alt1 == alt2) {
    routepoint->alttype = AT;
  }
  else if (alt1 == 0.0 && alt2 != 0.0) {
    routepoint->alttype = ATORABOVE1;
  }
  else if (alt1 != 0.0 && alt2 == 0.0) {
    routepoint->alttype = ATORBELOW1;
  }
  else if (alt1 != 0.0 && alt2 != 0.0) {
    routepoint->alttype = ABOVEBELOW;
  }
  if (namefix != "") {
    routepoint->name = namefix;
    free(namefix);
  }
  else {
    routepoint->name = "";
  }
  routepoint->speed = speed;
  return routepoint;
  //ROUTEPOINTFIX: TURNDIRECT DIRECTBOOL FIX OVERFLYBOOL NAME VAL SPEEDVAL
}

void* routePoint(double trkval, bool vectors, double untilval, double interceprval, int fromto, char* fixname, double speed) {
  RoutePoint* routepoint = new RoutePoint();
  if (fixname != "") {
    routepoint->dist = untilval;
  }
  else {
    routepoint->alt = untilval;
  }
  if (trkval >= 0) {
    routepoint->hdg = trkval;
  }
  else {
    routepoint->trk = -trkval;
  };
  if (fixname != "") {
    routepoint->name = fixname;
    free(fixname);
  }
  else {
    routepoint->name = "";
  }
  routepoint->radial = interceprval;
  routepoint->speed = speed;
  if (!vectors && interceprval > 0.0 && fromto == 0 && trkval >= 0) {
    routepoint->type = HDGINTERCEPTFROMFIX;
  }
  else if (!vectors && interceprval > 0.0 && fromto == 1 && trkval >= 0) {
    routepoint->type = HDGINTERCEPTTOFIX;
  }
  else if (!vectors && interceprval > 0.0 && fromto == 0 && trkval < 0) {
    routepoint->type = TRKINTERCEPTFROMFIX;
  }
  else if (!vectors && interceprval > 0.0 && fromto == 1 && trkval < 0) {
    routepoint->type = TRKINTERCEPTTOFIX;
  }
  else if (!vectors && interceprval == 0.0 && fromto == 0 && trkval < 0 && routepoint->dist > 0.0) {
    routepoint->type = TRKUNTILFROMFIX;
  }
  else if (!vectors && interceprval == 0.0 && trkval >= 0 && routepoint->dist == 0.0) {
    routepoint->type = HDGUNTIL;
  }
  else if (!vectors && interceprval == 0.0 && trkval < 0 && routepoint->dist == 0.0) {
    routepoint->type = TRKUNTIL;
  }
  else if (trkval < 0 && vectors) {
    routepoint->type = TRKVECTORS;
  }
  else if (trkval >= 0 && vectors) {
    routepoint->type = HDGVECTORS;
  }
  else {
    routepoint->type = NONE;
  }
  return routepoint;
}

void* holdAt(char* fixname, int directhead, double courseval, double alt, double speed, double legval) {
  RoutePoint* routepoint = new RoutePoint();
  routepoint->hdg = courseval;
  routepoint->alt = alt;
  routepoint->speed = speed;
  if (legval >= 0) {
    routepoint->dist = legval;
  }
  else {
    routepoint->legtime = -legval;
  }
  if (fixname != "") {
    routepoint->name = fixname;
    free(fixname);
  }
  else {
    routepoint->name = "";
  }
  routepoint->type = FIX1;
  return routepoint;
  //ROUTEPOINT: HOLDAT FIXPOINT DIRECTHEAD TURN INBOUNDCOURSE VAL ALTVAL SPEEDVAL LEGVAL	
}

void* addSidPoints(std::vector<RoutePoint>* routepoints, RoutePoint* routepoint) {
  if (!routepoints) {
    routepoints = new std::vector<RoutePoint>();
  }
  routepoints->push_back(*routepoint);
  free(routepoint);
  return routepoints;
}

void* addApproachPoints(std::vector<RoutePoint>* routepoints, RoutePoint* routepoint) {
  if (!routepoints) {
    routepoints = new std::vector<RoutePoint>();
  }
  routepoints->push_back(*routepoint);
  free(routepoint);
  return routepoints;
}