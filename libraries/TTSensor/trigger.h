/************************************************************************************
 * 	
 * 	Name    : Trigger Trap trigger.h                        
 * 	Author  : Noah Shibley, NoMi Design nomidesign.net                       
 * 	Date    : July 10th 2011                                    
 * 	Version : 0.1                                              
 * 	Notes   :  The base class for all triggers. 
 *			   DO NOT EDIT - EDITING WILL CHANGE ALL SENSORS BEHAVIOR
 *			   ,inherit instead to a sub class and change there.            
 * 
 * 	 Copyright (c) 2011 NoMi Design All right reserved.
 * 
 *     
 *
 * 
 * 	This file is part of Trigger Trap.
 * 
 * 		    Trigger Trap is free software: you can redistribute it and/or modify
 * 		    it under the terms of the GNU General Public License as published by
 * 		    the Free Software Foundation, either version 3 of the License, or
 * 		    (at your option) any later version.
 * 
 * 		    Trigger Trap is distributed in the hope that it will be useful,
 * 		    but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 		    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		    GNU General Public License for more details.
 * 
 * 		    You should have received a copy of the GNU General Public License
 * 		    along with Trigger Trap.  If not, see <http://www.gnu.org/licenses/>.
 * 
 ***********************************************************************************/

#ifndef TRIGGER_H
#define TRIGGER_H

//#include "globals.h"
#include <WProgram.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <TTIRremote.h>

//Eeprom read/write Macros
#define eeprom_read_to(dst_p, eeprom_field, dst_size,blockOffset) eeprom_read_block(dst_p, (void *)(blockOffset+offsetof(__eeprom_data, eeprom_field)), MIN(dst_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define eeprom_read(dst, eeprom_field,blockOffset) eeprom_read_to(&dst, eeprom_field, sizeof(dst),blockOffset)
#define eeprom_write_from(src_p, eeprom_field, src_size,blockOffset) eeprom_write_block(src_p, (void *)(blockOffset+offsetof(__eeprom_data, eeprom_field)), MIN(src_size, sizeof((__eeprom_data*)0)->eeprom_field))
#define eeprom_write(src, eeprom_field,blockOffset) { typeof(src) x = src; eeprom_write_from(&x, eeprom_field, sizeof(x),blockOffset);}
#define MIN(x,y) ( x > y ? y : x )

//most triggers use these menu options. 
const int TRIG_THRESHOLD = 0;	 //Option menu option 0.
const int TRIG_DELAY = 1;  //Option menu option 1. 
const int TRIG_TYPE = 2; //Option menu option 2.  		

const int CAMERA_TRIGGER_A = 13;		// D13 = Digital out - Camera Trigger A
const int CAMERA_TRIGGER_B = 12;		// D12 = Digital out - Camera Trigger B


//#define SERIAL_DEBUG //remove if you don't want serial statements

class Trigger {

public:

/***********************************************************
 * 
 * shotCount
 *
 * returns how many shots where taken since the start button was activated. 
 * 
 ***********************************************************/
const int shotCount(){ return shotCounter_; }


/***********************************************************
 * 
 * sensorLevel
 *
 * returns the analog level of the sensor being used, this is virtual 
 * 
 ***********************************************************/
virtual int sensorLevel() { return 0; }

/***********************************************************
 * 
 * trigger
 *
 * if returns true, camera ready to trigger, this is virtual, override in sub class. 
 * 
 ***********************************************************/
virtual boolean trigger() = 0;

/***********************************************************
 * 
 * shutter
 *
 *  Take the picture, bring shutterA and/or B low
 * 
 ***********************************************************/
void shutter(boolean delayActive=true,boolean delayUnitMs=true);

/***********************************************************
 * 
 * select
 *
 * return which select option is active. 
 *   
 ***********************************************************/
const int select();

/***********************************************************
 * 
 * incSelect
 *
 * increment the select option that is active 
 *   
 ***********************************************************/
virtual void incSelect();

/***********************************************************
 * 
 * incSetting
 *
 * go to the next Select menu string, pass in a char array for printing
 * 
 ***********************************************************/
virtual void incSetting(char buffer[], int inc=1);

/***********************************************************
 * 
 * decSetting
 *
 * go to the previous Select menu string, pass in a char array for printing
 * 
 ***********************************************************/
virtual void decSetting(char buffer[], int dec=1);


/***********************************************************
 * 
 * getModeMenu
 *
 * get the current Mode menu string and place it in the char buffer for LCD or serial
 * 
 ***********************************************************/
virtual	void getModeMenu(char buffer[]);

/***********************************************************
 * 
 * getSelectMenu
 *
 * get the current select menu string and place it in the char buffer for LCD or serial
 * 
 ***********************************************************/
virtual void getSelectMenu(char buffer[]);

/***********************************************************
 * 
 * getActiveMessage
 *
 * get the current sensors LCD message to print during trap Active mode.
 * 
 ***********************************************************/
virtual void getActiveMessage(char buffer[]);

/***********************************************************
 * 
 * start
 *
 * set values that are called when start button is pressed 
 * 
 ***********************************************************/
virtual void start(unsigned long startTime);

/***********************************************************
 * 
 * stop
 *
 * return abortTrigger, tell the UI to stop the trigger action
 * 
 ***********************************************************/
virtual boolean stop();

/***********************************************************
 * 
 * IRShutter
 *
 * send shutter command via IR transmitter
 * 
 ***********************************************************/
void IRTransmit();

/***********************************************************
 * 
 * resetShutter
 *
 * this resets the shutter HIGH after being LOW for 10 ms. 
 * 
 ***********************************************************/
void resetShutter();

/***********************************************************
 * 
 * setShutters
 *
 * set which line will be used, Focus or Shutter 
 * 
 ***********************************************************/
void setShutters(boolean cameraA, boolean cameraB, boolean IRshutter, int shutterPulseTime); 

/***********************************************************
 * 
 * saveState
 *
 * save all settings values to eeprom
 * 
 ***********************************************************/
void saveState();

/***********************************************************
 * 
 * restoreState
 *
 * restore all trigger settings from eeprom
 * 
 ***********************************************************/
void restoreState();

/***********************************************************
 * 
 * initState
 *
 * reset eeprom to default values for all trigger settings 
 * 
 ***********************************************************/
void initState();

void setIndex(int index) {triggerIndex = index;}

protected: 
	
	
  int triggerIndex; //trigger list index number. Used with eeprom memory offset values 
  byte maxOptionMenu; //option menu highest menu number. set to 3 for 3 option bttn menus
  unsigned int optionValues[3];	//option menu settings values
  boolean triggerState_; //On or OFF, based on above or below the threshold
  byte select_; //trigger on START,STOP or CHANGE
  byte sensorLevel_; //incoming sensor value
  unsigned int shotCounter_; // current shot since pressing the start button
  byte sensorPin_; //the sensor pin
 
