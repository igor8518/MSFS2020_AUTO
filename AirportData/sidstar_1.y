%{
    #include <stdio.h>
	#include <string>
    void yyerror(char *s) {
      fprintf (stderr, "%s\n", s);
    }
	extern int ScanToken();
	int yylex() {return ScanToken(); };
	int countfixes = 0;
	int countrnws = 0;
	
%}



%token RIGHT TURN N260 N261 N262 APPROACHES APPROACH N265 N266 N267 N268 N269
%token FIXES
%token N271 ENDFIXES SIDS SID1 ENDSIDS STARS STAR
%token ENDSTARS RNWS RNW 
%token <string> RNWNUM 
%token ENDRNWS
%token <string> NAMEROUTE  
%token FIX 
%token <flo> VAL 
%token N286 N287 FROM N289 ATORABOVE ATORBELOW N292 N293 LATLON 
%token <sym> DIGIN DIGIE
%token N297 N298 TO INTERCEPT UNTIL N302 N303 N304 N305 N306 N307 N308 N309 DIRECT
%token RADIAL SPEED TRK

%union {
	char* string;
	double flo;
	char sym;
}
%yacc
%fixed-output-files
%locations
%pure-parser

%%



EVALUATE1: ALLFIXES ALLRNWS 
FIXPOINT: FIX NAMEROUTE LATLON DIGIN VAL VAL DIGIE VAL VAL { 
	double lat = $5 + $6 / 60;
	if ($4 == 'S') {
		lat = -lat;
	}
	double lon = $8 + $9 / 60;
	if ($4 == 'W') {
		lon = -lon;
	}
	countfixes++;
	printf("FIX %s\tlat: %f\tlon: %f\n", $2, lat, lon); 
} 
FIXPOINT: FIXPOINT FIXPOINT
ALLFIXES: FIXES FIXPOINT ENDFIXES { printf("Total fixes: %d\n\n", countfixes); }


RNWPOINT: RNW RNWNUM {
	countrnws++;
	printf("RNW %s\n", $2);
}
RNWPOINT: RNWPOINT RNWPOINT
ALLRNWS: RNWS RNWPOINT ENDRNWS { printf("Total rnws: %d\n\n", countrnws); }





%%