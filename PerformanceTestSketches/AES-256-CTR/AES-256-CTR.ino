#include <Crypto.h>
#include <AES.h>

// Define the block size for AES (16 bytes for AES-128)
#define KEY_SIZE 256/8
#define BLOCK_SIZE 16
#define BLOCK_CNT 16
#define ITERATIONS 1000

const int ledPin = 13; // Built-in LED on most Arduino boards

// Create AES object for block encryption
AES256 aes;

// Key (16 bytes of 0x01)
uint8_t key[KEY_SIZE];

// IV/Counter (16 bytes, typically starts with a unique value for encryption)
uint8_t counter[BLOCK_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t counter2[BLOCK_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

// Plaintext (32 bytes for 2 blocks)
uint8_t plaintext[BLOCK_SIZE * BLOCK_CNT];
uint8_t decypttext[BLOCK_SIZE * BLOCK_CNT];

// Buffer for ciphertext (same size as plaintext)
uint8_t ciphertext[BLOCK_SIZE * BLOCK_CNT];

void incrementCounter(uint8_t* counter) {
  // Increment the counter value (handles overflow)
  for (int i = BLOCK_SIZE - 1; i >= 0; i--) {
    counter[i]++;
    if (counter[i] != 0) break; // Stop incrementing if no overflow
  }
}

void initPlain(uint8_t* plain, int shift) {
  for (int i = 0; i < BLOCK_SIZE * BLOCK_CNT; i++) {
    plain[i] = i + shift;
  }
}

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

  Serial.println("AES-256-CTR");
    
  for (int z = 0; z < ITERATIONS; z++) {

    // encryption first

    // Random Key
    initRand(plaintext, sizeof(plaintext));
    initRand(key, sizeof(key));
    aes.setKey(key, sizeof(key));

    uint8_t encryptedCounter[BLOCK_SIZE]; // Buffer for the encrypted counter

    unsigned long e_t0 = micros();
    for (int i = 0; i < BLOCK_CNT; i++) {
      aes.encryptBlock(encryptedCounter, counter);
      // XOR the encrypted counter with the plaintext block to produce ciphertext
      for (int j = 0; j < BLOCK_SIZE; j++) {
        ciphertext[i * BLOCK_SIZE + j] = plaintext[i * BLOCK_SIZE + j] ^ encryptedCounter[j];
      }
      // Increment the counter for the next block
      incrementCounter(counter);
    }
    unsigned long e_t1 = micros();

    unsigned long d_t0 = micros();
    for (int i = 0; i < BLOCK_CNT; i++) {
      aes.encryptBlock(encryptedCounter, counter2);
      // XOR the encrypted counter with the plaintext block to produce ciphertext
      for (int j = 0; j < BLOCK_SIZE; j++) {
        decypttext[i * BLOCK_SIZE + j] = ciphertext[i * BLOCK_SIZE + j] ^ encryptedCounter[j];
      }
      // Increment the counter for the next block
      incrementCounter(counter2);
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
