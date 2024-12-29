


void setupMP3Player() {

// Initialize MP3 player
  mp3Serial.begin(9600, SERIAL_8N1, 17,16); // RX=16, TX=17
  if (!mp3Player.begin(mp3Serial)) {
    Serial.println("MP3 Player not detected.");
    while (true);  // Halt if MP3 Player is not detected
  }
    Serial.println("MP3 Player ready.");

    mp3Player.volume(30);
    mp3Player.play(1); // Play the first MP3 file (replace with your file index)
}
