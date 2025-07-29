#include <BluetoothSerial.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <HMC5883L.h>

#define COMPASS_AVG 10

// Hardware Serial for external communication
#define UART_PORT Serial2
#define UART_TX_PIN 17
#define UART_RX_PIN 16

// Bluetooth Serial
BluetoothSerial SerialBT;

// Command identifiers
#define BUTTON_CMD 0xAC
#define COMPASS_CMD 0xAD
#define LEFT_STICK_CMD 0xAA
#define RIGHT_STICK_CMD 0xAB

// Button data structure
uint8_t BUTTON_DATA[9] = {BUTTON_CMD, 0, 0, 0, 0, 0, 0, 0, 0};

// Custom button states (1 byte per button)
uint8_t custom_button_states[8] = {0};

// Compass variables
HMC5883L compass;
float angleCompassStartPoint = 0.0f;
float previousAzimuth = 0;
float accumulatedChange = 0;

// Mode control
enum Mode { STEERING, ROTATION };
Mode currentMode = STEERING;

// Current control values
float current_angle = 0;
float current_speed = 0;
int current_direction = 0;
float current_radius = 0;
float current_angular_speed = 0;

float filtered = 0;

float compass_read()
{
  float headingDegrees = 0.0f;
  for (uint8_t x = 0 ; x < 10 ; x++) 
  {
    Vector norm = compass.readNormalize();
    float heading = atan2(norm.YAxis, norm.XAxis);

    float declinationAngle = (4.0 + (49.0 / 60.0)) / (180 / M_PI);
    heading += declinationAngle;

    // Correct for heading < 0deg and heading > 360deg
    if (heading < 0)
    {
      heading += 2 * PI;
    }

    if (heading > 2 * PI)
    {
      heading -= 2 * PI;
    }

    // Convert to degrees
    headingDegrees += heading * (180/M_PI);  
  }
  headingDegrees /= COMPASS_AVG;


  return headingDegrees ;
}

void setup() {
  // Initialize serial ports
  Serial.begin(115200);
  SerialBT.begin("ESP32_Slave");
  UART_PORT.begin(115200, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  Serial.println("Initialize HMC5883L");
  while (!compass.begin())
  {
    Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
    delay(500);
  }

  // Set measurement range
  compass.setRange(HMC5883L_RANGE_1_3GA);

  // Set measurement mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_30HZ);

  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_8);

  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(-122, -138, -103, 0.97, 0.94, 1.11);

  Serial.println("Compass Initialized Successfully");

  // Calculate initial compass heading
  for (uint8_t i = 0; i < 5; i++) {
    
    angleCompassStartPoint += compass_read();
    //Serial.println(angleCompassStartPoint);
    //delay(10);
  }
  angleCompassStartPoint /= 5;

  Serial.println("System Initialized - Ready for Bluetooth Commands");
}

void loop() {
  processCompass();
  processBluetooth();
  //delay(10);
}

void processCompass() {
  float azimuth_read = compass_read();
  float azimuth = angleCompassStartPoint - azimuth_read;
  
  if (azimuth < 0) azimuth += 360;
  
  float difference = azimuth - previousAzimuth;
  
  if (difference > 180) difference -= 360;
  else if (difference < -180) difference += 360;
  
  accumulatedChange += difference;
  previousAzimuth = azimuth;
  
  if (difference != 0) {
    sendCompassData(accumulatedChange);
  }
  Serial.print("Compass angle sent: ");
  Serial.println(accumulatedChange);
}

