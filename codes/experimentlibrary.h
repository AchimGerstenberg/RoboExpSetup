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

int distancelog_left = 0;
int distancelog_right = 0;
float xpos = 0.0;
float ypos = 0.0;
float pos = 0.0;
int alpha = 0;
long int oldleftwheel = 0;
long int oldrightwheel = 0;
float oldx = 0.0;
float oldy = 0.0;
float oldalpha = 0.0;
int old_speed_left_wheel = 0;
int old_speed_right_wheel = 0;
int blink = 0;
int blink2 = 0;
int screencounter = 0;
int dispX[8];
int dispY[8];


bool exeflag = false;
const float rotcm_conversion = 7/240;
const float cmdeg_conversion = 3.07;

unsigned long int time_left = 0;
unsigned long int time_right = 0;
unsigned long int t = 0;
unsigned long int timer1 = 0;
unsigned long int timer2 = 0;
unsigned long int timer3 = 0;
unsigned long int screentimer = 0;

ColorSensorReadType csr;
ColorSensorReadType csr2;

// POSITION TRACKING

void wait(int time)
{
 Wait(time);
}

task updatePositionTask()
{
  if(!exeflag)
  {
    oldx = xpos;
    oldy = ypos;
    oldalpha = alpha;
    exeflag = true;
  }
  
  while(1)
  {
    float deltaleft = MotorRotationCount(OUT_B) - oldleftwheel;
    float deltaright = MotorRotationCount(OUT_C) - oldrightwheel;
    oldleftwheel = MotorRotationCount(OUT_B);
    oldrightwheel = MotorRotationCount(OUT_C);
    float d;


    if(deltaleft >= deltaright)
    {
      d = deltaleft * rotcm_conversion;
      alpha = oldalpha + rotcm_conversion * cmdeg_conversion * (deltaleft - deltaright)+0.5;
      oldalpha = alpha;
    }
    else
    {
      d = deltaright * rotcm_conversion;
      alpha = oldalpha + rotcm_conversion * cmdeg_conversion * (deltaright - deltaleft)+0.5;
      oldalpha = alpha;
    }

    if(abs(deltaleft + deltaright) < 20)    // this is the case when the robot is rotating
    {
      d = 0.0;
    }

    Wait(10);                                 // pause needed or the next calculations go to hell ...

    float sinresult = Sin(alpha);            // save Sin as a float
    float cosresult = Cos(alpha);            // save Cos as a float

    xpos += d*sinresult*0.01;                // now all these floats can be multiplied (but you cannot multiply a Sin or Cos directly with a float
    ypos += d*cosresult*0.01;                // also cos or sin do not accept float as input

    /*
    ClearScreen();
    NumOut(0,LCD_LINE1,xpos);
    NumOut(0,LCD_LINE2,ypos);
    NumOut(0,LCD_LINE3,alpha);
    */

    Wait(250);    // should not be too small because if the MotorRotationCount is small the integer conversion will cut off accuracy
  }
}



/*void updatePosition()
{
  if(!exeflag)
  {
    oldx = xpos;
    oldy = ypos;
    oldalpha = alpha;
    exeflag = true;
  }

  float deltaleft = MotorRotationCount(OUT_B) - oldleftwheel;
  float deltaright = MotorRotationCount(OUT_C) - oldrightwheel;
  oldleftwheel = MotorRotationCount(OUT_B);
  oldrightwheel = MotorRotationCount(OUT_C);
  float d;


  if(deltaleft >= deltaright)
  {
    d = deltaleft * rotcm_conversion;
    alpha = oldalpha + rotcm_conversion * cmdeg_conversion * (deltaleft - deltaright)+0.5;
    oldalpha = alpha;
  }
  else
  {
    d = deltaright * rotcm_conversion;
    alpha = oldalpha + rotcm_conversion * cmdeg_conversion * (deltaright - deltaleft)+0.5;
    oldalpha = alpha;
  }

  if(abs(deltaleft + deltaright) < 5)    // this is the case when the robot is rotating
  {
    d = 0.0;
  }

  Wait(10);                                 // pause needed or the next calculations go to hell ...

  float sinresult = Sin(alpha);            // save Sin as a float
  float cosresult = Cos(alpha);            // save Cos as a float

  xpos += d*sinresult*0.01;                // now all these floats can be multiplied (but you cannot multiply a Sin or Cos directly with a float
  ypos += d*cosresult*0.01;                // also cos or sin do not accept float as input

  ClearScreen();
  NumOut(0,LCD_LINE1,xpos);
  NumOut(0,LCD_LINE2,ypos);
  NumOut(0,LCD_LINE3,alpha);

  Wait(250);    // should not be too small because if the MotorRotationCount is small the integer conversion will cut off accuracy
}
*/


