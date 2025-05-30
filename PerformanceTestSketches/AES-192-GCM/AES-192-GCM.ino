#include <Crypto.h>
#include <AES.h>
#include <GCM.h>

#define KEY_SIZE 192/8
#define NONCE_SIZE 12
#define TAG_SIZE 16
#define BLOCK_SIZE 16
#define PLAINTEXT_SIZE 256
#define ITERATIONS 1000
#define AD_SIZE (1 << 8) // Bit shift - 0 = 2**0, 1 = 2**1....

const int ledPin = 13; // Built-in LED on most Arduino boards

// Buffers
uint8_t key[KEY_SIZE];
uint8_t nonce[NONCE_SIZE];
uint8_t plaintext[PLAINTEXT_SIZE];
uint8_t ciphertext[PLAINTEXT_SIZE + TAG_SIZE];
uint8_t decrypted[PLAINTEXT_SIZE];
uint8_t ad[AD_SIZE]; // Optional Associated Data (AD)

String incomingMessage;

AES192 aes;
GCM<AES192> gcm;

void initRand(uint8_t* buff, size_t size) {
  for (int i = 0; i < size; i++) {
    buff[i] = random(0, 256);
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  while(1) {
    if (Serial.available() > 0) {
      // Read incoming bytes until a newline character is found
      incomingMessage = Serial.readStringUntil('\n');

        // Check if the message matches "START"
      if (incomingMessage == "START") {
        break;  // Exit the loop
      }
    } 
  }


  randomSeed(analogRead(A0));

  unsigned long EncryptDeltaT;
  unsigned long DecryptDeltaT;

  size_t ad_length = 0;
  size_t plaintext_length = PLAINTEXT_SIZE;

  while(ad_length <= AD_SIZE) {
    EncryptDeltaT = 0;
    DecryptDeltaT = 0;

    Serial.print("AES-128-GCM AD_SIZE:");
    Serial.println(ad_length);

    for (int z = 0; z < ITERATIONS; z++) {
      initRand(key, sizeof(key));
      initRand(nonce, sizeof(nonce));
      initRand(plaintext, plaintext_length);
      initRand(ad, sizeof(ad));

      gcm.setKey(key, sizeof(key));
      gcm.setIV(nonce, sizeof(nonce));
      gcm.addAuthData(ad, ad_length);

      unsigned long e_t0 = micros();
      gcm.encrypt(ciphertext, plaintext, plaintext_length);
      gcm.computeTag(ciphertext + plaintext_length, TAG_SIZE);
      unsigned long e_t1 = micros();

      gcm.setKey(key, sizeof(key));
      gcm.setIV(nonce, sizeof(nonce));
      gcm.addAuthData(ad, ad_length);

      unsigned long d_t0 = micros();
      gcm.decrypt(decrypted, ciphertext, plaintext_length);
      bool valid = gcm.checkTag(ciphertext + plaintext_length, TAG_SIZE);
      unsigned long d_t1 = micros();

      EncryptDeltaT += (e_t1 - e_t0);
      DecryptDeltaT += (valid ? (d_t1 - d_t0) : 0); // Skip timing if tag failed

      Serial.print(z);
      Serial.print(", ");
      Serial.print(e_t1 - e_t0);
      Serial.print(", ");
      Serial.println(valid ? (d_t1 - d_t0) : -1);
    }
    ad_length = ad_length ? (ad_length << 1) : 1;
  }
  Serial.println("#");
  digitalWrite(ledPin, HIGH);
}

void loop() {
  // Nothing to do in loop
}
