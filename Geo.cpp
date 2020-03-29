#include "Geo.hpp"



Geo::Geo(int pin){
	_pin = pin;
	active = false;
	_start_hour = 25;
	_finish_hour = 25;
	_day = 10;
	_temp_demander = 0.0;
	_temp_default = 12.0;
}


void Geo::check(float temp_actuel, int heure, int day_actu){
#ifdef SERIAL_DEBUG
	Serial.printf("t_actu: %f t_mise: %f\n", temp_actuel, this->_temp_demander);
#endif
	if(_temp_demander == 0.0){ // si c'est un arret
		this->default_check(temp_actuel); // pour la clim au lieu d'y arreteer on verifier avec la temperature par defaut
		return;
	}
	
	if(_day == day_actu && heure >= _start_hour && heure <= _finish_hour){ // si c'est et le bon jour et la bonne heure
		float diff_t = temp_actuel - _temp_demander; //negatif si il fait trop froid
		if(diff_t < -0.5){
			this->toggle(true);
		}else if(diff_t > 0.5){
			this->toggle(false); // si il fait la temperature demander ou plus on arrete
		}
		// surtout pas d'execution d check default car il eteindrais quand c'est mis
	}else{
		this->default_check(temp_actuel); // comme on est pas dans les heures programmer in verifie le default
	}
}


void Geo::default_check(float temp_actuel){
	float diff_t = temp_actuel - _temp_default;

	if(diff_t < 0){
		this->toggle(true);
	}else if(diff_t > 1){
		this->toggle(false);
	}
} 

void Geo::toggle(boolean activation){
	if(activation && !active){
		digitalWrite(_pin, HIGH);
		active = true;
	}else if(!activation && active){
		digitalWrite(_pin, LOW);
		active = false;
	}
}

void Geo::set_geo(ordre_t ordre, float temp_default){
	this->_start_hour = ordre.start_hour;
	this->_finish_hour = ordre.finish_hour;
	this->_temp_demander = ordre.temp_set;
	this->_temp_default = temp_default;
	this->_day = ordre.day;
#ifdef SERIAL_DEBUG
	Serial.printf("\nt_mise: %f \n", ordre.temp_set);
#endif
}

void Geo::arret(void){
	this->_start_hour = 25;
	this->_finish_hour = 25;
	this->_temp_demander = 0.0;
	this->_day = 10;
}


int Geo::get_start_hour(void){
	return _start_hour;
}

int Geo::get_finish_hour(void){
	return _finish_hour;
}

int Geo::get_day(void){
	return _day;
}

float Geo::get_temp_demander(void){
	return _temp_demander;
}


etat_t Geo::get_status(void){
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
