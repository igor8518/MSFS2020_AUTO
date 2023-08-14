#pragma once
#include "SimConnect.h"
#define _USE_MATH_DEFINES 
#include <math.h>
#include <vector>
#include <iostream>
#include <string>
#include "AirportData.h"
namespace PMDG_TEST
{
	
	enum ALTOPT {
		NONEALT,
		FIXALT,
		ALTORBELOW,
		ALTORABOVE,
		CRUISEALT
	};
	/*std::string ParkType[14] = {"", "RAMP_GA", "RAMP_GA_SMALL", "RAMP_GA_MEDIUM", "RAMP_GA_LARGE", "RAMP_CARGO", "RAMP_MIL_CARGO", "RAMP_MIL_COMBAT", "GATE_SMALL", "GATE_MEDIUM", "GATE_HEAVY", "DOCK_GA", "FUEL", "VEHICLES"};
	std::string ParkName[38] = { "NONE", "PARKING", "N_PARKING", "NE_PARKING", "E_PARKING", "SE_PARKING", "S_PARKING", "SW_PARKING", "W_PARKING", "NW_PARKING", "GATE", "DOCK",
	  "GATE_A", "GATE_B", "GATE_C", "GATE_D", "GATE_E", "GATE_F", "GATE_G", "GATE_H", "GATE_I", "GATE_J", "GATE_K", "GATE_L", "GATE_M", "GATE_N", "GATE_O", "GATE_P", "GATE_Q", "GATE_R", "GATE_S", "GATE_T",
	  "GATE_U", "GATE_V", "GATE_W", "GATE_X", "GATE_Y", "GATE_Z" };*/
	static class SIMMATH {
	public:
		
		static struct DSHEH {
			SIMCONNECT_DATA_LATLONALT Slla;
			SIMCONNECT_DATA_LATLONALT Ella;
			int fixAlt = NONEALT;
			double D;
			double SH;
			double EH;
			double H;
			double speedLimit;
			int typePath;
			std::string nameEndPoint;
			double TargetAlt = 0.0;
			double DistToAlt = 0.0;
			double Speed = 0.0;
			double Dist = 0.0;
		};
		static double GetFixDA(double da, double angle) {
			if (((angle < 0) && (da > 0)) || ((angle > 0) && (da < 0))) {
				return -da;
			}
			return da;
		}
		static void DOrtoKM(DSHEH* dsheh) {
			double SLatitude = dsheh->Slla.Latitude / 180 * M_PI;
			double SLongitude = dsheh->Slla.Longitude / 180 * M_PI;
			double ELatitude = dsheh->Ella.Latitude / 180 * M_PI;
			double ELongitude = dsheh->Ella.Longitude / 180 * M_PI;
			double dO = (acos(sin(SLatitude)*sin(ELatitude) + cos(SLatitude)*cos(ELatitude)*cos(ELongitude - SLongitude))) * 180 / M_PI*111.12;
			if ((SLatitude == ELatitude) && (SLongitude == ELongitude)) {
				dsheh->D = 0;
			}
			else {
				dsheh->D = dO;
			}
			dsheh->SH = M_PI_2 - atan(cos(SLatitude)*tan(ELatitude) / sin(ELongitude - SLongitude) - sin(SLatitude) / tan(ELongitude - SLongitude));
			dsheh->EH = M_PI_2 - atan(sin(ELatitude) / tan(ELongitude - SLongitude) - cos(ELatitude)*tan(SLatitude) / sin(ELongitude - SLongitude));
			dsheh->SH = dsheh->SH * 180 / M_PI;
			dsheh->EH = dsheh->EH * 180 / M_PI;
			if ((dsheh->Ella.Longitude - dsheh->Slla.Longitude) == 0) {
				dsheh->SH = 0;
				dsheh->EH = 0;
			}
			if (((dsheh->Ella.Longitude - dsheh->Slla.Longitude) < 0)) {
				dsheh->SH = dsheh->SH + 180;
				dsheh->EH = dsheh->EH + 180;
			}
			else if (((dsheh->Ella.Latitude - dsheh->Slla.Latitude) == 0) && ((dsheh->Ella.Longitude - dsheh->Slla.Longitude) < 0)) {
				dsheh->SH = dsheh->SH + 180;
				dsheh->EH = dsheh->EH + 180;
			}
			else if (((dsheh->Ella.Longitude - dsheh->Slla.Longitude) == 0) && ((dsheh->Ella.Latitude - dsheh->Slla.Latitude) < 0)) {
				dsheh->SH = dsheh->SH + 180;
				dsheh->EH = dsheh->EH + 180;
			}
			if (dsheh->SH > 360) {
				dsheh->SH = dsheh->SH - 360;
			}
			if (dsheh->SH < -360) {
				dsheh->SH = dsheh->SH + 360;
			}
			if (dsheh->EH > 360) {
				dsheh->EH = dsheh->EH - 360;
			}
			if (dsheh->EH < -360) {
				dsheh->EH = dsheh->EH + 360;
			}
		}
		static double Constrain180(double angle) {
			angle = Constrain360(angle + 180.0);
			return angle - 180.0;
		}
		static double DecodeLat(DWORD lat) {
			return  90.0 - lat * (180.0 / (2 * 0x10000000));
		}
		static DWORD EncodeLat(double lat) {
			return  double(90.0 - lat) / double(180.0 / double(2 * 0x10000000));
		}
		static double DecodeLon(DWORD lon) {
			return lon * (360.0 / (3 * 0x10000000)) - 180.0;
		}
		static DWORD EncodeLon(double lon) {
			return double(lon + 180.0) / double(360.0 / double(3 * 0x10000000));
		}
		static double GetAngle360(double angle) {
			double Tangle;
			Tangle = angle;
			while (Tangle < 0) {
				Tangle = Tangle + 360;
			}
			while (Tangle >= 360) {
				Tangle = Tangle - 360;
			}
			return Tangle;
		}

