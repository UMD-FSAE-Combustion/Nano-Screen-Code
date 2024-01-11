/*
 *  THIS CODE IS FOR THE 3.5" TFT Display
 *  To install this code, download and open with PlatformIO and plug in arduino
 *  then upload to arduino.
 *
 *  ********DO NOT EDIT THE CODE UNLESS YOU KNOW WHAT YOU ARE DOING********
 * 
 *  V 2.0
 */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <mcp2515.h>
#include <FlickerFreePrint.h>
#include <fonts/FreeSansBold12pt7b.h>
#include <fonts/FreeSansBold24pt7b.h>

enum ShiftState { NO_SHIFT = 0, PREP_SHIFT = 1, UPSHIFT = 2, TEST = 3 };

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

FlickerFreePrint<MCUFRIEND_kbv> GEAR(&tft, WHITE, BLACK);
FlickerFreePrint<MCUFRIEND_kbv> SHIFT_MSG(&tft, WHITE, BLACK);

int gearPos = 0;
ShiftState shift;


void setup() {
  // put your setup code here, to run once:
  uint16_t ID = tft.readID();
  Serial.begin(115200);
  Serial.println(ID); 
  
  //Initializing TFT display:
  tft.begin(ID);
  tft.invertDisplay(1);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setFont(&FreeSansBold24pt7b);

  //SHIFT_MSG.setFont(&FreeSansBold24pt7b);

  // Initializing CAN module
  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ); //Sets CAN at 1MBps and Clock 8MHz
  mcp2515.setNormalMode();                  //Sets CAN at normal mode

}

void loop() {

  tft.fillScreen(BLACK);
  while(mcp2515.readMessage(&canMsg) != MCP2515::ERROR_OK) {

    // If no CAN data is being recieved, this warning flashes 
    tft.drawTriangle(195, 150, 295, 150, 250, 20, RED);
    tft.setCursor(230, 135);
    tft.setTextSize(2);
    tft.setTextColor(RED, BLACK);
    tft.print("!");

    tft.setTextSize(1);
    tft.setCursor(82, 235);
    tft.print("no CAN data!");
  }
  //else {

    // Processing the RPM speed and driving the display 
    tft.fillScreen(BLACK);
    while(mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      
      gearPos = canMsg.data[0];
      if (gearPos == 0 || -1) {
        tft.setCursor(190, 285);
        tft.setTextSize(3);
        GEAR.print("N");
      }
      else {
        tft.setCursor(190, 285);
        tft.setTextSize(3);
        GEAR.print(gearPos);
      }

      if(gearPos == 1 || gearPos == 0) {

        if(shift = NO_SHIFT) { 
          tft.fillRect(2,2,480,85,GREEN);
          tft.setCursor(75, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("              ");
        }
        else if (shift = PREP_SHIFT) {
          tft.fillRect(2,2,480,85,YELLOW);

          tft.setCursor(75, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("GET READY");
        }      
        else if (shift = UPSHIFT) {
          tft.fillRect(2,2,480,85,RED);

          tft.setCursor(75, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("UPSHIFT NOW");
        }
      }
      else if(gearPos != 1 && gearPos != 0) {

        if(shift = NO_SHIFT) { 
          tft.fillRect(2,2,480,85,GREEN);
          tft.setCursor(75, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("              ");
        }
        else if (shift = PREP_SHIFT) {
          tft.fillRect(2,2,480,85,YELLOW);

          tft.setCursor(75, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("GET READY");
        }      
        else if (shift = UPSHIFT) {
          tft.fillRect(2,2,480,85,RED);

          tft.setCursor(75, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("UPSHIFT NOW");
        }  
        else {
          tft.fillRect(2,2,480,85,CYAN);

          tft.setCursor(75, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("DOWNSHIFT");
        } 
      }
    }
  //}
}

/*

void loop() {

  if(mcp2515.readMessage(&canMsg) != MCP2515::ERROR_OK) {

    // If no CAN data is being recieved, this warning flashes 
    tft.fillScreen(BLACK);
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

    // Processing the RPM speed and driving the display 
    tft.fillScreen(BLACK);
    while(mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      
      gearPos = canMsg.data[0];
      if (gearPos == 0 || -1) {
        tft.setCursor(190, 285);
        tft.setTextSize(3);
        GEAR.print("N");
      }
      else {
        tft.setCursor(190, 285);
        tft.setTextSize(3);
        GEAR.print(gearPos);
      }

      if(gearPos == 1 || gearPos == 0) {

        if(shift = NO_SHIFT) { 
          tft.fillRect(2,2,480,85,GREEN);
          tft.setCursor(80, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("              ");
        }
        else if (shift = PREP_SHIFT) {
          tft.fillRect(2,2,480,85,YELLOW);

          tft.setCursor(80, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("GET READY");
        }      
        else if (shift = UPSHIFT) {
          tft.fillRect(2,2,480,85,RED);

          tft.setCursor(80, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("UPSHIFT NOW");
        }
      }
      else if(gearPos != 1 && gearPos != 0) {

        if(shift = NO_SHIFT) { 
          tft.fillRect(2,2,480,85,GREEN);
          tft.setCursor(80, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("              ");
        }
        else if (shift = PREP_SHIFT) {
          tft.fillRect(2,2,480,85,YELLOW);

          tft.setCursor(80, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("GET READY");
        }      
        else if (shift = UPSHIFT) {
          tft.fillRect(2,2,480,85,RED);

          tft.setCursor(80, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("UPSHIFT NOW");
        }  
        else {
          tft.fillRect(2,2,480,85,CYAN);

          tft.setCursor(80, 150);
          tft.setTextSize(1);
          SHIFT_MSG.print("DOWNSHIFT");
        } 
      }
    }
  }
}

*/