void updatePosition_old()
{
 if(!exeflag)  // initializes reference variables when function is called the first time
 {
  oldx = xpos;
  oldy = ypos;
  oldalpha = alpha;
  exeflag = true;
 }
 Wait(100);

 float deltaleft = (MotorRotationCount(OUT_B) - oldleftwheel);
 float deltaright = (MotorRotationCount(OUT_C) - oldrightwheel);
 float d;   // newly added distance in units of rotation count

 if(deltaleft > deltaright)
 {
  d = deltaright * rotcm_conversion;
  alpha = oldalpha + rotcm_conversion * cmdeg_conversion * (deltaleft - deltaright);
 }
 else
 {
  d = deltaleft * rotcm_conversion;
  alpha = oldalpha - rotcm_conversion * cmdeg_conversion * (deltaright - deltaleft);
 }

 if(deltaleft == deltaright)
 {
  alpha = oldalpha;
  d = deltaleft * rotcm_conversion;
 }
 
 if(abs(deltaleft + deltaright) < 100)    // this is the case when the robot is rotating
 d = 0;
 
 while(alpha < 0)     // modulo 360 degrees
 {
  alpha = alpha + 360;
 }

 while(alpha >= 360)     // modulo 360 degrees
 {
  alpha = alpha - 360;
 }

 xpos = oldx + d*Sin(alpha)/100;
 ypos = oldy + d*Cos(alpha)/100;

 // making the new old values for the next time the function is called
 oldx = xpos;
 oldy = ypos;
 oldalpha = alpha;
 oldleftwheel = MotorRotationCount(OUT_B);
 oldrightwheel = MotorRotationCount(OUT_C);
}




void displayPosition()
{
 if(CurrentTick() - t > 100)
  {
   //updatePosition();
   ClearScreen();
   TextOut(0,LCD_LINE1,"xpos:");
   NumOut(50,LCD_LINE1,xpos);
   TextOut(0,LCD_LINE2,"ypos:");
   NumOut(50,LCD_LINE2,ypos);
   TextOut(0,LCD_LINE3,"alpha:");
   NumOut(50,LCD_LINE3,alpha);

   t = CurrentTick();
  }
}

// MOTOR CONTROLS

void motor(int speed_left_wheel, int speed_right_wheel)
{
 /*
 if (old_speed_left_wheel != speed_left_wheel || old_speed_right_wheel != speed_right_wheel)
 {
  updatePosition();
 }
 */

 OnFwdReg(OUT_B,speed_left_wheel,OUT_REGMODE_SPEED);
 OnFwdReg(OUT_C,speed_right_wheel,OUT_REGMODE_SPEED);
 old_speed_left_wheel = speed_left_wheel;
 old_speed_right_wheel = speed_right_wheel;
}


void turn(int speed, int degree)
{
 long int oldleftcount = MotorRotationCount(OUT_B);
 long int oldrightcount = MotorRotationCount(OUT_C);

 long int targetleft = oldleftcount + degree / (2*rotcm_conversion * cmdeg_conversion);
 long int targetright = oldrightcount - degree / (2*rotcm_conversion * cmdeg_conversion);

 while(MotorRotationCount(OUT_B) < targetleft && MotorRotationCount(OUT_C) > targetright)
 {
  motor(speed,-speed);
 }

while(MotorRotationCount(OUT_B) > targetleft && MotorRotationCount(OUT_C) < targetright)
 {
  motor(-speed,speed);
 }
 motor(0,0);
}



int motorhistory_left()
{
 distancelog_left = (MotorRotationCount(OUT_B)/14.2);
 return distancelog_left;
}

int motorhistory_right()
{
 distancelog_right = (MotorRotationCount(OUT_C)/14.2);
 return distancelog_right;
}



// TIME HANDLING AND RANDOM STUFF

void startTimer1()
{
 timer1 = CurrentTick();
}

void startTimer2()
{
 timer2 = CurrentTick();
}

void startTimer3()
{
 timer3 = CurrentTick();
}

void startScreenTimer()
{
 screentimer = CurrentTick();
}

unsigned long int readTimer1()
{
 return CurrentTick()-timer1;
}

unsigned long int readTimer2()
{
 return CurrentTick()-timer2;
}

unsigned long int readTimer3()
{
 return CurrentTick()-timer3;
}

unsigned long int readScreenTimer()
{
 return CurrentTick()-screentimer;
}



int random(int min, int max)
{
 int x = 0;
 
 if(min > max)
  return 0;
  
 x = min + Random(max-min+1);
 return x;
}




