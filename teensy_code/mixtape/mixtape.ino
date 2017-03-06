#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioPlaySdWav           playWav1;
// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
//AudioOutputI2S           audioOutput;
//AudioOutputSPDIF       audioOutput;
AudioOutputAnalog      audioOutput;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

int available_bytes;
const int array_len = 20;
char in_data[array_len];

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13
int vol_pwm = 20;

void setup() {
  Serial.begin(9600);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
//  sgtl5000_1.enable();
//  sgtl5000_1.volume(0.5);

  pinMode(vol_pwm, OUTPUT);
  digitalWrite(vol_pwm, HIGH);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  Serial3.begin(38400);  // HC-05 default speed in AT command more

  // Clear input string by inserting nulls
  int index = 0;
  for(index=0; index<array_len; index++)
  {
    in_data[index] = '\0';    
  }

  Serial.println("Printing hello...");
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);

  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
    if (Serial3.available()>0){
      break;
    }
  }
}

int flag = -1;

void loop() {
    // Enter main logic sequence when serial data is available
  if(Serial3.available()>0)
  {
    // Wait for all serial data to come in, get string length
    delay(50);
    available_bytes = Serial3.available();

    // Read in string
    int index = 0;
    for(int index=0; index<available_bytes; index++)
    {
      in_data[index] = Serial3.read();
    }
    Serial3.println(in_data);

    // if in_data[0] is 'R' reset arduino
    if (in_data[0] == 'B'){
      Serial3.println("BANMA");
      flag = 0;
    }
    // if in_data[0] is 'E' try to write to analog or digital pin
    else if (in_data[0] == '2'){
      Serial3.println("2");
      flag = 1;
    }
    // if in_data[0] is 'D' try to write to analog or digital pin
    else if (in_data[0] == '3'){
      Serial3.println("3");
      flag = 2;
    }
    else if (in_data[0] == '4'){
      Serial3.println("4");
      flag = 3;
    }
    else if (in_data[0] == 'T'){
      Serial3.println("TUONI");
      flag = 4;
    }
    else if (in_data[0] == 'U'){
      Serial3.println("UANDI");
      flag = 5;
    }
    else if (in_data[0] == 'P'){
      Serial3.println("PKOPEN");
      flag = 6;
    }
    else if (in_data[0] == 'O'){
      Serial3.println("PKOAK");
      flag = 7;
    }
    else if (in_data[0] == 'W'){
      Serial3.println("PKWILD");
      flag = 8;
    }
    else if (in_data[0] == 'G'){
      Serial3.println("GOAT");
      flag = 9;
    }
    else if (in_data[0] == 'K'){
      Serial3.println("KESHA");
      flag = 10;
    }
    else if (in_data[0] == 'L'){
      Serial3.println("LAZER");
      flag = 11;
    }
    else {
      flag = -1;
    }

//    }

    // Clear input string by inserting nulls
    index = 0;
    for(index=0; index<array_len; index++)
    {
      in_data[index] = '\0';    
    }
  }

  switch (flag) {
    case 0:
      playFile("BANMA.WAV");  // filenames are always uppercase 8.3 format
    case 1:
      playFile("SDTEST2.WAV");
    case 2:
      playFile("SDTEST3.WAV");
    case 3:
      playFile("SDTEST4.WAV");
    case 4:
      playFile("TUONI.WAV");
    case 5:
      playFile("UANDI.WAV");
    case 6:
      playFile("PKOPEN.WAV");
    case 7:
      playFile("PKOAK.WAV");
    case 8:
      playFile("PKWILD.WAV");
    case 9:
      playFile("GOAT.WAV");
    case 10:
      playFile("KESHA.WAV");
    case 11:
      playFile("LAZER.WAV");
    default: 
      Serial3.println("Waiting for message");
  }
  delay(500);
  
}


void software_reset(){
//  asm volatile ("  jmp 0");
  SCB_AIRCR = 0x05FA0004; // software reset
}



