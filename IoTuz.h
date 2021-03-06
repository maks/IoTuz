// This code can work somewhat on the WROVER board, you can comment this out:
//#define WROVER

// MartyMacGyver/ESP32-Digital-RGB-LED-Drivers is better than the Adafruit 
// Neopixel library. It flickers a bit, but if you want it, there you go:
//#define NEOPIXEL

#ifndef IOTUZ_H
#define IOTUZ_H

#include <Wire.h>
#include <SPI.h>

#include <AikoEvents.h>
using namespace Aiko;

#include <Adafruit_GFX.h>
#ifdef WROVER
#include "WROVER_KIT_LCD.h"
// Some code uses the Adafruit_ILI9341 interface for tft, fix this here
#define Adafruit_ILI9341 WROVER_KIT_LCD
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#else
// You need a recent github version of this library, including this patch:
// https://github.com/adafruit/Adafruit_ILI9341/pull/26
#include <Adafruit_ILI9341.h>
#endif

// https://learn.adafruit.com/adafruit-neopixel-uberguide/
// Support for APA106 RGB LEDs
// APA106 is somewhat compatible with WS2811 or WS2812 (well, maybe not 100%, but close enough to work)
// Note that bit banging in that library does not work well because ESP32 is not a real
// time processor and may interrupt the arduino task in the middle of a bit bang and 
// cause delays that end up sending the wrong color.
#ifdef NEOPIXEL
#include "Adafruit_NeoPixel.h"
#else
// Instead use this library that was designed to use the ESP32 IO hardware
// https://github.com/MartyMacGyver/ESP32-Digital-RGB-LED-Drivers.git
#include "ws2812.h"
#endif

// Accelerometer
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// Temp/Humidity/Pressure
#include <Adafruit_BME280.h>

// Touch screen https://github.com/PaulStoffregen/XPT2046_Touchscreen
#include <XPT2046_Touchscreen.h>

// https://github.com/marcmerlin/Arduino-IRremote
#include <IRremote.h>
#include "IRcodes.h"

// https://github.com/CCHS-Melbourne/iotuz-esp32-hardware/wiki has hardware mapping details
// On ESP32, those IO pins do not exist: 6 7 8 10 11 20 24 28 29 30 31
// These I/O pins are unused on the board design, but they are input only: 37 and 38

/*
I2C addresses:
Audio:  0x1A
IO_EXP: 0x20 => Spec sheet says 
"To enter the Read mode the master (microcontroller) addresses the slave
device and sets the last bit of the address byte to logic 1 (address byte read)"
The Wire lib automatically uses the right I/O port for read and write.
ADXL:   0x53
BME280: 0x77 (Temp/Humidity/Pressure)
*/

#define IR_RX_PIN 35
#define IR_TX_PIN 26

// Joystick Setup
#define JOYSTICK_X_PIN 39
#define JOYSTICK_Y_PIN 34
#define JOYSTICK_BUT_PIN 0

// Pins unused by this code
#define I2C_EXPANDER_INT 25
#define ADC_OUT	    27
#define SD2_SPI	    9
#define I2S_MCLK    16
#define I2S_DAC_IN  17
#define I2S_DCLK    5
#define I2S_FS	    18
#define UART_RX	    1
#define UART_TX	    3

// TFT + Touch Screen Setup Start
#define TFT_DC 4
#define TFT_CS 19
#define TFT_RST 32
// SPI Pins are shared by TFT, touch screen, SD Card
#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_CLK 14


// Rotary Encocer
#define ENCODERA_PIN 15
#define ENCODERB_PIN 36

#define BAT_PIN 2

// APA106 LEDs
#define RGB_LED_PIN 23
#define NUMPIXELS 2

// LCD brightness control and touchscreen CS are behind the port
// expander, as well as both push buttons
#define I2C_EXPANDER 0x20	//0100000 (7bit) address of the IO expander on i2c bus

// Some I/O pins are behind the I/O expander
/* Port expander PCF8574, access via I2C on */
#define I2CEXP_ACCEL_INT    0x01	// (In)
#define I2CEXP_A_BUT	    0x02	// (In)
#define I2CEXP_B_BUT	    0x04	// (In)
#define I2CEXP_ENC_BUT	    0x08	// (In)
#define I2CEXP_SD_CS	    0x10	// (Out)
#define I2CEXP_TOUCH_INT    0x20	// (In)
#define I2CEXP_TOUCH_CS	    0x40	// (Out)
#define I2CEXP_LCD_BL_CTR   0x80	// (Out)

