#ifndef GEO_HEADER
#define GEO_HEADER
#include "Arduino.h"
#include "utilities.hpp"


class Geo{
	private:
		int _pin;
		boolean active;
		int _start_hour;
		int _finish_hour;
		float _temp_demander;
		float _temp_default;
		int _day;
	public:
		Geo(int pin);
		void check(float temp_actuel, int heure, int day_actu);
		void default_check(float temp_actuel);
		void toggle(boolean activation);
		void set_geo(ordre_t ordre, float temp_default);
		void arret(void);
		int get_start_hour(void);
		int get_finish_hour(void);
		int get_day(void);
		float get_temp_demander(void);
		etat_t get_status(void);
};


#endif