		static double Constrain360(double angle) {
			angle = fmod(angle, 360.0);
			while (angle < 0.0) {
				angle += 360.0;
			}
			return angle;
		}

		static SIMCONNECT_DATA_LATLONALT GetDALatLon(double lat, double lon, double ang, double dist) {
			dist = dist / 111.12;
			SIMCONNECT_DATA_LATLONALT latlon;
			double newlat = sin(lat * M_PI / 180)*cos(dist * M_PI / 180) + cos(lat * M_PI / 180)*sin(dist * M_PI / 180)*cos(ang * M_PI / 180);
			newlat = asin(newlat) * 180 / M_PI;
			double newlon = sin(dist * M_PI / 180)*sin(ang * M_PI / 180) / (cos(lat * M_PI / 180)*cos(dist * M_PI / 180) - sin(lat * M_PI / 180)*sin(dist * M_PI / 180)*cos(ang * M_PI / 180));
			newlon = lon + ((atan(newlon) * 180 / M_PI));
			latlon.Latitude = newlat;
			latlon.Longitude = newlon;
			return latlon;
		}

		static BOOL PreparePaths(std::vector<TaxiwayPoints>* tpoints, std::vector<TaxiwayPaths>* tpaths, std::vector<TaxiwayNames>* tNames, std::vector<TaxiwayParks>* tParks, std::vector<RunwayPaths>* rPth){
			DWORD n = tpoints->size();
			DWORD points = tpoints->size();
			DWORD m = tpaths->size();
			for (int i = 0; i < m; i++) {
				if ((tpaths->at(i).Type & 0xf) == 0x3) {
					//tpaths->at(i).IndexEndPoint = tpaths->at(i).IndexEndPoint + tpoints->size();
					if (tpaths->at(i).End == 24) {
						std::cout << 1;
					}
					tpaths->at(i).End = tpaths->at(i).End + tpoints->size();
					/*DWORD PI = tParks->at(i).TaxiParkInfo;
					DWORD PIi = (PI >> 12) & 0xfff;
					DWORD PIt = (PI >> 8) & 0xf;
					DWORD PIn = PI & 0x1f;
					tpaths->at(i).TaxiNameIndex = tNames->size();*/
					//tNames->push_back(std::string(ParkType[PIt] + " " + std::to_string(PIi) + " " + ParkName[PIn]));
				}
				tpaths->push_back(tpaths->at(i));
				//tpaths->at(tpaths->size() - 1).IndexEndPoint = tpaths->at(i).IndexStartPoint;
				//tpaths->at(tpaths->size() - 1).IndexStartPoint = tpaths->at(i).IndexEndPoint & 0xfff;
				tpaths->at(tpaths->size() - 1).End = tpaths->at(i).IndexStartPoint;
				tpaths->at(tpaths->size() - 1).IndexStartPoint = tpaths->at(i).End;

				
			}
			for (int i = 0; i < tParks->size(); i++) {
				tpoints->push_back(tpoints->at(0));

				

				tpoints->at(tpoints->size() - 1).Lat = tParks->at(i).Lat;
				tpoints->at(tpoints->size() - 1).Lon = tParks->at(i).Lon;
			}
			/*for (int i = 0; i < rPth->size(); i++) {
				DWORD run = rPth->at(i).Path.at(rPth->at(i).Path.size() - 1);
				DWORD end = rPth->at(i).Path.at(rPth->at(i).Path.size() - 2);
				tpoints->push_back(tpoints->at(run));
				tpaths->push_back(tpaths->at(0));
				tpaths->at(tpaths->size() - 1).IndexStartPoint = end;
				tpaths->at(tpaths->size() - 1).IndexEndPoint = tpoints->size() - 1;
				tpaths->at(tpaths->size() - 1).Type = 14;

				run = rPth->at(i).Path.at(rPth->at(i).Path.size() - 2);
				end = rPth->at(i).Path.at(rPth->at(i).Path.size() - 1);
				tpoints->push_back(tpoints->at(run));
				tpaths->push_back(tpaths->at(0));
				tpaths->at(tpaths->size() - 1).IndexStartPoint = end;
				tpaths->at(tpaths->size() - 1).IndexEndPoint = tpoints->size() - 1;
				tpaths->at(tpaths->size() - 1).Type = 14;
			}*/
			return true;
		}
	