// Dealing with the I/O expander is a bit counter intuitive. There is no difference between a
// write meant to toggle an output port, and a write designed to turn off pull down resistors and trigger
// a read request.
// The write just before the read should have bits high on the bits you'd like to read back, but you
// may get high bits back on other bits you didn't turn off the pull down resistor on. This is normal.
// Just filter out the bits you're trying to get from the value read back and keep in mind that when
// you write, you should still send the right bit values for the output pins.
// This is all stored in i2cexp which we initialize to the bits used as input:
#define I2CEXP_IMASK ( I2CEXP_ACCEL_INT + I2CEXP_A_BUT + I2CEXP_B_BUT + I2CEXP_ENC_BUT + I2CEXP_TOUCH_INT )

// Touch screen select is on port expander line 6, not directly connected, so the library
// cannot toggle it directly. It however requires a CS pin, so I'm giving it 33, a spare IO
// pin so that it doesn't break anything else.
// CS is then toggled manually before talking to the touch screen.
#define TS_CS_PIN  33

//
// WROVER defines here, please connect at least a rotary encoder or joystick
// or you will be stuck in the main menu with no way to move.
// 
#ifdef WROVER
#define TFT_DC 21
#define TFT_CS 22
#define TFT_RST 18
#define SPI_MISO 25
#define SPI_MOSI 23
#define SPI_CLK 19
#define LCD_BL_CTR 5
#define TFT_CS 0

//
// You can plug a neopixel here:
#define RGB_LED_PIN 0

// Infrared is not needed either, but works for me.
// You would have to enter your own hex codes in IRcodes.h
#define IR_RX_PIN 35
 
// Do not use pin 12, this blocks the board. Likely avoid 13/14 too (SPI)
// 32, 33, 34 do not seem to work either for IO input,
// joystick is recommended to play the games
#define JOYSTICK_BUT_PIN 27
#define JOYSTICK_X_PIN 39
#define JOYSTICK_Y_PIN 36
// but you can navigate the menu with a rotary encoder instead
#define ENCODERA_PIN 25
#define ENCODERB_PIN 26
#define ENCODER_BUT_PIN 15

// My WROVER board uses a ST7789V instead of ILI9341, the TFT init returns this:
// ILI9341 Test!
// Display Power Mode: 0xCE
// MADCTL Mode: 0x0
// Pixel Format: 0x2
// Image Format: 0x0
// Self Diagnostic: 0xE0
// Resolution: 320 x 240

// ILI driver works on rover boards with an ILI chip, but newer boards use ST7789V
// so use the WROVER driver that handles both.
extern WROVER_KIT_LCD tft;
#else
extern Adafruit_ILI9341 tft;
#endif

#ifdef NEOPIXEL
extern Adafruit_NeoPixel pixels;
#else
extern rgbVal pixels[NUMPIXELS];
#endif
extern Adafruit_ADXL345_Unified accel;
extern Adafruit_BME280 bme;
extern XPT2046_Touchscreen ts;
extern IRrecv irrecv;
extern decode_results IR_result;

typedef enum {
    BUT_DOWN = 0,
    BUT_PUSHED = 1,
    BUT_UP = 2,
    BUT_RELEASED = 3,

} ButtState;

class IoTuz {
  public:
    IoTuz();

    // tft_width, tft_height, calculated in setup after tft init
    uint16_t tftw, tfth;

    // Actual analog values, 0 to 4096
    uint16_t joyValueX, joyValueY;
    // massaged values to remove dead center and return -5 to +5
    int8_t joyRelX, joyRelY;
    bool joyBtn;

    // Some touch screens have reversed coordinates. The original IoTuz did not
    // but the compatible replacement screen I got, is reversed.
    bool ts_revX = false;
    bool ts_revY = false;
    // This is calibration data for the raw touch data to the screen coordinates
    uint16_t ts_minX=320;
    uint16_t ts_minY=220;
    uint16_t ts_maxX=3920;
    uint16_t ts_maxY=3820;
    uint16_t minpressure=500;
    uint16_t maxpressure=3000;

    // Buffer to store strings going to be printed on tft
    char tft_str[64];

    void enable_aiko_ISR();
    void i2cexp_clear_bits(uint8_t);
    void i2cexp_set_bits(uint8_t);
    uint8_t i2cexp_read();
    int16_t read_encoder();
    int8_t encoder_changed();
    ButtState butEnc();
    ButtState butA();
    ButtState butB();
    void read_joystick(bool showdebug=false);
    float battery_level();
    void screen_bl(bool);
    void reset_tft();
    void tftprint(uint8_t, uint8_t, uint8_t, char *);
    TS_Point get_touch();
    void touchcoord2pixelcoord(uint16_t *, uint16_t *, uint16_t);
    void calibrateScreen();
    void begin();

  private:
    uint8_t _i2cexp;
    void pcf8574_write_(uint8_t);
    ButtState _but(uint8_t);
    void _getMinMaxTS();
};



#endif

// vim:sts=4:sw=4
