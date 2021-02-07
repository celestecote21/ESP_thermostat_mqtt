# thermostat connecte utilisant un ESP32 et le protocole MQTT

## fonctionnement

Peut contrôler un chauffage au sol grace à un relai et contrôler une clim réversible par infrarouge.
Ce contrôle grace a l'application android [thermco](https://github.com/celestecote21/thermCo).
Utilisation du protocole MQTT grace à la librairie [PubSubClient](https://pubsubclient.knolleary.net/), il reçoit un JSON lui donnant les horaires et le jour d'activation ainsi que la température qu'il doit faire dans la pièce.
Le relai peut être programmé pour une horaire et un jour spécifique a une certaine température (programmation de la température à 18h quand on rentre chez soi pour que le chauffage ne reste pas allumer toute la journée par exemple).
Le contrôle de la clim est lui aussi programmé suivant les mêmes parametres(jour, heure, temperature), et est controlé indépendament du relais.
La température de la pièce est envoyé au broker pour être affiché sur l'application, la dernière programmation s'affiche aussi sur l'application.
Il est possible d'installer un écran ou d'ajouter une carte fille pour de nouvelle fonction, grace au header nommer "screen_header" sur le schemas élèctronique.

## Utilisation

Le systeme n'est pas autonome (Sur la branche [version_1](https://github.com/celestecote21/thermostat_connecter_arduino/tree/version_1) il y a une version avec un serveur http sur l'ESP32). Pour fonctionner il a besoin d'un brocker priver (tel que mosquitto) sur votre réseau local ou un brocker public.
Fournissez ssid et mot de passe wifi ainsi que les informations de connexion pour votre brocker, par defaut il va subscrire a Maison/thermostat/set pour recevoir les instructions, et va publier toutes les minutes la temperature de la piece sur Maison/thermostat/get
L'envoie d'instruction se fait par JSON dans ce format:

	{"day":0,"start":18,"finish":20,"set":20,"type":1,"default":"12.0"}

* day: le jour de la semaine de la programmation
* start: l'heure de départ de la mise en route
* finish: l'heure de fin/d'arret
* set: la température qu'il doit faire dans la pièce
* type: si c'est la geothermie ou la clim
* default: la température de base toujours active pour le relais

Pour le infra-rouge une excellente librairie est disponible [ici](https://github.com/crankyoldgit/IRremoteESP8266) répertoriant de nombreuses marques et modèles.
Des mises à jours peuvent être faites à distance en envoyant une url pointant vers un fichier binaire, utilisez apache par exemple.

## Design PCB

design cree a l'aide d'easyeda, vous pouvez tout retrouver [ici](https://easyeda.com/celeste21/thermco).
\
![Schematic_thermco](Schematic_thermco.png)
\
![pcb](pcb_thermco.png)
