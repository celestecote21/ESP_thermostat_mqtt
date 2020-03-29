#ifndef CLIM_HEADER
#define CLIM_HEADER

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Fujitsu.h>
#include "Arduino.h"
#include "utilities.hpp"

class Clim{
  private:
    IRFujitsuAC* _ac;
    boolean active;
    int start_hour;
    int finish_hour;
    float temp_demander;
    int day;
  public:
    Clim(IRFujitsuAC* ac);
    void check(float temp_actuel, int heure, int day_actu);
    void toggle(boolean activation);
    void set_clim(ordre_t ordre);
    void arret(void);
    int get_start_hour(void);
    int get_finish_hour(void);
    int get_day(void);
    float get_temp_demander(void);
    etat_t get_status(void);    
    
};

#endif
