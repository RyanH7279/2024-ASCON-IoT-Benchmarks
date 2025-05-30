#include <Crypto.h>
#include <AES.h>

// Define the block size for AES (16 bytes for AES-128)
#define KEY_SIZE 192/8
#define BLOCK_SIZE 16
#define BLOCK_CNT 16
#define ITERATIONS 1000

const int ledPin = 13; // Built-in LED on most Arduino boards

// Create AES object for block encryption
AES192 aes;

// Key (16 bytes of 0x01)
uint8_t key[KEY_SIZE];

// Plaintext (32 bytes for 2 blocks)
uint8_t plaintext[BLOCK_SIZE * BLOCK_CNT];
uint8_t decypttext[BLOCK_SIZE * BLOCK_CNT];

// Buffer for ciphertext (same size as plaintext)
uint8_t ciphertext[BLOCK_SIZE * BLOCK_CNT];

void initRand(uint8_t* buff, size_t size) {
  for(int i = 0; i < size; i++) {
    buff[i] = random(0, 256);
  }
}

void printHex(const uint8_t* data, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (data[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");

    // Print a new line after each block
    if ((i + 1) % BLOCK_SIZE == 0) {
      Serial.println();
    }
  }
  Serial.println();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  while (!Serial); // Wait for Serial to be ready
  
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

  unsigned long EncryptDeltaT = 0;
  unsigned long DecryptDeltaT = 0;

  Serial.println("AES-192-ECB");
    
  for (int z = 0; z < ITERATIONS; z++) {

    // encryption first

    // Random Key
    initRand(plaintext, sizeof(plaintext));

    initRand(key, sizeof(key));

    aes.setKey(key, sizeof(key));

    uint8_t encryptedCounter[BLOCK_SIZE]; // Buffer for the encrypted counter

    unsigned long e_t0 = micros();
    for (int i = 0; i < BLOCK_SIZE * BLOCK_CNT; i += BLOCK_SIZE) {
        aes.encryptBlock(ciphertext + i, plaintext + i);
    }
    unsigned long e_t1 = micros();

    unsigned long d_t0 = micros();
    for (int i = 0; i < BLOCK_SIZE * BLOCK_CNT; i += BLOCK_SIZE) {
        aes.decryptBlock(decypttext + i, ciphertext + i);
    }
    unsigned long d_t1 = micros();

    EncryptDeltaT = EncryptDeltaT + (e_t1-e_t0);
    DecryptDeltaT = DecryptDeltaT + (d_t1-d_t0);

    Serial.print(z);
    Serial.print(", ");
    Serial.print(e_t1-e_t0);
    Serial.print(", ");
    Serial.println(d_t1-d_t0);
  }

  Serial.println("#");
  digitalWrite(ledPin, HIGH);
}

void loop() {
  // Nothing to do in loop
}