		static std::vector<TPath> solve(std::vector<TaxiwayPoints>* tpoints, std::vector<TaxiwayPaths>* tpaths, std::vector<TaxiwayNames>* tNames, std::vector<TaxiwayParks>* tParks, WORD start, WORD end, WORD run, std::string RunwayName, double heading, INT64* dR) {
			std::string ds[] = { "", "L", "R", "C", "W", "A", "B" };
			double INF = 100000000.0;
			std::vector<TPath> rPath;
			long v = start;
			long t = end;
			int na = 0;




			DWORD m = tpaths->size();
			end = tpoints->size() - 1;
			DWORD n = tpoints->size();
			std::vector<double>* d = new std::vector<double>(n, INF);
			d->at(v) = 0.0;
			std::vector<int> p(n, -1);
			std::vector<std::string> names(n, "");
			std::vector<double> a(n, 180.0);
			a[v] = heading;
			DSHEH* dsheh = new DSHEH();
			int x;
			std::vector<TaxiwayPaths> Falls;
			for (;;) {
				bool any = false;
				x = -1;
				for (int j = 0; j < m; ++j) {
					//int typeNew = (tpaths->at(j).Type & 0xff);
					int type = (tpaths->at(j).Type & 0xf);
					if (

						type == 0x0B ||
						type == 0x0C ||
						type == 0x0E ||
						type == 0x06 ||
						type == 0x09 ||
						//type == 0x01 ||
						//type == 0x02 ||
						//type == 0x03 ||
						//type == 0x04 ||
						//type == 0x07 || //??
						type == 0x00 )
						
					{
						if ((tpaths->at(j).IndexStartPoint) >= d->size()) {
							tpaths->at(j).IndexStartPoint = tpaths->at(j).IndexStartPoint & 0xfff;
							//Falls.push_back(tpaths->at(j));
						}
						/*if ((tpaths->at(j).IndexEndPoint) >= d->size()) {
							tpaths->at(j).IndexEndPoint = tpaths->at(j).IndexEndPoint & 0xfff;
							Falls.push_back(tpaths->at(j));
						}*/
						if ((tpaths->at(j).End) >= d->size()) {
							tpaths->at(j).End = tpaths->at(j).End & 0xfff;
							//Falls.push_back(tpaths->at(j));
						}
					}
					else {
						na = type;
					}
					
					/*if (((tpaths->at(j).Type & 0xf) == 0x0) ||
						((tpaths->at(j).Type & 0xf) == 0x6) || 
						((tpaths->at(j).Type & 0xf) >= 0x8))
					{
						Falls.push_back(tpaths->at(j));
						continue;
					}*/
					if ((tpaths->at(j).IndexStartPoint) >= d->size()) {
						tpaths->at(j).IndexStartPoint = tpaths->at(j).IndexStartPoint & 0xff;
						Falls.push_back(tpaths->at(j));
						//continue;
					}
					if ((tpaths->at(j).End) >= tpoints->size()) {
						tpaths->at(j).End = tpaths->at(j).End & 0xff;
						Falls.push_back(tpaths->at(j));
						//continue;
					}
					if ((d->at(tpaths->at(j).IndexStartPoint) < INF) && (((tpaths->at(j).Type & 0xf) == 1) || ((tpaths->at(j).Type & 0xf) == 2) || ((tpaths->at(j).Type & 0xf) == 3) || ((tpaths->at(j).Type & 0xf) == 4) || ((tpaths->at(j).Type & 0xf) == 14))) {
						dsheh->Slla.Latitude = DecodeLat(tpoints->at(tpaths->at(j).IndexStartPoint).Lat);
						dsheh->Slla.Longitude = DecodeLon(tpoints->at(tpaths->at(j).IndexStartPoint).Lon);
						/*dsheh->Ella.Latitude = DecodeLat(tpoints->at(tpaths->at(j).IndexEndPoint).Lat);
						dsheh->Ella.Longitude = DecodeLon(tpoints->at(tpaths->at(j).IndexEndPoint).Lon);*/
						dsheh->Ella.Latitude = DecodeLat(tpoints->at(tpaths->at(j).End).Lat);
						dsheh->Ella.Longitude = DecodeLon(tpoints->at(tpaths->at(j).End).Lon);
						DOrtoKM(dsheh);
						if ((tpaths->at(j).Type & 0xf) == 2) {
							dsheh->D = dsheh->D * 1000000;
						}
						if ((tpaths->at(j).Type & 0xf) == 3) {
							dsheh->D = dsheh->D * 100000;
						}
						if (d->at(tpaths->at(j).IndexStartPoint) < 0) {
							dsheh->D = int(INF / 10) - dsheh->D;
						}
						double ra;
						int CountRa = 1;
						ra = dsheh->EH - a[tpaths->at(j).IndexStartPoint];
						if (ra > 180) {
							ra = ra - 360;
						}
						else if (ra < -180) {
							ra = ra + 360;
						}
						ra = abs(ra);
						double DComm = 0.050;
						DWORD lastPoint = tpaths->at(j).IndexStartPoint;
						for (int cur = tpaths->at(j).IndexStartPoint; cur != -1; cur = p[cur]) {
							PMDG_TEST::SIMMATH::DSHEH de = {
							de.Slla.Latitude = DecodeLat(tpoints->at(lastPoint).Lat),
							de.Slla.Longitude = DecodeLon(tpoints->at(lastPoint).Lon), 0.0,
							de.Ella.Latitude = DecodeLat(tpoints->at(cur).Lat),
							de.Ella.Longitude = DecodeLon(tpoints->at(cur).Lon), 0.0 };
							DOrtoKM(&de);
							DComm = DComm - de.D;
							if (DComm > 0.0) {
								if (de.D != 0.0) {
									double ra2 = a[lastPoint] - a[cur];
									if (ra2 > 180) {
										ra2 = ra2 - 360;
									}
									else if (ra2 < -180) {
										ra2 = ra2 + 360;
									}
									CountRa = CountRa + 1;
									ra = ra + abs(ra2);

								}
								lastPoint = cur;
							}
							else {
								break;
							}
						}
						ra = ra / CountRa;
						double RRA;
						RRA = abs(pow(abs(ra), 3)) / 100.0;
						//if (d->at(tpaths->at(j).IndexEndPoint) > (d->at(tpaths->at(j).IndexStartPoint) + dsheh->D + RRA)) {
							//if ((tpaths->at(j).IndexEndPoint) == 511) {
						if (d->at(tpaths->at(j).End) > (d->at(tpaths->at(j).IndexStartPoint) + dsheh->D + RRA)) {
							if ((tpaths->at(j).End) == 511) {
								std::cout << 0;
							}
							//d->at(tpaths->at(j).IndexEndPoint) = max(-INF, d->at(tpaths->at(j).IndexStartPoint) + dsheh->D + RRA);
							//a[tpaths->at(j).IndexEndPoint] = dsheh->EH;
							//p[tpaths->at(j).IndexEndPoint] = tpaths->at(j).IndexStartPoint;
							//x = tpaths->at(j).IndexEndPoint;
							d->at(tpaths->at(j).End) = max(-INF, d->at(tpaths->at(j).IndexStartPoint) + dsheh->D + RRA);
							a[tpaths->at(j).End] = dsheh->EH;
							p[tpaths->at(j).End] = tpaths->at(j).IndexStartPoint;
							x = tpaths->at(j).End;
							if ((tpaths->at(j).Type & 0xf) == 2) {
								std::string R1 = "";
								if ((tpaths->at(j).TaxiNameIndex) < 10) {
									R1 = "0" + std::to_string(tpaths->at(j).TaxiNameIndex) + ds[tpaths->at(j).IndexEndPoint >> 12];
								}
								else {
									R1 = std::to_string(tpaths->at(j).TaxiNameIndex) + ds[tpaths->at(j).IndexEndPoint >> 12];
								}
								//names[tpaths->at(j).IndexEndPoint] = R1;
								names[tpaths->at(j).End] = R1;
							}
							else if ((tpaths->at(j).Type & 0xf) == 14) {
								//names[tpaths->at(j).IndexEndPoint] = RunwayName;
								names[tpaths->at(j).End] = RunwayName;
							}
							else {
								//names[tpaths->at(j).IndexEndPoint] = tNames->at(tpaths->at(j).TaxiNameIndex).TaxiName;
								names[tpaths->at(j).End] = tNames->at(tpaths->at(j).TaxiNameIndex).TaxiName;
							}
							any = true;
						}
					}
				}
				if (!any) {
					break;
				}
			}
			delete dsheh;
			dsheh = NULL;
			int z = 0;
			if (x == -1) {
				std::cout << "No negative cycle from " << v;
			}
			else {
				int y = x;
				for (int i = 0; i < n; ++i) {
					if (y >= 0) {
						z = y;
						y = p[y];
					}
					else {
						std::cout << "!!! ";
					}
				}
				std::vector<int> path;
				for (int cur = y; ; cur = p[cur]) {
					path.push_back(cur);
					if (cur == y && path.size() > 1)  break;
				}
				reverse(path.begin(), path.end());
				std::cout << "Negative cycle: ";
				for (size_t i = 0; i < path.size(); ++i)
					std::cout << path[i] << ' ';
			}
			if (d->at(t) >= INF) {
				std::cout << std::endl << v << "->" << t << "=" << "Not";
			}
			else {
				std::cout << std::endl << v << "->" << t << "=" << d->at(t);
			}
			if (d->at(t) == INF) {
				std::cout << std::endl << "No path from " << v << " to " << t << ".";
			}
			else {
				std::vector<int> path;
				for (int cur = t; cur != -1; cur = p[cur]) {
					path.push_back(cur);
				}
				reverse(path.begin(), path.end());
				std::cout << std::endl << "Path from " << v << " to " << t << ": ";
				for (int i = 0; i < path.size(); ++i) {
					std::cout << path[i] << ' ';
					TPath sPath;
					sPath.index = i;
					sPath.name = names.at(path[i]);
					sPath.Lat = DecodeLat(tpoints->at(path[i]).Lat);
					sPath.Lon = DecodeLon(tpoints->at(path[i]).Lon);
					sPath.Type = 0;
					sPath.InRunway = tpoints->at(path[i]).Flag;
					rPath.push_back(sPath);
				}
				PMDG_TEST::SIMMATH::DSHEH dsheh = {
					DecodeLat(tpoints->at(path[0]).Lat),
					DecodeLon(tpoints->at(path[0]).Lon), 0.0,
					DecodeLat(tpoints->at(path[1]).Lat),
					DecodeLon(tpoints->at(path[1]).Lon), 0.0
				};
				PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
				double RAngle = dsheh.EH - heading;
				if (RAngle > 180) {
					RAngle = RAngle - 360;
				}
				else if (RAngle < -180) {
					RAngle = RAngle + 360;
				}
				if (path.size() > 0) {
					PMDG_TEST::SIMMATH::DSHEH dsheh = {
						DecodeLat(tpoints->at(path[0]).Lat),
						DecodeLon(tpoints->at(path[0]).Lon), 0.0,
						DecodeLat(tpoints->at(path[1]).Lat),
						DecodeLon(tpoints->at(path[1]).Lon), 0.0
					};
					PMDG_TEST::SIMMATH::DOrtoKM(&dsheh);
					double RAngle = dsheh.EH - heading;
					if (RAngle > 180) {
						RAngle = RAngle - 360;
					}
					else if (RAngle < -180) {
						RAngle = RAngle + 360;
					}
					if (abs(RAngle) > (90.0)) {
						int i = 1;
						rPath[0].Type = 11;
						while (true) {
							PMDG_TEST::SIMMATH::DSHEH dshehS = {
								DecodeLat(tpoints->at(path[i - 1]).Lat),
								DecodeLon(tpoints->at(path[i - 1]).Lon), 0.0,
								DecodeLat(tpoints->at(path[i]).Lat),
								DecodeLon(tpoints->at(path[i]).Lon), 0.0
							};
							PMDG_TEST::SIMMATH::DOrtoKM(&dshehS);
							double RA = 360.0;
							int index = tpaths->size();
							for (int j = 0; j < tpaths->size(); j++) {
								if ((tpaths->at(j).IndexStartPoint == path[i]) &&
									((tpaths->at(j).Type & 0xf) != 0x3) &&
									//((tpaths->at(j).IndexEndPoint) != path[i - 1]) &&
									//((tpaths->at(j).IndexEndPoint) != path[i + 1])) {
									((tpaths->at(j).End) != path[i - 1]) &&
										((tpaths->at(j).End) != path[i + 1])) {
									PMDG_TEST::SIMMATH::DSHEH dshehE = {
										DecodeLat(tpoints->at(tpaths->at(j).IndexStartPoint).Lat),
										DecodeLon(tpoints->at(tpaths->at(j).IndexStartPoint).Lon), 0.0,
										//DecodeLat(tpoints->at((tpaths->at(j).IndexEndPoint)).Lat),
										//DecodeLon(tpoints->at((tpaths->at(j).IndexEndPoint)).Lon), 0.0
										DecodeLat(tpoints->at((tpaths->at(j).End)).Lat),
										DecodeLon(tpoints->at((tpaths->at(j).End)).Lon), 0.0
									};
									PMDG_TEST::SIMMATH::DOrtoKM(&dshehE);
									double RAngle = dshehS.EH - dshehE.EH;
									if (RAngle > 180) {
										RAngle = RAngle - 360;
									}
									else if (RAngle < -180) {
										RAngle = RAngle + 360;
									}
									if (abs(RAngle) < (105.0)) {
										if (abs(RAngle) < RA) {
											RA = abs(RAngle);
											index = j;
										}
									}
								}
							}

							PMDG_TEST::SIMMATH::DSHEH dsheh;
							if (index != tpaths->size()) {
								rPath[i].Type = 11;
								TPath sPath;
								sPath.index = path[i];
								sPath.name = names.at(path[i]);
								//sPath.Lat = DecodeLat(tpoints->at((tpaths->at(index).IndexEndPoint)).Lat);
								//sPath.Lon = DecodeLon(tpoints->at((tpaths->at(index).IndexEndPoint)).Lon);
								sPath.Lat = DecodeLat(tpoints->at((tpaths->at(index).End)).Lat);
								sPath.Lon = DecodeLon(tpoints->at((tpaths->at(index).End)).Lon);
								sPath.Type = 11;
								sPath.InRunway = tpoints->at((tpaths->at(index).End)).Flag;
								rPath.insert(rPath.begin() + i + 1, sPath);
								break;
							}
							else {
								rPath[i].Type = 11;
								i++;
							}
						}
					}
				}
			}
			TPath sPath;
			sPath.index = rPath.size();
			sPath.name = RunwayName;
			sPath.Lat = DecodeLat(tpoints->at(run).Lat);
			sPath.Lon = DecodeLon(tpoints->at(run).Lon);
			sPath.InRunway = tpoints->at(run).Flag;
			sPath.Type = 14;
			rPath.push_back(sPath);
		
			*dR = (INT64)d;
			return rPath;
		}

