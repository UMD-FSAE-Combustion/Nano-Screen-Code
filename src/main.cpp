/*
 *  THIS CODE IS FOR THE 3.5" TFT Display
 *  To install this code, download and open with PlatformIO and plug in arduino
 *  then upload to arduino.
 *
 *  ********DO NOT EDIT THE CODE UNLESS YOU KNOW WHAT YOU ARE DOING********
 */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <mcp2515.h>
#include <String.h>
#include <fonts/FreeSansBold12pt7b.h>

#define BLACK 0x0000
#define NAVY 0x000F
#define DARKGREEN 0x03E0
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define PURPLE 0x780F
#define OLIVE 0x7BE0
#define LIGHTGREY 0xC618
#define DARKGREY 0x7BEF
#define BLUE 0x001F
#define GREEN 0x07E0
#define CYAN 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xFD20
#define GREENYELLOW 0xAFE5
#define PINK 0xF81F

struct can_frame canMsg;
MCP2515 mcp2515(10); 

MCUFRIEND_kbv tft;

int gearPos = 0;
uint32_t rpmSpeed = 0;
String rpmString = "";
String rpmDisp = "";


/* Function for reconstructing hexadecimal string */
void swap(char &a, char &b) {
  char  c = a;
  a = b;
  b = c;
}

/* Function for calculating RPM value from hexadecimal string */
int calcRPM(String hexVal) {

  int len = hexVal.length(); 
  
  // Initializing base value to 1, i.e 16^0 
  int base = 1; 
  
  int dec_val = 0;

  // Extracting characters as digits from last 
  // character 
  for (int i = len - 1; i >= 0; i--) { 
      // if character lies in '0'-'9', converting 
      // it to integral 0-9 by subtracting 48 from 
      // ASCII value 
      if (hexVal[i] >= '0' && hexVal[i] <= '9') { 
        dec_val += (int(hexVal[i]) - 48) * base; 
  
        // incrementing base by power 
        base = base * 16; 
      } 
  
      // if character lies in 'A'-'F' , converting 
      // it to integral 10 - 15 by subtracting 55 
      // from ASCII value 
      else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') { 
          dec_val += (int(hexVal[i]) - 55) * base; 
  
          // incrementing base by power 
          base = base * 16; 
        } 
    } 

    return dec_val / 6;
}

/* Function to pack CAN data back into hex format */
String decToHex(int n) { 
    // ans string to store hexadecimal number 
    String ans = ""; 
    
    while (n != 0) { 
        // remainder variable to store remainder 
        int rem = 0; 
          
        // ch variable to store each character 
        char ch; 
        // storing remainder in rem variable. 
        rem = n % 16; 
  
        // check if temp < 10 
        if (rem < 10) { 
            ch = rem + 48; 
        } 
        else { 
            ch = rem + 55; 
        } 
          
        // updating the ans string with the character variable 
        ans += ch; 
        n = n / 16; 
    } 
      
    // reversing the ans string to get the final result 
    int i = 0, j = ans.length() - 1; 
    while(i <= j) 
    { 
      swap(ans[i], ans[j]); 
      i++; 
      j--; 
    } 
    return ans; 
} 

void setup() {
  // put your setup code here, to run once:
  uint16_t ID=tft.readID();
  Serial.begin(115200);
  Serial.println(ID); 
  
  //Initializing TFT display:
  tft.begin(ID);
  tft.invertDisplay(0);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setFont(&FreeSansBold12pt7b);

// Initializing CAN module
  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ); //Sets CAN at speed and Clock 8MHz
  mcp2515.setNormalMode();                  //Sets CAN at normal mode

}

