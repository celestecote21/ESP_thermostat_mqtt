

#include "Clim.hpp"


Clim::Clim(IRFujitsuAC* ac){
  _ac = ac;
  _ac->begin();
  delay(500);
  _ac->setModel(ARREB1E);
  _ac->setSwing(kFujitsuAcSwingBoth);
  _ac->setMode(kFujitsuAcModeCool);
  _ac->setFanSpeed(kFujitsuAcFanHigh);
  _ac->setTemp(27);  // 24C

   active = false;
   start_hour = 25; // hours 25 veut dire desactiver
   finish_hour = 25;
   temp_demander = 0.0;
   day = 10; // day inactif
}

void Clim::check(float temp_actuel, int heure_actu, int day_actu){
	 if(temp_demander == 0.0){ // si c'est un arret
	 	this->toggle(false);
		return;
	 }
	if(day == day_actu){ // si c'est le bon jour
		if(heure_actu >= start_hour && heure_actu <= finish_hour){ // si on est dans le bonne horaire
			float diff_t = 0.0;
			if(temp_demander > 0.0){ // calcul de la difference de temp suivant la temperature demander
				diff_t = temp_actuel - temp_demander;
			    	_ac->setMode(kFujitsuAcModeHeat); // on met la clim en mode chaud
		       	}else{// froid, temperature demander negative
				diff_t = temp_actuel + temp_demander;
			    	diff_t = -diff_t;
				_ac->setMode(kFujitsuAcModeCool); // on met la clim en mode froid
			}
			  if(diff_t < -0.5){
				this->toggle(true); // activation lorsque que la diff bonne
				delay(50);
			  }else if(diff_t > 0.5){ // desactivation lorsque l'on a depasser
				this->toggle(false);
				delay(50);
			  }
		}else{ // si on est pas dans le bonne horaire on eteint
			this->toggle(false);
			return;
		}
	}else{
		this->toggle(false);
		return;
	}

}
void Clim::toggle(boolean activation){
	if(activation && !active){ // si on veut activer et qu'elle est pas activer
		_ac->setCmd(kFujitsuAcCmdTurnOn);
    		_ac->send();
		active = true;
	}else if(!activation && active){
		_ac->setCmd(kFujitsuAcCmdTurnOff);
    		_ac->send();
		active = false;
	}
}
int Clim::get_start_hour(void){
	return start_hour;  
}
int Clim::get_finish_hour(void){
	return finish_hour;  
}
int Clim::get_day(void){
	return day;
  
}

void Clim::set_clim(ordre_t ordre){
	start_hour = ordre.start_hour;
	finish_hour = ordre.finish_hour;
	temp_demander = ordre.temp_set;
	day = ordre.day;
}

void Clim::arret(void){
	start_hour = 25;
	finish_hour = 25;
	temp_demander = 0.0; 
	day = 10;
}

float Clim::get_temp_demander(void){
	return temp_demander;
}

etat_t Clim::get_status(void){
	etat_t etat;
	ordre_t ordre;
	ordre.start_hour = this->get_start_hour();
	ordre.finish_hour = this->get_finish_hour();
	ordre.temp_set = this->get_temp_demander();
	ordre.day = this->get_day();
	etat.last_ordre = ordre;
	etat.isActif = this->active;
	etat.isArret = (this->get_temp_demander() == 0.0);
	return etat;
}