		static double GetPathLength(std::vector<TPath>* p) {
			double D = 0;
			for (int i = 1; i < p->size() - 1; i++) {
				PMDG_TEST::SIMMATH::DSHEH dsheh = { 0 };
				dsheh.Slla = { p->at(i - 1).Lat, p->at(i - 1).Lon, 0 };
				dsheh.Ella = { p->at(i).Lat, p->at(i).Lon, 0 };
				DOrtoKM(&dsheh);
				D = D + dsheh.D;
			}
			return D;
		}

		static double GetRWAngle(std::vector<TPath>* p) {
			double D = 0;
			
	
			PMDG_TEST::SIMMATH::DSHEH dsheh = { 0 };
			dsheh.Ella = { p->at(p->size() - 1).Lat, p->at(p->size() - 1).Lon, 0 };
			dsheh.Slla = { p->at(p->size() - 2).Lat, p->at(p->size() - 2).Lon, 0 };
			DOrtoKM(&dsheh);
			D = dsheh.SH;

			return D;
		}
		static double GetRWLength(std::vector<TPath>* p) {
			double D = 0;
			PMDG_TEST::SIMMATH::DSHEH dsheh = { 0 };
			dsheh.Ella = { p->at(p->size() - 1).Lat, p->at(p->size() - 1).Lon, 0 };
			dsheh.Slla = { p->at(p->size() - 2).Lat, p->at(p->size() - 2).Lon, 0 };
			DOrtoKM(&dsheh);
			D = dsheh.D;
			return D;
		}