void loop() {
if(mcp2515.readMessage(&canMsg) != MCP2515::ERROR_OK) {

    /* If no CAN data is being recieved, this warning flashes */
    tft.drawTriangle(190, 150, 305, 150, 250, 20, RED);
    tft.setCursor(225, 135);
    tft.setTextSize(5);
    tft.setTextColor(RED, BLACK);
    tft.print("!");

    tft.setTextSize(2);
    tft.setCursor(80, 235);
    tft.print("no CAN data!");
  }

  else {

    /* Processing the RPM speed and driving the display */
    tft.fillScreen(BLACK);
    while(mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      
      gearPos = canMsg.data[0];
         if (gearPos == 0) {
           tft.setCursor(205, 280);
           tft.setTextColor(WHITE, BLACK);
           tft.setTextSize(6);
           tft.print("N");
         }
         else {
            tft.setCursor(205, 280);
           tft.setTextColor(WHITE, BLACK);
           tft.setTextSize(6);
           tft.print(gearPos);
         }
      

      tft.setCursor(120, 125);
      tft.setTextSize(2);
      tft.setTextColor(WHITE, BLACK);
      tft.print("RPM ");

      rpmString = decToHex(canMsg.data[1]);
      rpmString += decToHex(canMsg.data[2]);
      rpmString += decToHex(canMsg.data[3]);
      rpmString += decToHex(canMsg.data[4]); 

      rpmSpeed = calcRPM(rpmString);
      rpmDisp = String(rpmSpeed);
      while (rpmDisp.length() > 4)
        rpmDisp = "0" + rpmDisp;
      
      tft.setCursor(235, 95);
      tft.setTextSize(2);
      tft.setTextColor(WHITE, BLACK);
      tft.print(rpmDisp);
      

      if(gearPos == 1 || gearPos == 0) {

        if(rpmSpeed <= 8000) 
          tft.fillRect(2,2,480,85,GREEN);
        else if (rpmSpeed > 8000 && rpmSpeed <= 8800)
          tft.fillRect(2,2,480,85,YELLOW);
        else if (rpmSpeed > 8800 && rpmSpeed <= 8900)
          tft.fillRect(2,2,480,85,RED);

      }
      else if(gearPos != 1 && gearPos != 0) {

         if(rpmSpeed >= 6000 && rpmSpeed <= 8000) 
            tft.fillRect(2,2,480,85,GREEN);
          else if (rpmSpeed > 8000 && rpmSpeed <= 8800)
            tft.fillRect(2,2,480,85,YELLOW);
          else if (rpmSpeed > 8800 && rpmSpeed <= 8900)
            tft.fillRect(2,2,480,85,RED);
          else
            tft.fillRect(2,2,480,85,CYAN);

      }
    }
  }
}

/*

if(mcp2515.readMessage(&canMsg) != MCP2515::ERROR_OK) {

    tft.drawTriangle(190, 140, 305, 140, 250, 20, RED);
    tft.setCursor(215, 125);
    tft.setTextSize(5);
    tft.setTextColor(RED, BLACK);
    tft.print("!");

    tft.setTextSize(2);
    tft.setCursor(80, 235);
    tft.print("no CAN data!");
  }

  else {

    tft.fillScreen(BLACK);
    while(mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {

      gearPos = canMsg.data[0];
         if (gearPos == 0) {
          tft.setCursor(215, 200);
          tft.setTextColor(WHITE, BLACK);
          tft.setTextSize(15);
          tft.print("N");
         }
         else {
          tft.setCursor(215, 200);
          tft.setTextColor(WHITE, BLACK);
          tft.setTextSize(15);
          tft.print(gearPos);
         }
      

      tft.setCursor(80, 95);
      tft.setTextSize(7);
      tft.setTextColor(WHITE, BLACK);
      tft.print("RPM ");

      
      rpmString = decToHex(canMsg.data[1]);
      rpmString += decToHex(canMsg.data[2]);
      rpmString += decToHex(canMsg.data[3]);
      rpmString += decToHex(canMsg.data[4]); 


      tft.fillRect(230,95,200,50,BLACK);
      rpmSpeed = calcRPM(rpmString);
      tft.setCursor(235, 95);
      tft.setTextSize(7);
      tft.setTextColor(WHITE, BLACK);
      tft.print(rpmSpeed);
      

      if(gearPos == 1 || gearPos == 0) {

        if(rpmSpeed <= 8000) 
          tft.fillRect(2,2,480,85,GREEN);
        else if (rpmSpeed > 8000 && rpmSpeed <= 8800)
          tft.fillRect(2,2,480,85,YELLOW);
        else if (rpmSpeed > 8800 && rpmSpeed <= 8900)
          tft.fillRect(2,2,480,85,RED);

      }
      else if(gearPos != 1 && gearPos != 0) {

         if(rpmSpeed >= 6000 && rpmSpeed <= 8000) 
            tft.fillRect(2,2,480,85,GREEN);
          else if (rpmSpeed > 8000 && rpmSpeed <= 8800)
            tft.fillRect(2,2,480,85,YELLOW);
          else if (rpmSpeed > 8800 && rpmSpeed <= 8900)
            tft.fillRect(2,2,480,85,RED);
          else
            tft.fillRect(2,2,480,85,CYAN);

      }
    }
  }

*/


/*
    tft.fillRect(2,2,480,85,GREEN);

    tft.setCursor(120, 125);
    tft.setTextSize(2);
    tft.setTextColor(WHITE, BLACK);
    tft.print("RPM ");

    tft.setCursor(275, 125);
    tft.setTextSize(2);
    tft.setTextColor(WHITE, BLACK);
    tft.print("0");

    tft.setCursor(205, 280);
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(6);
    tft.print("N");
*/
