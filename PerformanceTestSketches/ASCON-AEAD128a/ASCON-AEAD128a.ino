#include <ASCON.h>

// Define payload size
#define KEY_SIZE 128/8
#define PAYLOAD_SIZE 256
#define TAG_SIZE 16
#define NONCE_SIZE 16
#define ITERATIONS 1000
#define AD_SIZE 256

const int ledPin = 13; // Built-in LED on most Arduino boards

// Buffers for plaintext, ciphertext, and decrypted data
uint8_t plaintext[PAYLOAD_SIZE];
uint8_t ciphertext[PAYLOAD_SIZE + TAG_SIZE]; // Space for tag
uint8_t decrypted[PAYLOAD_SIZE];
uint8_t ad[AD_SIZE];

// Key and nonce for ASCON-128
uint8_t key[KEY_SIZE];
uint8_t nonce[NONCE_SIZE];

void initRand(uint8_t* buff, size_t size) {
  for(int i = 0; i < size; i++) {
    buff[i] = random(0, 256);
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for the Serial to initialize

  randomSeed(analogRead(A0));

  String incomingMessage;

  while (true) {
    if (Serial.available() > 0) {
      // Read incoming bytes until a newline character is found
      incomingMessage = Serial.readStringUntil('\n');

      // Check if the message matches "START"
      if (incomingMessage == "START") {
        break;  // Exit the loop
      }
    }
  }

  unsigned long EncryptDeltaT;
  unsigned long DecryptDeltaT;

  size_t ad_length = 0;

  while(ad_length <= AD_SIZE) {
    EncryptDeltaT = 0;
    DecryptDeltaT = 0;

    Serial.print("ASCON-128a AD_SIZE:");
    Serial.println(ad_length);

    for (int z = 0; z < ITERATIONS; z++) {
        initRand(key, sizeof(key));
        initRand(nonce, sizeof(nonce));
        initRand(plaintext, sizeof(plaintext));
        initRand(ad, ad_length);

        unsigned long e_t0 = micros();
        // Encrypt the plaintext
        size_t ciphertext_length;
        ascon128a_aead_encrypt(
        ciphertext, &ciphertext_length,
        plaintext, PAYLOAD_SIZE,
        ad, ad_length,
        nonce, key
        );
        unsigned long e_t1 = micros();
        
        unsigned long d_t0 = micros();
        // Decrypt the ciphertext
        size_t decrypted_length;
        int result = ascon128a_aead_decrypt(
        decrypted, &decrypted_length,
        ciphertext, ciphertext_length,
        ad, ad_length,
        nonce, key
        );
        unsigned long d_t1 = micros();

        EncryptDeltaT = EncryptDeltaT + (e_t1-e_t0);
        DecryptDeltaT = DecryptDeltaT + (d_t1-d_t0);

        Serial.print(z);
        Serial.print(", ");
        Serial.print(e_t1-e_t0);
        Serial.print(", ");
        Serial.println(d_t1-d_t0);
        }
        ad_length = ad_length ? (ad_length << 1) : 1;
    }
  Serial.println("#");
  digitalWrite(ledPin, HIGH);
}

void loop() {
  // Nothing to do in the loop
}

// Helper function to print buffers in hexadecimal format
void printHex(const uint8_t* data, size_t length) {
  for (size_t i = 0; i < length; i++) {
      if (data[i] < 0x10) Serial.print("0");
      Serial.print(data[i], HEX);
      Serial.print(" ");
      if ((i + 1) % 16 == 0) Serial.println(); // Newline every 16 bytes
  }
  Serial.println();
}