  boolean shutterStateA_; //status of shutter A, high or low. Goes low for small amount of time at shot
  boolean shutterStateB_; //status of shutter B, high or low. Goes low for small amount of time at shot
  boolean cameraA_;  //camera A on
  boolean cameraB_; //camera B on
  boolean IRShutter_; //use IR shutter?
  boolean shutterReady; //trigger is ready, take a picture
  boolean abortTrigger;  //stop the trigger action. example, all timelapse shots finished 
  
 int shutterPulseTime_; //the time shutter pin is low 

  unsigned long shutterDelay; //keep track of the time the shutter has been low
  //unsigned long focusDelay; //keep track of the time the focus has been low
  unsigned long delayCount; //when trigger is ready, start counting, till delay time is up
  unsigned long startBttnTime; //the time when the start button is pressed. 
  
	

   IRsend irsend;
	
	//values to save into eeprom
	struct __eeprom_data {
	  byte optionSelect; //select_
	  unsigned int optionVal[3]; //optionValues array
	};

/***********************************************************
 * 
 * high
 *
 * if trigger just went high, trigger the camera. (return true)
 * 
 ***********************************************************/
virtual boolean rise();
/***********************************************************
 * 
 * low
 *
 * if trigger just went low, trigger the camera. (return true)
 * 
 ***********************************************************/
virtual boolean fall();
/***********************************************************
 * 
 * change
 *
 * if trigger changed state (high/low), trigger the camera. (return true)
 * 
 ***********************************************************/
virtual boolean change();

/***********************************************************
 * 
 * setSelect
 *
 * Set the select option that is active, this will override the UI menu setting. 
 *   
 ***********************************************************/
void setSelect(int select);



/***********************************************************
 * 
 * option
 *
 * Return the value of a menu setting for this object. Ex, timelapse delta value
 * 
 ***********************************************************/
const unsigned int option(int menuOption);

/***********************************************************
 * 
 * setOption
 *
 * set the value of a menu option for this object. Ex. light threshold value.
 * 
 ***********************************************************/
void setOption(int menuOption,unsigned int value);

/***********************************************************
 * 
 * incOption
 *
 * increment the value of a menu option for this object +1. 
 * 
 ***********************************************************/
void incOption(int menuOption, unsigned int maxValue,int inc=1);

/***********************************************************
 * 
 * incOption
 *
 * decrement the value of a menu option for this object -1. 
 * 
 ***********************************************************/
void decOption(int menuOption, unsigned int maxValue,int dec=1);

  /***********************************************************
 * 
 * formatTimeString
 *
 * format seconds into minutes and seconds place in char buffer. 
 * 
 ***********************************************************/
void formatTimeString(unsigned int data, char buffer[]);
 
 /***********************************************************
 * 
 * formatMSString
 *
 * format milliseconds into seconds and ms place in char buffer. 
 * 
 ***********************************************************/
void formatMSString(unsigned int data,char buffer[]);

/***********************************************************
 * 
 * formatThresholdString
 *
 * format into current sensor level : setting level. 
 * 
 ***********************************************************/
void formatThresholdString(unsigned int data, char buffer[]);

/***********************************************************
 * 
 * getSettingMenu
 *
 * get the current Setting menu string and place it in the char buffer for LCD or serial
 * 
 ***********************************************************/
virtual void getSettingMenu(char buffer[]);


private:
	

};

#endif

