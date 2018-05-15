/* 
 Copyright 2018 Achim Gerstenberg
 achim.gerstenberg@ntnu.no

 This program is free software : you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#include <PS2KeyAdvanced.h>
#include <PS2KeyMap.h>
#include <Keyboard.h>
#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>

#define DATAPIN 6
#define IRQPIN  2
#define chipselect 4
#define participant "keylog.txt"

PS2KeyAdvanced keyboard;
PS2KeyMap keymap;
File keylogfile;
File tasklogfile;
File infofile;
File summaryfile;

uint16_t code;
uint16_t mapped;
uint8_t found;
bool special = 0;

// variables for saving code automatically
int task = 0;
String taskstr;
String taskname = "task";
int iteration = 0;
String iterationstr;
String filename;
long unsigned int tasktimer = 0;
long unsigned int lastcompile = 0;
long unsigned int duration = 0;
long unsigned int F5timer = 0;
long unsigned int millisoffset = 0;
time_t unixtime;


void loadcode()
{
  Keyboard.write(130);  // left alt
  Keyboard.write(102);  // f
  Keyboard.write(111);  // a
  //Keyboard.write(178);  // backspace
  delay(200);
  Keyboard.print(filename);   //write the actual filename
  delay(200);
  Keyboard.write(179);  // tab
  Keyboard.write(179);  // tab again
  Keyboard.write(176);  // carriage return  
  delay(1000);
}


void F6F7F8()
{
  Keyboard.write(199);  // F6
  delay(3000);
  Keyboard.write(200);  // F7
  delay(350);
  Keyboard.write(201);  // F8
  delay(800);
  Keyboard.write(201);  // F8 again because sometimes it does not work 
}



void createfilename()
{
  filename = task;
  filename.concat("_");
  
  if(iteration < 10)
  {
   iterationstr = "0";
   iterationstr.concat(iteration);
  }
  else
  {
   iterationstr = iteration;
  }
  filename.concat(iterationstr);
  Serial.println(filename);
}





void copyinfo(String destination)
{
  File destinationfile;
  String tempstring="";

  
  infofile = SD.open("info.txt");
  if(infofile)
  {
   while(infofile.available())
   {
     tempstring.concat((char)infofile.read());
   }
   infofile.close();
  }
  else
  Serial.println("reading info from info.txt failed");

  
  destinationfile = SD.open(destination, FILE_WRITE);
  if(destinationfile)
  {
   destinationfile.println(tempstring);
   destinationfile.close();
  }
  else
  Serial.println("copying info to " + destination + " failed");

  
  destinationfile = SD.open(destination, FILE_READ);
  if(destinationfile)
  {
   Serial.println("content of " + destination + ":");
   while(destinationfile.available())
   {
     Serial.write(destinationfile.read());
   }
   destinationfile.close();
  }
  else
  Serial.println("reading check from " + destination + " failed");
}






void taskentry()
{
  tasklogfile = SD.open("tasklog.txt", FILE_WRITE);
  if(tasklogfile)
  {
    createfilename();
    tasklogfile.println(filename + "\t" + (millis() - millisoffset) + "\t" + task + "\t" + iteration);
    tasklogfile.close();
  }
  else
  Serial.println("writing error in tasklog.txt");


  tasklogfile = SD.open("tasklog.txt", FILE_READ);
  if(tasklogfile)
  {
   Serial.println("content of tasklog.txt:");
   while(tasklogfile.available())
   {
     Serial.write(tasklogfile.read());
   }
   tasklogfile.close();
  }
  else
  Serial.println("reading check from tasklog.txt failed");
}


unsigned long int setunixtime()
{
  Serial.println("Calibration of the clock:");
  Serial.println("fill in a unix time stamp of ca. 10 seconds in advance and press enter when this time is actually reached");
  
  while (Serial.available() == 0);
  {
   unixtime = Serial.parseInt(); //read int or parseFloat for ..float...
  }
  Serial.println(unixtime);
  
  infofile = SD.open("info.txt", FILE_WRITE);
  if(infofile)
  {
   infofile.print("unixtime: ");
   infofile.println(unixtime);
   infofile.print("millis offset at unixtime: ");
   infofile.println(millis());
   infofile.println("-----------------------------");
   infofile.close();
  }
  else
  Serial.println("attaching unixtime and millis to info.txt failed");
  
  return millis();
}


void setup() {
  Serial.begin( 115200 );
  keyboard.begin( DATAPIN, IRQPIN );
  Keyboard.begin();

  pinMode(12, INPUT);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!SD.begin(chipselect)) {
    Keyboard.println("SD card initialization failed!");
    Serial.println("SD card initialization failed!");
    return;}
  
  keyboard.setNoBreak( 1 );   // Disable Break codes (key release) from PS2KeyAdvanced
  keyboard.setNoRepeat( 1 );  // and set no repeat on CTRL, ALT, SHIFT, GUI while outputting
  keymap.selectMap( (char *)"FR" ); // set to "Norwegian" keyboard (which used to be French)

  millisoffset = setunixtime();
  
  copyinfo("keylog.txt");
  copyinfo("tasklog.txt");
  copyinfo("summary.txt");
}

void loop() {

  if(digitalRead(12))   // if button for next task is pressed
  {
    /*
    // save old task duration and number of interations to summary file
    summaryentry(); 
    
    // increment task for filename and reset iteration (# of compilations to 1), delay to debounce the button (do not press the button longer than 1 sec)
    task++;
    iteration = 0;
    createfilename();
    delay(200); 
    loadcode();   // loads the next nxc file
    F6F7F8();     // compile and load onto robot (F6), execute (F7) and abort program (F8)
    taskentry();
    */

    task++;

    // save code with filename eval#
    Keyboard.write(130);  // left alt
    Keyboard.write(102);  // f
    Keyboard.write(97);  // a
    delay(200);

    //ctrl + x for storing old filename
    Keyboard.press(128);
    Keyboard.press('x');
    Keyboard.release('x');
    Keyboard.release(128);

    filename = "eval";
    filename.concat(task);
    Keyboard.print(filename);   //write the iteration number
    delay(200);
    Keyboard.write(179);  // tab
    Keyboard.write(179);  // tab again
    Keyboard.write(176);  // carriage return

    // loading code onto robot
    F6F7F8();
    
    // saving again under old filename so that it looks like nothing happened
    Keyboard.write(130);  // left alt
    Keyboard.write(102);  // f
    Keyboard.write(97);  // a
    delay(200);
    // ctrl + v for writing the stored old filename
    Keyboard.press(128);
    Keyboard.press('v');
    Keyboard.release('v');
    Keyboard.release(128);
    delay(200);
    Keyboard.write(179);  // tab
    Keyboard.write(179);  // tab again
    Keyboard.write(176);  // carriage return 
    // it now says that filename already exists and asks if it shall overwrite, answer yes by pressing "y"
    Keyboard.press('y');
    Keyboard.release('y');

    // documenting in the tasklogfile
    tasklogfile = SD.open("tasklog.txt", FILE_WRITE);
    if(tasklogfile)
    {
      tasklogfile.println(filename + "\t" + (millis() - millisoffset) + "\t" + task + "\t" + iteration);
      tasklogfile.close();
    }

    // documenting in the keylogfile
    keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("EVAL ");
          keylogfile.println(task);
          keylogfile.close();
        }
     Serial.print("EVAL ");
     Serial.println(task);
     delay(500);
  }




  if(keyboard.available())
  { 

    keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.println("");
          keylogfile.print(millis()-millisoffset);
          keylogfile.print("\t");
          keylogfile.close();
        }  
        
    code = keyboard.read();
    mapped = keymap.remapKey(code);   // needed to destinguish between small and capital letters but special characters return 0.
    Serial.print("code: ");
    Serial.println(code);
    Serial.print("mapped: ");
    Serial.println(mapped);

   

    if(mapped > 0)  // if > 0 then "normal" key else special key like ctrl
    {  
     
     switch (mapped)
      {
        case 27:// esc
        Keyboard.write(177);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[ESC]");
          keylogfile.close();
        } 
        break;
        
        case 127://delete
        Keyboard.write(212);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[DEL]");
          keylogfile.close();
        }  
        break;
        
        case 13:// enter/return
        Keyboard.write(176);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.println("[RETURN]");
          keylogfile.close();
        }          
        break;

        case 250: // caps lock
        Keyboard.write(193); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[CAPS]");
          keylogfile.close();
        }         
        break;
        
        case 9: // tab
        Keyboard.write(179);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[TAB]");
          keylogfile.close();
        }          
        break;
        
        case 8: // backspace
        Keyboard.write(178); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[BSP]");
          keylogfile.close();
        }         
        break;

        case 39: // "'" apostrophe 
        Keyboard.write(mapped);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[APO]");
          keylogfile.close();
        }         
        break;

        case 32: // spacebar 
        Keyboard.write(mapped);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[SPACE]");
          keylogfile.close();
        }         
        break;

        case 8275:  // ctrl + s 
        Serial.println("ctrl + s");
        Keyboard.press(128);
        Keyboard.press('s');
        Keyboard.release('s');
        Keyboard.release(128);
        special = 1;
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[CTRL S]");
          keylogfile.close();
        }         
        break;

        case 8262:  // ctrl + f 
        Serial.println("ctrl + f");
        Keyboard.press(128);
        Keyboard.press('s');
        Keyboard.release('s');
        Keyboard.release(128);
        special = 1;
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[CTRL F]");
          keylogfile.close();
        }         
        break;

        case 8312:  // ctrl + x works
        Serial.println("ctrl + x");
        Keyboard.press(128);
        Keyboard.press('x');
        Keyboard.release('x');
        Keyboard.release(128);
        special = 1;
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[CTRL X]");
          keylogfile.close();
        }         
        break;

        case 8291:  // ctrl + c works
        Serial.println("ctrl + c");
        Keyboard.press(128);
        Keyboard.press('c');
        Keyboard.release('c');
        Keyboard.release(128);
        Keyboard.press(128);    // the "ctrl + c" actually does control x and thus we need to print again what we just deleted.
        Keyboard.press('v');
        Keyboard.release('v');
        Keyboard.release(128);
        special = 1;
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[CTRL C]");
          keylogfile.close();
        }         
        break;

        case 8310:  // ctrl + v works
        Serial.println("ctrl + v");
        Keyboard.press(128);
        Keyboard.press('v');
        Keyboard.release('v');
        Keyboard.release(128);
        special = 1;
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[CTRL V]");
          keylogfile.close();
        }         
        break; 

        case 8289:  // ctrl + a works
        Serial.println("ctrl + a");
        Keyboard.press(128);
        Keyboard.press('a');
        Keyboard.release('a');
        Keyboard.release(128);
        special = 1;
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[CTRL A]");
          keylogfile.close();
        } 
        break; 

        case 8314:  // ctrl + a works
        Serial.println("ctrl + z");
        Keyboard.press(128);
        Keyboard.press('z');
        Keyboard.release('z');
        Keyboard.release(128);
        special = 1;
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[CTRL Z]");
          keylogfile.close();
        } 
        break;         

        case 2057:  // alt + tab does not work
        Serial.println("alt + tab");
        Keyboard.press(130);
        Keyboard.press(179);
        Keyboard.release(179);
        Keyboard.release(130);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[ALT TAB]");
          keylogfile.close();
        } 
        break; 

      } 
      
      
      if(special)
      {
       //Keyboard.write(178); // backspace --> delete the c from ctrl + c, etc.
       special = 0;
      }
      else
      {
       if(!(mapped == 39 || mapped == 8 || mapped == 32 || mapped == 9 || mapped == 13)) // 39=' ; 8 = BSP ; 32 = SPACE ; 9 = TAB ; 13 = Carriage return
       { 
          Keyboard.write(mapped);     // THIS IS THE OUTPUT FOR NORMAL CHARACTER SUCH AS LETTERS
          keylogfile = SD.open(participant, FILE_WRITE);
          if (keylogfile) 
          {  
            keylogfile.write(mapped);
            keylogfile.close();  
          } 
       }
      }
          
    }



    
    else
    { // special key
      Serial.println(code);
      switch (code)
      {
        case 4355: // caps lock
        Keyboard.write(193); 
        break;
        
        case 20742: // left shift
        Keyboard.write(129); 
        break;
        
//        case 12552: // left ctrl
//        Keyboard.write(128); 
//        break;
        
        case 4876: // left windows
        Keyboard.write(131); 
        break;
        
//        case 6410: // left alt
//        Keyboard.write(130); 
//        break;
        
        case 5387: // right alt gr
        Keyboard.write(134); 
        break;

        case 12553: // r ctrl
        Keyboard.write(132); 
        break;
        
        case 274: // end
        Keyboard.write(213); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[END]");
          keylogfile.close();
        } 
        break;
        
        case 276: // page down
        Keyboard.write(214);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[PGDWN]");
          keylogfile.close();
        }  
        break;
        
        case 275: // page up
        Keyboard.write(211); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[PGUP]");
          keylogfile.close();
        } 
        break;

        case 273: // home
        Keyboard.write(210); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[HOME]");
          keylogfile.close();
        } 
        break;
        
        case 281: // insert
        Keyboard.write(209); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[INS]");
          keylogfile.close();
        } 
        break;

        case 277: // arrow left
        Keyboard.write(216);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[LEFT]");
          keylogfile.close();
        }  
        break;
        
        case 280: // arrow down
        Keyboard.write(217);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[DOWN]");
          keylogfile.close();
        }  
        break;

        case 278: // arrow right
        Keyboard.write(215); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[RIGHT]");
          keylogfile.close();
        } 
        break;
        
        case 279: // arrow up
        Keyboard.write(218);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[UP]");
          keylogfile.close();
        } 
        break;
                
        case 353: // F1
        Keyboard.write(194); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F1]");
          keylogfile.close();
        } 
        break;
        
        case 354: // F2
        Keyboard.write(195);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F2]");
          keylogfile.close();
        }  
        break;
        
        case 355: // F3
        Keyboard.write(196);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F3]");
          keylogfile.close();
        }  
        break;
        
        case 356: // F4
        Keyboard.write(197); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F4]");
          keylogfile.close();
        } 
        break;
        
        case 357: // F5
        Keyboard.write(198);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F5]");
          keylogfile.close();
        }  
        F5timer = millis()-millisoffset;
        break;


        
        case 358: // F6 --> compile & load code --> also save file
        Serial.println(F5timer);
        Serial.println(millis() - millisoffset - F5timer);
        if(millis()-millisoffset - 15000 < F5timer)    
        { 
                 
         //create filename for the nxc file.
         iteration++;        
         createfilename();
          
        
         // saving as:
         Keyboard.write(130);  // left alt
         Keyboard.write(102);  // f
         Keyboard.write(97);  // a
         delay(200);
         Keyboard.press(215); // arrow right
         Keyboard.release(215);
         Keyboard.write(178); // BSP, 4x to delete last extension
         Keyboard.write(178);
         Keyboard.write(178);
         Keyboard.write(178);
         Keyboard.print(filename);   //write the iteration number
         delay(200);
         Keyboard.write(179);  // tab
         Keyboard.write(179);  // tab again
         Keyboard.write(176);  // carriage return

         taskentry();
         lastcompile = millis() - millisoffset;
         delay(1000); 
         F6F7F8();
              
         keylogfile = SD.open(participant, FILE_WRITE);
         if (keylogfile) 
         {
           keylogfile.print("[F6]");
           keylogfile.close();
         }
        }    
        break;
        