void processBluetooth() {
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    command.trim();
    Serial.println("Received: " + command);

    // Parse mode commands
    if (command == "MODE:STEERING") {
      currentMode = STEERING;
      Serial.println("Mode set to Steering");
    }
    else if (command == "MODE:ROTATION") {
      currentMode = ROTATION;
      Serial.println("Mode set to Rotation");
    }
    else if (command.startsWith("BUTTONS:")) {
      // Parse button states command "BUTTONS:1,0,1,0,0,0,1,0"
      int colonPos = command.indexOf(':');
      String statesStr = command.substring(colonPos + 1);
      
      int startIndex = 0;
      int endIndex = statesStr.indexOf(',');
      
      for (int i = 0; i < 8; i++) {
        if (endIndex == -1) endIndex = statesStr.length();
        
        String stateStr = statesStr.substring(startIndex, endIndex);
        custom_button_states[i] = stateStr.toInt();
        
        startIndex = endIndex + 1;
        endIndex = statesStr.indexOf(',', startIndex);
      }
      
      // Send button states via UART
      sendButtonStates();
      
      Serial.println("Button states updated:");
      for (int i = 0; i < 8; i++) {
        Serial.printf("Button %d: %d\n", i+1, custom_button_states[i]);
      }
    }
    // Parse combined steering command
    else if (command.startsWith("STEER:")) {
      int colon1 = command.indexOf(':');
      int colon2 = command.indexOf(':', colon1 + 1);
      int colon3 = command.indexOf(':', colon2 + 1);
      
      if (colon1 != -1 && colon2 != -1 && colon3 != -1) {
        current_angle = command.substring(colon1 + 1, colon2).toFloat();
        current_speed = command.substring(colon2 + 1, colon3).toFloat();
        current_direction = command.substring(colon3 + 1).toInt();
        
        current_angle = -1 * constrain(current_angle, -90, 90);
        current_speed = constrain(current_speed, 0, 100);
        if (current_direction == 1)
        {
          if (current_angle < 0)  current_angle += 360;
        }
        else if (current_direction == -1)
        {
          current_angle += 180;
        }
      
        sendIntegerByteByByte(currentMode == STEERING ? LEFT_STICK_CMD : 0X00);
        sendFloatByteByByte(current_speed);
        sendFloatByteByByte(current_angle);
        
        Serial.print("Command - Angle: ");
        Serial.print(current_angle);
        Serial.print(", Speed: ");
        Serial.print(current_speed);
        Serial.print(", Direction: ");
        Serial.println(current_direction);
      }
    }
      // Parse combined steering command
    else if (command.startsWith("ROTATE:")) {
      int colon1 = command.indexOf(':');
      int colon2 = command.indexOf(':', colon1 + 1);
      int colon3 = command.indexOf(':', colon2 + 1);
      
      if (colon1 != -1 && colon2 != -1 && colon3 != -1) {
        current_radius = command.substring(colon1 + 1, colon2).toFloat();
        current_angular_speed = command.substring(colon2 + 1, colon3).toFloat();
        current_direction = command.substring(colon3 + 1).toInt();
        
        current_radius = constrain(current_radius, -100, 100);
        current_angular_speed = constrain(current_angular_speed, 0, 100);
        
        if (current_direction == -1)
        {
          current_angular_speed *= -1;
        }
      
        sendIntegerByteByByte(currentMode == ROTATION ? RIGHT_STICK_CMD : 0X00);
        sendFloatByteByByte(current_angular_speed);
        sendFloatByteByByte(current_radius);
        
        Serial.print("Command - Radius: ");
        Serial.print(current_radius);
        Serial.print(", Angular Speed: ");
        Serial.print(current_angular_speed);
        Serial.print(", Direction: ");
        Serial.println(current_direction);
      }
    }
  }
}

void sendButtonStates() {
  // Send BUTTON_CMD (0xAC) followed by 8 state bytes
  UART_PORT.write(BUTTON_CMD);
  for (int i = 0; i < 8; i++) {
    UART_PORT.write(custom_button_states[i]);
  }
}

void sendIntegerByteByByte(byte num) {
  UART_PORT.write(num);
}

void sendFloatByteByByte(float value) {
  union {
    float x;
    uint8_t data[4];
  } floatUnion;
  
  floatUnion.x = value;
  for (int i = 0; i < 4; i++) {
    UART_PORT.write(floatUnion.data[i]);
  }
}

void sendButton(uint8_t *value) {
  for (int i = 0; i < 9; i++) {
    UART_PORT.write(*(value + i));
  }
}

void sendCompassData(float compass_value) {
  union {
    float angle;
    uint8_t data[4];
  } compassUnion;
  
  compassUnion.angle = compass_value;
  UART_PORT.write(COMPASS_CMD);
  for (int i = 0; i < 4; i++) {
    UART_PORT.write(compassUnion.data[i]);
  }
  for (int i = 0; i < 4; i++) {
    UART_PORT.write(0);
  }
}