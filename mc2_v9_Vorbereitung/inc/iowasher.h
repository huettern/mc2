/*
 * iowasher.h
 *
 *  Created on: May 1, 2017
 *      Author: noah
 */

#ifndef INC_IOWASHER_H_
#define INC_IOWASHER_H_


// Eingabetasten:
enum Pushbuttons {OPEN_DOOR=1, START_STOP=2};
enum Pushbuttons GetPushbuttons();

// Waschprogramm-Vorwahlschalter:
enum WashMode {PREWASH=1, COLDWASH=2, HOTWASH=4, WRING=8 };
enum WashMode GetWashMode();

// Sensoren:
int GetTemperature(); // Wassertemperatur in °C
int GetWaterLevel(); // Wasserstand in % (0-100)
int IsDoorOpen(); // Türzustand: 1 - offen, 0 - geschlossen

// Aktoren:
void SetWater(int on); // Wassereinlaufventil: 1 - ein, 0 - aus
void SetHeating(int on); // Heizung: 1 - ein, 0 - aus
void SetOpenDoor(int on); // Türöffner: 1 - ein, 0 - off
void SetAuslauf(int on); // Wasserauslauf: 1 – offen (ablaufen lassen) , 0 – geschlossen (kein Auslauf)
void PutSoap(int on); // Seifendispenser: 1 – öffnen (Seife zugeben), 0 – geschlossen
enum DrumSpeed {HALT, SLOW_LEFT, SLOW_RIGHT, WRINGING};
void SetDrumSpeed(enum DrumSpeed speed); // Geschwindigkeit der Waschtrommel


#endif /* INC_IOWASHER_H_ */
