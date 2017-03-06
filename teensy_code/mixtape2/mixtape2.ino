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
int playing_now = 0;
int flag = 0;

void setup() {
  Serial.begin(9600);

  AudioMemory(8);

  pinMode(vol_pwm, OUTPUT);
  digitalWrite(vol_pwm, HIGH);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repeatedly
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

void play_song(String filename)
{
  Serial3.print("Playing file: ");
  Serial3.println(filename);

  char charBuf[50];
  filename.toCharArray(charBuf, 50);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(charBuf);

  // A brief delay for the library read WAV info
  delay(5);
  playing_now = 1;

  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
    if (Serial3.available()>0){
      break;
    }
  }
}

void stop_song() {
  playWav1.stop();
  flag = 0;
  playing_now = 0;
}

void pause_song() {
  playWav1.stop();
  playing_now = 0;
}

#define SONGNAME_LEN 20
char* song_array[] = {"BANMA.WAV",
                      "TUONI.WAV",
                      "UANDI.WAV",
                      "PKOPEN.WAV",
                      "PKOAK.WAV",
                      "PKWILD.WAV",
                      "GOAT.WAV",
                      "KESHA.WAV",
                      "LAZER.WAV"};
int size_var =sizeof(song_array) / sizeof(char *);

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

    if (in_data[0] == 'N'){
      flag += 1;

      if (flag >= size_var){
        flag = 0;
      }
      
      Serial3.println("NEXT SONG");
      Serial3.println(song_array[flag]);
    }
    else if (in_data[0] == 'L'){
      flag -= 1;

      if (flag < 0){
        flag = size_var - 1;
      }
      
      Serial3.println("LAST SONG");
      Serial3.println(song_array[flag]);
    }
    else if (in_data[0] == 'S'){
      Serial3.println("STOP SONG");
      stop_song();
    }
    else if (in_data[0] == 'P'){
      if (playing_now == 1){
        Serial3.println("PAUSE SONG");
        pause_song();
      }
      else {
        Serial3.println("PLAY SONG");
        playing_now = 1;
        Serial3.println(flag);
      }
    }

//    }

    // Clear input string by inserting nulls
    index = 0;
    for(index=0; index<array_len; index++)
    {
      in_data[index] = '\0';    
    }
  }

  if ((flag <= size_var) & (flag >= 0) & playing_now){
    play_song(song_array[flag]);
  }
  delay(500);
  
}


void software_reset(){
//  asm volatile ("  jmp 0");
  SCB_AIRCR = 0x05FA0004; // software reset
}



