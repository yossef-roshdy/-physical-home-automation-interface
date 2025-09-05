#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include <SoftwareSerial.h>

const int Password_Length = 4;
String text = "";
int i = 8;
float temp = 0;
String Data;
String pass = "1234";
byte data_count = 0;
SoftwareSerial bt(0, 1); // RX, TX pins for Bluetooth communication
Servo myservo; // Declare servo object

// Define pin connections for LCD, keypad, PIR sensor, and buzzer
const int pirPin = 30;
const int buzzerPin = 7; // Example pin for the buzzer
// Define LCD pins
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Define keypad pins and keys
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};
byte rowPins[ROWS] = {36, 37, 38, 39};
byte colPins[COLS] = {40, 41, 42};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
    lcd.begin(16, 2);
    lcd.print("Smart Home");
    Serial.begin(9600); // Initialize hardware serial
    bt.begin(9600);     // Initialize software serial for Bluetooth communication

    pinMode(pirPin, INPUT);
    pinMode(buzzerPin, OUTPUT);
    myservo.attach(24); // Attach servo to pin 24

    for (int i = 2; i < 54; i++)
        pinMode(i, OUTPUT);

    pinMode(30, INPUT);
    pinMode(A1, INPUT);

    for (int i = 2; i < 54; i++)
        digitalWrite(i, LOW);
}

void loop() {
    PASSWORD();
    choice();
    PIR();
    heat();
}

void PIR() {
    int pirState = digitalRead(pirPin);

    if (pirState == HIGH && i == 1) {
        digitalWrite(buzzerPin, LOW);
    } else if (pirState == HIGH && i == 0) {
        digitalWrite(buzzerPin, HIGH);

    } else if (pirState == HIGH) {
        digitalWrite(buzzerPin, HIGH);
    }
    delay(100);
}

void heat() {
    temp = analogRead(A1);
    temp = temp * 0.48875125;

    if (temp > 40) {
        text = "door1open";
    }
}

void choice() {
    if (bt.available()) {
        while (bt.available()) {
            char c = bt.read();
            Serial.write(c); // Echo received Bluetooth data to hardware serial
            if (Serial.available()) {
                while (Serial.available()) {
                    text = Serial.readString();
                }
                if (text == "led1") {
                    digitalWrite(2, HIGH);
                } else if (text == "led2") {
                    digitalWrite(3, HIGH);
                } else if (text == "led1off") {
                    digitalWrite(2, LOW);
                } else if (text == "led2off") {
                    digitalWrite(3, LOW);
                } else if (text == "temp") {
                    print_temp();
                    heat();
                } else if (text == "door1open") {
                    door_open();
                } else if (text == "door1close") {
                    door_close();
                }
            }
        }
    }
}

void print_temp() {
    temp = analogRead(A1);
    temp = temp * 0.48875125;
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("TEMP= ");
    lcd.print(temp);
    lcd.print("*C");
    delay(30);
}

void door_open() {
    myservo.write(180);
    delay(15);
    door_close();
}

void door_close() {
    myservo.write(0);
    delay(15);
    PASSWORD(); // Prompt for password again after closing the door
}

bool PASSWORD() {
    lcd.setCursor(0, 0);
    lcd.clear();
    delay(1000);
    lcd.print("Enter PASS: ");
    lcd.setCursor(1, 1);
    text = "";
    while (1) {
        char key = keypad.getKey();
        if (key) {
            Data += key;
            lcd.setCursor(data_count, 1);
            lcd.print(Data[data_count]);
            data_count++;
        }
        if (data_count == Password_Length) {
            lcd.clear();
            if (Data == pass) {
                lcd.print("correct");
                lcd.print("Welcome home");
                door_open();
                i = 1;
                return true;
            } else {
                lcd.print("wrongpass");
                i = 0;
                data_count = 0;
                Data = "";
                return false;
            }
        }
    }
}
