// ****************************************************************
// *                                                              *
// *     PPM to XBUS (JR and Multiplex SRXL V2) protocol          *
// *                                                              *
// * V0.1                                                         *
// ****************************************************************

// Arduino micro Pro (ATMega328 5V 16 Mhz board)
// input PPM signal on pin capable of Input Capture (interruption)
// outpur XBUS protocol on UART TX pin
#include <PPMReader.h>

#define DEBUG
//#define DEBUGPPM

//choose your serial protocol
//#define GRAUPNER                                                // send datas using JR protocol (XBUS). (channels from 0x1c20 (900 µs) to 0x41a0 (2100 µs).  (equivalent to PPM µs * 8))
//#define MPX_V1                                               // send datas using Multiplex SRLX protocol (channels from 0x000 (800µs) to 0xFFF (2200 µs)
#define MPX_V2

#define interruptPin          2                           // only pins 2 and 3 are interrupt-capable
#define BAUD                  115200


#define PPM_NB_CHANNELS       8                           // up to 8 channels for the library
#define PPM_MIN               850                         // PWM pulse in µs
#define PPM_MAX               2150                        // PWM pulse un µs
#define PPM_BLANK             2200

#define GRAUPNER_SERIAL_NB_CHANNELS    8
#define GRAUPNER_START_BYTE            0xA8
#define GRAUPNER_HEADER_BYTE           0x01
#define MPX_V1_SERIAL_NB_CHANNELS   12
#define MPX_V1_START_BYTE           0xA1
#define MPX_V2_SERIAL_NB_CHANNELS   16
#define MPX_V2_START_BYTE           0xA2

#ifdef GRAUPNER
  #define SERIAL_NB_CHANNELS      GRAUPNER_SERIAL_NB_CHANNELS
  #define START_BYTE              GRAUPNER_START_BYTE
  #define HEADER_BYTE             GRAUPNER_HEADER_BYTE
#endif
#ifdef MPX_V1
  #define SERIAL_NB_CHANNELS      MPX_V1_SERIAL_NB_CHANNELS
  #define START_BYTE              MPX_V1_START_BYTE
  #define MPX
#endif
#ifdef MPX_V2
  #define SERIAL_NB_CHANNELS      MPX_V2_SERIAL_NB_CHANNELS
  #define START_BYTE              MPX_V2_START_BYTE
  #define MPX
#endif

// ******************** GLOBAL VARIABLES **************************
unsigned int crc;

// ************************ FUNCTIONS *****************************
#define CRC_POLYNOME 0x1021
// CRC calculation, adds a 8 bit unsigned to 16 bit crc
static void CRC16(uint8_t value)
{
    uint8_t i;

    crc = crc ^ (int16_t)value << 8;
    for (i = 0; i < 8; i++) {
    if (crc & 0x8000)
        crc = (crc << 1) ^ CRC_POLYNOME;
    else
        crc = (crc << 1);
    }
}



// **************************** MAIN ******************************
PPMReader ppm(interruptPin, PPM_NB_CHANNELS );


void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD);
  ppm.minChannelValue=PPM_MIN;
  ppm.maxChannelValue=PPM_MAX;
  ppm.blankTime = PPM_BLANK;

  #ifdef DEBUG
  Serial.println();
  Serial.println("PPM -> SUMD or SRXL converter");

  //delay(5);
  #ifdef GRAUPNER
      Serial.println("GRAUPNER SUMD protocol");
  #endif
  #ifdef MPX_V1
      Serial.println("MULTIPLEX SRXL V1 Protocol");
  #endif
  #ifdef MPX_V2
      Serial.println("MULTIPLEX SRXL V2 Protocol");
  #endif

  #endif
  Serial.flush();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Print latest valid values from all channels
  #ifdef DEBUGPPM
    for (byte channel = 1; channel <= PPM_NB_CHANNELS; ++channel) {
      unsigned value = ppm.latestValidChannelValue(channel,0);
      Serial.print(value);
      if(channel < PPM_NB_CHANNELS) Serial.print('\t');
    }
    Serial.println();
    delay(20);
  #endif


  crc=0;                                                          //reset crc checksum to 0 before calculation
  // start of transmission
  Serial.write(START_BYTE);
  CRC16(START_BYTE);                                              // calculation of crc after each byte send
  #ifdef GRAUPNER
    Serial.write(HEADER_BYTE);
    CRC16(HEADER_BYTE);
    Serial.write(SERIAL_NB_CHANNELS);
    CRC16(SERIAL_NB_CHANNELS);
  #endif
  for (char i=1;i<=SERIAL_NB_CHANNELS; i++) {                     //send each channel data in 2 bytes
    unsigned int data=ppm.latestValidChannelValue(i, 0);
    #ifdef GRAUPNER
      data = data<<3;                                                // modification of value to be compliant with GRAUPNER SUMD protocol (JR format) (data = µs * 8)
    #endif
    #ifdef MPX
      if (i<=PPM_NB_CHANNELS) {
        data = data*32/11 -2340+8;                                 // modification of value to be complient with MPX format (0x0000 to 0xFFF, neutral = 0x800)
                                                                  // real formula is µs * 2,925714 - 2340. but we have to manage 16 bits integer capacity from 0 to 65535
        constrain(data, 0x0000, 0x0FFF);
      } else {
        data=0x800;
      }
    #endif
    Serial.write( data >> 8);
    CRC16( data >>8);
    Serial.write((char) data);
    CRC16( (char)data );
  }
  //Send CRC Checkshum
  Serial.write(crc >> 8);
  Serial.write((char) crc);
  // end of transmission
  delay(20);

}
