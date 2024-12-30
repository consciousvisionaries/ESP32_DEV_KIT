#include <DFRobotDFPlayerMini.h>  // Make sure you have this library installed

// Software serial for MP3 module
HardwareSerial mp3Serial(2); // RX, TX uart2 rx 16 tx 17
DFRobotDFPlayerMini mp3Player;

// files 1-8 individual clips
// files 9 complete clip
// file 10 success sound


int lastVolumeSet;

void setupMP3Player() {

    mp3Serial.end();
    //mp3Player.end();
    
    // Initialize MP3 player
    mp3Serial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX); 
    if (!mp3Player.begin(mp3Serial)) {
        Serial.printf("MP3 Player not detected.\n");
        while (true);  // Halt if MP3 Player is not detected
    }

    Serial.printf("MP3 Player ready.\n");
    mp3Player.volume(30);
    mp3Player.play(1); // Play the first MP3 file (Full Song)
}



void playMP3(int songNumber) {
  
   
    mp3Player.play(songNumber);
    Serial.printf("Playing MP3 file number: %d\n", songNumber);
    delay(50);

}