		static std::vector<TPath> solveGate(std::vector<TaxiwayPoints>* tpoints, std::vector<TaxiwayPaths>* tpaths, std::vector<TaxiwayNames>* tNames, std::vector<TaxiwayParks>* tParks, WORD start, WORD end, double heading) {
			std::string ds[] = { "", "L", "R", "C", "W", "A", "B" };
			double INF = 100000000.0;
			std::vector<TPath> rPath;
			long v = start;
			long t = end + tpoints->size() - tParks->size();
			DWORD m = tpaths->size();
			DWORD n = tpoints->size();
			std::vector<double> d(n, INF);
			d[v] = 0.0;
			std::vector<int> p(n, -1);
			std::vector<std::string> names(n, "");
			std::vector<double> a(n, 180.0);
			a[v] = heading;
			DSHEH* dsheh = new DSHEH();
			int x;
			for (int i = 0; i<m; ++i) {
				bool any = false;
				x = -1;
        for (int j = 0; j < m; ++j) {
          if ((d[tpaths->at(j).IndexStartPoint] < INF) && (((tpaths->at(j).Type & 0xf) == 1) || ((tpaths->at(j).Type & 0xf) == 2) || ((tpaths->at(j).Type & 0xf) == 3) || ((tpaths->at(j).Type & 0xf) == 4) || ((tpaths->at(j).Type & 0xf) == 14))) {
            dsheh->Slla.Latitude = DecodeLat(tpoints->at(tpaths->at(j).IndexStartPoint).Lat);
            dsheh->Slla.Longitude = DecodeLon(tpoints->at(tpaths->at(j).IndexStartPoint).Lon);
            //dsheh->Ella.Latitude = DecodeLat(tpoints->at(tpaths->at(j).IndexEndPoint & 0xfff).Lat);
            //dsheh->Ella.Longitude = DecodeLon(tpoints->at(tpaths->at(j).IndexEndPoint & 0xfff).Lon);
						dsheh->Ella.Latitude = DecodeLat(tpoints->at(tpaths->at(j).End).Lat);
						dsheh->Ella.Longitude = DecodeLon(tpoints->at(tpaths->at(j).End).Lon);
            if (tpaths->at(j).IndexStartPoint == 22) {
              std::cout << 1;
            }
            DOrtoKM(dsheh);
            if ((tpaths->at(j).Type & 0xf) == 2) {
              dsheh->D = dsheh->D * 1000000;
            }
            if ((tpaths->at(j).Type & 0xf) == 3) {
              dsheh->D = dsheh->D * 100000;
            }
            if (d[tpaths->at(j).IndexStartPoint] < 0) {
              dsheh->D = int(INF / 10) - dsheh->D;
            }
            double ra;
            int CountRa = 1;
            ra = dsheh->EH - a[tpaths->at(j).IndexStartPoint];
            if (ra > 180) {
              ra = ra - 360;
            }
            else if (ra < -180) {
              ra = ra + 360;
            }
            ra = abs(ra);
            double DComm = 0.050;
            DWORD lastPoint = tpaths->at(j).IndexStartPoint;
            for (int cur = tpaths->at(j).IndexStartPoint; cur != -1; cur = p[cur]) {
              PMDG_TEST::SIMMATH::DSHEH de;
              de.Slla.Latitude = DecodeLat(tpoints->at(lastPoint).Lat);
              de.Slla.Longitude = DecodeLon(tpoints->at(lastPoint).Lon);
              de.Ella.Latitude = DecodeLat(tpoints->at(cur).Lat);
              de.Ella.Longitude = DecodeLon(tpoints->at(cur).Lon);
              DOrtoKM(&de);
              DComm = DComm - de.D;
              if (DComm > 0.0) {
                if (de.D != 0.0) {
                  double ra2 = a[lastPoint] - a[cur];
                  if (ra2 > 180) {
                    ra2 = ra2 - 360;
                  }
                  else if (ra2 < -180) {
                    ra2 = ra2 + 360;
                  }
                  CountRa = CountRa + 1;
                  ra = ra + abs(ra2);
                }
                lastPoint = cur;
              }
              else {
                break;
              }
            }
            ra = ra / CountRa;
            double RRA;
            RRA = abs(pow(abs(ra), 5)) / 100000.0;
            //if (d[tpaths->at(j).IndexEndPoint & 0xfff] > (d[tpaths->at(j).IndexStartPoint] + dsheh->D + RRA)) {
              //d[tpaths->at(j).IndexEndPoint & 0xfff] = max(-INF, d[tpaths->at(j).IndexStartPoint] + dsheh->D + RRA);
              //a[tpaths->at(j).IndexEndPoint & 0xfff] = dsheh->EH;
              //p[tpaths->at(j).IndexEndPoint & 0xfff] = tpaths->at(j).IndexStartPoint;
              //x = tpaths->at(j).IndexEndPoint & 0xfff;
							if (d[tpaths->at(j).End] > (d[tpaths->at(j).IndexStartPoint] + dsheh->D + RRA)) {
								d[tpaths->at(j).End] = max(-INF, d[tpaths->at(j).IndexStartPoint] + dsheh->D + RRA);
								a[tpaths->at(j).End] = dsheh->EH;
								p[tpaths->at(j).End] = tpaths->at(j).IndexStartPoint;
								x = tpaths->at(j).End;
              if ((tpaths->at(j).Type & 0xf) == 2) {
                std::string R1 = "";
                if ((tpaths->at(j).TaxiNameIndex) < 10) {
                  R1 = "0" + std::to_string(tpaths->at(j).TaxiNameIndex) + ds[tpaths->at(j).IndexEndPoint >> 12];
                }
                else {
                  R1 = std::to_string(tpaths->at(j).TaxiNameIndex) + ds[tpaths->at(j).IndexEndPoint >> 12];
                }
                //names[tpaths->at(j).IndexEndPoint & 0xfff] = R1;
								names[tpaths->at(j).End] = R1;
              }
              else {
                //names[tpaths->at(j).IndexEndPoint & 0xfff] = tNames->at(tpaths->at(j).TaxiNameIndex).TaxiName;
								names[tpaths->at(j).End] = tNames->at(tpaths->at(j).TaxiNameIndex).TaxiName;
              }
              any = true;
            }
          }
        }
				if (!any)  break;
			}
			delete dsheh;
			dsheh = NULL;
			int z = 0;
      if (x == -1) {
        std::cout << "No negative cycle from " << v;
      }
			else {
				int y = x;
				for (int i = 0; i < n; ++i) {
					if (y >= 0) {
						z = y;
						y = p[y];
					}
					else {
						std::cout << "!!! ";
					}
				}
				std::vector<int> path;
				for (int cur = y; ; cur = p[cur]) {
					path.push_back(cur);
					if (cur == y && path.size() > 1)  break;
				}
				reverse(path.begin(), path.end());
				std::cout << "Negative cycle: ";
				for (size_t i = 0; i<path.size(); ++i)
					std::cout << path[i] << ' ';
			}
			if (d[t] >= INF) {
				std::cout << std::endl << v << "->" << t << "=" << "Not";
			}
			else {
				std::cout << std::endl << v << "->" << t << "=" << d[t];
			}
      if (d[t] == INF) {
        std::cout << std::endl << "No path from " << v << " to " << t << ".";
      }
			else {
				std::vector<int> path;
				for (int cur = t; cur != -1; cur = p[cur]) {
					path.push_back(cur);
				}
				reverse(path.begin(), path.end());
				std::cout << std::endl << "Path from " << v << " to " << t << ": ";
				for (int i = 0; i < path.size(); ++i) {
					std::cout << path[i] << ' ';
					TPath sPath;
					sPath.index = sPath.index = path[i];;
					sPath.name = names.at(path[i]);
					sPath.Lat = DecodeLat(tpoints->at(path[i]).Lat);
					sPath.Lon = DecodeLon(tpoints->at(path[i]).Lon);
					sPath.Type = 0;
					rPath.push_back(sPath);
				}
			}
			return rPath;
		}
	};
}
