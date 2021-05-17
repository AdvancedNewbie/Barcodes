//Install Adafruit GFX, esp32, Adafruit BusIO
//T5_V2.3_2.13

#include "WiFi.h"
#include "WebServer.h"

//#include "SD.h"
#include "SPI.h"
#include <GxEPD.h>  //Use 1.7.5
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include "EEPROM.h"

#define MAX_EEPROM_SCREENS    10
int NumSavedScreens = 0;

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
    return i;
}

#include "root_page.h"

const char* apssid = "unknown";
const char* appassword = "1234567890";

//#include <GxGDEH029A1/GxGDEH029A1.h> // 2.9" b/w 296 x 128 Pixel
#include <GxGDE0213B72B/GxGDE0213B72B.h>      // 2.13" b/w

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#define SPI_MOSI        23
#define SPI_MISO        2
#define SPI_CLK         18

#define ELINK_SS        5
#define ELINK_BUSY      4
#define ELINK_RESET     16
#define ELINK_DC        17

#define SDCARD_SS       13

#define BUTTON_1        39
//#define BUTTON_2        38
//#define BUTTON_3        39

#define VBAT_IO         35

#define SPEAKER_OUT     25

#define MAX_DIGITS      12

#define MAX_SCREEN_HEIGHT     122
#define MAX_SCREEN_WIDTH      250
#define X_START               30

static int X_Position = X_START; //X position for barcode

GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);
// SPIClass sdSPI(VSPI);

static void InitBarCode ( void );
static void drawCode( int c );
static void drawTest( void );
static void clearScreen( void );
static void drawStrip( int StripType );

enum {
  SPACE,
  BAR,
  SME_BAR
};

WebServer server(80);
static void handleRoot();
static void handleFormAction();
static void handleNotFound();



void setup()
{
  pinMode(BUTTON_1, INPUT);
  //    pinMode(BUTTON_2, INPUT);
  //    pinMode(BUTTON_3, INPUT);
  
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apssid, appassword); // Start AP mode
  
  IPAddress myIP = WiFi.softAPIP(); // IP address for AP mode
  Serial.print("AP IP address: ");
  Serial.println(myIP);
    
  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
  
  display.init();
  display.setRotation(1);

  server.on("/", handleRoot);
  server.on("/submit", handleFormAction);
  server.onNotFound(handleNotFound);
  server.begin();
  
  //drawTest();
   
}

void loop()
{
  server.handleClient();
  
  if (digitalRead(BUTTON_1) == LOW) {
    
    clearScreen();
        
    //drawTest();
    
    delay(500);
  }

      /* VBAT mesaurement */
//  int Vbat = analogRead(VBAT_IO);
//  Vbat = map(Vbat, 0, 2580, 0, 100);
//  Serial.println(Vbat);
}

static void drawTest( void )
{
  // Set Font
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  
  /* Draw Top Text */
  char string[] = "Top Text\0";
  
  display.setCursor( MAX_SCREEN_WIDTH - (strlen(string) * 14), 10 );
  display.println( string );

  /* Draw Code */
  //drawCode("060383758783");
  //drawCode("605388881243");
  drawCode("628915166095");

  /* Draw Bottom Text */
  strcpy(string, "Bottom Text\0");
  display.setCursor( MAX_SCREEN_WIDTH - (strlen(string) * 14), MAX_SCREEN_HEIGHT - 5 );
  display.println( string );

    /* VBAT mesaurement */
  int Vbat = analogRead(VBAT_IO);
  Vbat = map(Vbat, 0, 2580, 0, 100);
  String vbatString = String(Vbat);
  
  display.setCursor( MAX_SCREEN_WIDTH - 30, MAX_SCREEN_HEIGHT - 5 );
  display.println( vbatString.c_str() );
  
  //display.updateWindow(1, 8, 62, 54, true);
  display.update();
}

static void drawCode( char barCodeValue[MAX_DIGITS] ){
  
  int digitIndex = 0;
  
  X_Position = X_START;
  
  /* Draw Start */
  drawStrip(SME_BAR); drawStrip(SPACE); drawStrip(SME_BAR);
  
  /* Draw Left Numerical Digit */
  for (int i=0; i < 6; i++)
  {
    int digit = barCodeValue[i] - 48;

    switch(digit)
    {
      case 0:
        drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR);
        break;
      case 1:
        drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR);
        break;
      case 2:
        drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR);
        break;
      case 3:
        drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR);
        break;
      case 4:
        drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR);
        break;
      case 5:
        drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR);
        break;
      case 6:
        drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(BAR); drawStrip(BAR);
        break;
      case 7:
        drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR);
        break;
      case 8:
        drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(BAR);
        break;
      case 9:
        drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR);
        break;
    }
  }

  /* Draw Middle */
  drawStrip(SPACE); drawStrip(SME_BAR); drawStrip(SPACE); drawStrip(SME_BAR); drawStrip(SPACE);

  /* Draw Right Numerical Digit */
  for (int i=6; i < 12; i++)
  {
    int digit = barCodeValue[i] - 48;

    switch(digit)
    {
      case 0:
        drawStrip(BAR); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE);
        break;
      case 1:
        drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE);
        break;
      case 2:
        drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE);
        break;
      case 3:
        drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE);
        break;
      case 4:
        drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE);
        break;
      case 5:
        drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE);
        break;
      case 6:
        drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE);
        break;
      case 7:
        drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE);
        break;
      case 8:
        drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE); drawStrip(SPACE);
        break;
      case 9:
        drawStrip(BAR); drawStrip(BAR); drawStrip(BAR); drawStrip(SPACE); drawStrip(BAR); drawStrip(SPACE); drawStrip(SPACE);
        break;
    }
  }

  /* Draw End */
  drawStrip(SME_BAR); drawStrip(SPACE); drawStrip(SME_BAR);
  

}