/*
        case 359: // F7
        Keyboard.write(200);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F7]");
          keylogfile.close();
        }  
        break;
        
        case 360: // F8
        Keyboard.write(201); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F8]");
          keylogfile.close();
        } 
        break;
        
        case 361: // F9
        Keyboard.write(202);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F9]");
          keylogfile.close();
        }  
        break;
        
        case 362: // F10
        Keyboard.write(203);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F10]");
          keylogfile.close();
        }  
        break;

        case 363: // F11
        Keyboard.write(204);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F11]");
          keylogfile.close();
        }  
        break;
        
        case 364: // F12
        Keyboard.write(205); 
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[F12]");
          keylogfile.close();
        } 
        break;*/

        case 16661:  // shift + arrow left
        Keyboard.press(129);
        Keyboard.press(216);
        Keyboard.release(216);
        Keyboard.release(129);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[SHIFT + LEFT]");
          keylogfile.close();
        } 
        break;

        case 16662:  // shift + arrow right
        Keyboard.press(129);
        Keyboard.press(215);
        Keyboard.release(215);
        Keyboard.release(129);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[SHIFT + RIGHT]");
          keylogfile.close();
        } 
        break;

        case 16663:  // shift + arrow up
        Keyboard.press(129);
        Keyboard.press(218);
        Keyboard.release(218);
        Keyboard.release(129);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[SHIFT + UP]");
          keylogfile.close();
        } 
        break;

        case 16664:  // shift + arrow down
        Keyboard.press(129);
        Keyboard.press(217);
        Keyboard.release(217);
        Keyboard.release(129);
        keylogfile = SD.open(participant, FILE_WRITE);
        if (keylogfile) 
        {
          keylogfile.print("[SHIFT + DOWN]");
          keylogfile.close();
        } 
        break;          
      }
    }
  }

    
    
  delay( 10 );
  
}
