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

task main()
{
 SetSensorLight(IN_1);
 SetSensorColorFull(IN_2);
 SetSensorColorFull(IN_3);
 SetSensorUltrasonic(IN_4);
 csr.Port = IN_3;
 csr2.Port= IN_2;
 
 dispInit();
 
 oldx = xpos;
  oldy = ypos;
  oldalpha = alpha;
  exeflag = true;

 //StartTask(your_code_here);
 Precedes(your_code_here, blinklight2, updatePositionTask);
 //Precedes(your_code_here, achim);

}