// SENSORS  CONTROLS

int ultrasound()
{
 int distance = SensorUS(IN_4);
 if(distance > 200)
  distance = 200;
 
 if(distance < 0)
  distance = 0;

 return distance;
}

int lightSensorLeft()
{
 SysColorSensorRead(csr2);
 return csr2.RawArray[3];
}

int lightSensorRight()
{
 SysColorSensorRead(csr);
 return csr.RawArray[3];
}

int reflectionDown()
{
 return Sensor(IN_1);
}

task blinklight()
{
 int i1;
 int i2;
 int i3;
 int i4;
 int j1;
 int j2;
 int j3;
 int j4;
 int avgi;
 int avgj;
 int blinkleft;
 int blinkright;

 while(1)
 {
 i1 = lightSensorRight();
 j1 = lightSensorLeft();
 Wait(536);
 i2 = lightSensorRight();
 j2 = lightSensorLeft();
 Wait(536);
 i3 = lightSensorRight();
 j3 = lightSensorLeft();
 Wait(536);
 i4 = lightSensorRight();
 j4 = lightSensorLeft();

 avgi = (i1 + i2 + i3 + i4)/4;
 avgj = (j1 + j2 + j3 + j4)/4;
 blinkleft = (abs((i1 - avgi))+abs((i2 - avgi))+abs((i3 - avgi))+abs((i4 - avgi)))/4;
 blinkright = (abs((j1 - avgj))+abs((j2 - avgj))+abs((j3 - avgj))+abs((j4 - avgj)))/4;
 
 if(blinkleft >= blinkright)
  blink = blinkleft;
 else
  blink = blinkright;


 /*
 avgi = (i1 + i2)/2;
 blink = (abs((i1 - avgi))+abs((i2 - avgi)))/2; */

 /*ClearScreen();
 TextOut(0,LCD_LINE1,"avg");
 NumOut(0,LCD_LINE2,avg);
 TextOut(0,LCD_LINE4,"value");
 NumOut(0,LCD_LINE5,blink);   */

 }
}

task blinklight2()
{
 int i1;
 int i2;
 int j1;
 int j2;
 int blinkleft;
 int blinkright;
 int avgi;
 int avgj;
 
 while(1)
 {
  Wait(536);
  i1 = lightSensorRight();
  j1 = lightSensorLeft();
  Wait(536);
  i2 = lightSensorRight();
  j2 = lightSensorLeft();
  avgi = (i1 + i2)/2;
  avgj = (j1 + j2)/2;
  blinkleft = (abs(i1 - avgi)+abs(i2 - avgi))/2;
  blinkright = (abs(j1 - avgj)+abs(j2 - avgj))/2;
  
  if(blinkleft >= blinkright)
  blink = blinkleft;
 else
  blink = blinkright;
 }
}


int reflectionRedRight()
{
 SysColorSensorRead(csr);
 return csr.NormalizedArray[0];
}

int reflectionGreenRight()
{
 SysColorSensorRead(csr);
 return csr.NormalizedArray[1];
}

int reflectionBlueRight()
{
 SysColorSensorRead(csr);
 return csr.NormalizedArray[2];
}
int reflectionRedLeft()
{
 SysColorSensorRead(csr2);
 return csr2.NormalizedArray[0];
}

int reflectionGreenLeft()
{
 SysColorSensorRead(csr2);
 return csr2.NormalizedArray[1];
}

int reflectionBlueLeft()
{
 SysColorSensorRead(csr2);
 return csr2.NormalizedArray[2];
}



void displaySensorData()
{
  if(CurrentTick() - t > 100)
  {
   ClearScreen();
   TextOut(0,LCD_LINE1,"ultrasound:");
   NumOut(80,LCD_LINE1,ultrasound());

   TextOut(0,LCD_LINE3,"refl. down:");
   NumOut(80,LCD_LINE3,reflectionDown());

   TextOut(0,LCD_LINE4,"light right:");
   NumOut(80,LCD_LINE4,lightSensorRight());

   TextOut(0,LCD_LINE5,"light left:");
   NumOut(80,LCD_LINE5,lightSensorLeft());

   TextOut(0,LCD_LINE6,"refl. red left:");
   NumOut(80,LCD_LINE6,reflectionRedLeft());

   TextOut(0,LCD_LINE7,"refl. green left:");
   NumOut(80,LCD_LINE7,reflectionGreenLeft());

   TextOut(0,LCD_LINE8,"refl. blue left:");
   NumOut(80,LCD_LINE8,reflectionBlueLeft());

   t = CurrentTick();
  }
}



