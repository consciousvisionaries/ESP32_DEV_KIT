#include <DFRobotDFPlayerMini.h>  // Make sure you have this library installed

// Software serial for MP3 module
HardwareSerial mp3Serial(2); // RX, TX uart2 rx 16 tx 17
DFRobotDFPlayerMini mp3Player;

void setupMP3Player() {

    // Initialize MP3 player
    mp3Serial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX); 
    if (!mp3Player.begin(mp3Serial)) {
        Serial.printf("MP3 Player not detected.\n");
        while (true);  // Halt if MP3 Player is not detected
    }

    Serial.printf("MP3 Player ready.\n");
    mp3Player.volume(30);
    mp3Player.play(1); // Play the first MP3 file (replace with your file index)
}

void playMP3(int songNumber, int mp3volume) {
  
    mp3Player.play(songNumber);
    
    Serial.printf("Playing MP3 file number: %d\n", songNumber);

    // Only set volume if a valid value is provided
    if (mp3volume >= 0) {
        mp3Player.volume(mp3volume);
        Serial.printf("Volume set to: %d\n", mp3volume);
    }
}