static void drawStrip( int StripType )
{
  #define Y_START     20
  #define HEIGHT      75
  #define SME_HEIGHT  HEIGHT + 5
  #define BAR_WIDTH   2   //How many pixels wide each bar/space will be

  for (int numBars = 0; numBars < BAR_WIDTH; numBars++ )
  {
   
    switch( StripType )
    {
      case SPACE:
        //display.drawLine( X, Y, X, Y + Height, 0 );
        break;
      case BAR:
        display.drawLine( X_Position, Y_START, X_Position, Y_START + HEIGHT, 0 );
        break;
      case SME_BAR:
        display.drawLine( X_Position, Y_START, X_Position, Y_START + SME_HEIGHT, 0 );
        break;
    }
  
    X_Position++;
  
  }
}

static void clearScreen( void )
{
  display.setCursor(0, 0);
  display.fillRect(0, 0, MAX_SCREEN_WIDTH, MAX_SCREEN_HEIGHT, GxEPD_WHITE);
  //display.updateWindow(0, 0, MAX_SCREEN_WIDTH, MAX_SCREEN_HEIGHT, true);
  display.update();
}

static void handleFormAction() {
  //server.send(200, "text/plain", "hello from esp8266!");
  
  String message;

  bool bSaveToEeprom = false;

  typedef struct{
    String topTextStr;
    String codeStr;
    String bottomTextStr; 
  } SCREEN_INFO_TYPE;
  
  SCREEN_INFO_TYPE ScreenInfo;

  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    
    if (server.argName(i) == "top")
    {
      ScreenInfo.topTextStr += server.arg(i); 
    }
    
    if (server.argName(i) == "code")
    {
      ScreenInfo.codeStr += server.arg(i); 
    }

    if (server.argName(i) == "bottom")
    {
      ScreenInfo.bottomTextStr += server.arg(i); 
    }

    if (server.argName(i) == "save" && server.arg(i) == "1")
    {
      bSaveToEeprom = true;
    }
  }

  if ( (ScreenInfo.topTextStr != "") | (ScreenInfo.codeStr != "") | (ScreenInfo.bottomTextStr != "") )
  {
    clearScreen();
  }

  if (ScreenInfo.topTextStr != "")
  {
    ScreenInfo.topTextStr += "\0";
    
    /* Draw Top Text */
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    
    //display.setCursor( 1, 10 );
    display.setCursor( 1, 10 );
    display.println( ScreenInfo.topTextStr.c_str() );
    message += "\nGOT NEW TOP TEXT!";
  }

  if (ScreenInfo.codeStr != "")
  {
    char checkSumTemp = (ScreenInfo.codeStr[0] - 48) + (ScreenInfo.codeStr[2] - 48) + (ScreenInfo.codeStr[4] - 48) + (ScreenInfo.codeStr[6] - 48) + (ScreenInfo.codeStr[8] - 48) + (ScreenInfo.codeStr[10] - 48);
    checkSumTemp *= 3;
    checkSumTemp += (ScreenInfo.codeStr[1] - 48) + (ScreenInfo.codeStr[3] - 48) + (ScreenInfo.codeStr[5] - 48) + (ScreenInfo.codeStr[7] - 48) + (ScreenInfo.codeStr[9] - 48);
    
    char mod = checkSumTemp % 10;

    char checkSum = 0;
    
    if (mod != 0)
    {
      checkSum = 10 - mod;
    }
    else
    {
      checkSum = checkSumTemp;
    }

    ScreenInfo.codeStr += (char)(checkSum + 48);
    
    drawCode( (char *)(ScreenInfo.codeStr.c_str()) );
    message += "\nGOT NEW CODE!";

    message += "\nCheckSum: ";
    message += (char)(checkSum + 48);

    message += "\nCode w\\ Checksum: ";
    message += ScreenInfo.codeStr;
  }

  if (ScreenInfo.bottomTextStr != "")
  {
    ScreenInfo.bottomTextStr += "\0";
    
    /* Draw Bottom Text */
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    
    //display.setCursor( 1, MAX_SCREEN_HEIGHT - 5 );
    display.setCursor( 1, MAX_SCREEN_HEIGHT - 5 );
    display.println( ScreenInfo.bottomTextStr.c_str() );
    message += "\nGOT NEW BOTTOM TEXT!";
  }

  /* VBAT mesaurement */
  int Vbat = analogRead(VBAT_IO);
  Vbat = map(Vbat, 0, 2580, 0, 100);
  String vbatString = String(Vbat);
  display.setCursor( MAX_SCREEN_WIDTH - 30, MAX_SCREEN_HEIGHT - 5 );
  display.println( vbatString.c_str() );
  

  if ( (ScreenInfo.topTextStr != "") | (ScreenInfo.codeStr != "") | (ScreenInfo.bottomTextStr != "") )
  {
    display.update();
  }

  if (bSaveToEeprom == true)
  {
    NumSavedScreens++;
    EEPROM_writeAnything(NumSavedScreens, ScreenInfo);
    EEPROM_writeAnything(0, NumSavedScreens);
  }
  
  
  server.send(200, "text/plain", message);
}

static void handleRoot()
{
  String TopPageStr = ROOT_Page_Top;
  String BottomPageStr = ROOT_Page_Bottom;

  String ContentStr = TopPageStr + BottomPageStr;
  
  server.send( 200, "text/html", ContentStr );
}

static void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  
  server.send(404, "text/plain", message);
}