void gotoPosition(int x,int y,int a)
{
 //updatePosition();
 int angle;
 int deltax = x - xpos;
 int deltay = y - ypos;
 int d = sqrt(deltax*deltax + deltay*deltay);     // Pythagoras
 

 // turn towards destination
 if(d != 0)
 {
  int arg = 100*deltay/d;

  if(deltax > 0)
  {
   angle = 90 - Asin(arg);
  }
  else
  {
   angle = 270 + Asin(arg);
  }

  if(deltax == 0 && deltay > 0)
  angle = 0;

  if(deltax == 0 && deltay < 0)
  angle = 180;
 }

 if(d == 0)
  angle = a;




 if(angle - alpha <= 180 || angle - alpha < 0)
 turn(10,angle - alpha);

 if(angle - alpha > 180)
 turn(10,angle - alpha - 360);

 // drive distance d
 long int oldwheel = MotorRotationCount(OUT_B);
 long int neededrotation = oldwheel + d/rotcm_conversion;

 while(neededrotation >= MotorRotationCount(OUT_B))
 {
  //if(reflection_down() > 55)
   motor(20,20);
  //else
  //{
  // motor(0,0);
  // return;
  //}
 }
 motor(0,0);

 // turn into end position
 //updatePosition();
 if(a - alpha <= 180 || a - alpha < 0)
 turn(10,a - alpha);   // turn  clockwise

 if(a - alpha > 180)
 turn(10,a - alpha - 360);  // turn anticlockwise
}


// ScreenOut functions / print(x,y,content)

/* The following function have the aim to make the ClearScreen() function obsolete for the participant. The ClearScreen shall either be executed if the participant writes
a new display output exactly at a location that already has a display output or if the last display output was more than 10 ms ago.
I use two arrays (x and y) to store locations of the previous eight display outputs. For some reason I could not get a 2D array to work ... */


// initialize the arrays to a value outside the screen.
void dispInit()
{
 for(int i = 0; i < 8; i++)
 {
  dispX[i] = 99;
  dispY[i] = 99;
 }
}

// enter the newest location "on the top of the stack" and push the older entries "down"
void dispPush(int x, int y)
{
 for(int i = 7; i > 0; i--)
 {
  dispY[i] = dispY[i-1];
  dispX[i] = dispX[i-1];
  dispY[0] = y;
  dispX[0] = x;
 }
}

// for diagnostic purposes
void dispPrint()
{
 NumOut(30,LCD_LINE1,dispX[0]);        NumOut(50,LCD_LINE1,dispY[0]);
 NumOut(30,LCD_LINE2,dispX[1]);        NumOut(50,LCD_LINE2,dispY[1]);
 NumOut(30,LCD_LINE3,dispX[2]);        NumOut(50,LCD_LINE3,dispY[2]);
 NumOut(30,LCD_LINE4,dispX[3]);        NumOut(50,LCD_LINE4,dispY[3]);
 NumOut(30,LCD_LINE5,dispX[4]);        NumOut(50,LCD_LINE5,dispY[4]);
 NumOut(30,LCD_LINE6,dispX[5]);        NumOut(50,LCD_LINE6,dispY[5]);
 NumOut(30,LCD_LINE7,dispX[6]);        NumOut(50,LCD_LINE7,dispY[6]);
 NumOut(30,LCD_LINE8,dispX[7]);        NumOut(50,LCD_LINE8,dispY[7]);
 Wait(1000);
 ClearScreen();
}


void dispNum(int x, int y, float a)
{
 // if last display output was longer than 10 ms ago -> ClearScreen before new output
 if(readScreenTimer() > 10)
 {
  ClearScreen();
  dispInit();
 }

 // searches through the array looking for previous entries at equal locations, if so -> ClearScreen first
 for(int i = 0; i < 8; i++)
 {
  if(dispY[i] == y)
  {
   if(dispX[i] == x)
   {
    ClearScreen();
    dispInit();
   }
  }
 }
 NumOut(x,y,a);
 dispPush(x,y);
 startScreenTimer();
}

void dispText(int x, int y, string a)
{
 // if last display output was longer than 10 ms ago -> ClearScreen before new output
 if(readScreenTimer() > 10)
 {
  ClearScreen();
  dispInit();
 }

 // searches through the array looking for previous entries at equal locations, if so -> ClearScreen first
 for(int i = 0; i < 8; i++)
 {
  if(dispY[i] == y)
  {
   if(dispX[i] == x)
   {
    ClearScreen();
    dispInit();
   }
  }
 }
 TextOut(x,y,a);
 dispPush(x,y);
 startScreenTimer();
}

