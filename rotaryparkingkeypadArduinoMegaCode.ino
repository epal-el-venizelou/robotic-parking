#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Stepper.h>
Servo myServo;
long barOpenMillis=0;
bool barClosedAgainAfterEntry;
LiquidCrystal_I2C lcd(0x3f, 16, 2);  // Διεύθυνση I2C 0x3f, LCD 16x2

// Βήματα ανά πλήρη περιστροφή (ανάλογα το μοτέρ)
const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 48, 50, 49, 51);
const int hallSensorPin = 39; // Πινάκι όπου είναι συνδεδεμένος ο αισθητήρας hall


const int MAX_SPOTS = 5;  // Μέγιστος αριθμός θέσεων
int availableSpots= MAX_SPOTS;

// Ορίζουμε το Keypad
const byte ROW_NUM    = 4; // Ο αριθμός των γραμμών
const byte COL_NUM    = 4; // Ο αριθμός των στηλών
char keys[4][4] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'}
};
// Ορίζουμε τα pins των γραμμών και των στηλών
byte pin_rows[ROW_NUM] = {5, 6, 7, 8}; 
byte pin_cols[COL_NUM] = {9, 10, 11, 12};

// Δημιουργούμε το αντικείμενο Keypad
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_cols, ROW_NUM, COL_NUM);

// Ο πίνακας για την αποθήκευση των πινακίδων (0 = όχι αυτοκίνητο, 1-9 = πινακίδα αυτοκινήτου)
int cars[5] = {0, 0, 0, 0, 0};  // Αρχικές πινακίδες: 123, 456, 789 (και 0 για κενές θέσεις)
unsigned long parkedTime[5] = {0, 0, 0, 0, 0};  // Χρόνος στάθμευσης για κάθε αυτοκίνητο
int currentPosition=0;

void setup() {
  Serial.begin(9600);  // Ξεκινάμε την επικοινωνία με τον υπολογιστή
   Serial1.begin(9600);   // Επικοινωνία με ESP8266 μέσω Serial1 (Pin 18 και 19)
  Serial.println("Πίνακας Αυτοκινήτων με Keypad:");
  printCars();  // Εμφανίζουμε την αρχική κατάσταση των αυτοκινήτων
   myServo.attach(2);
   lcd.init();         // Αρχικοποίηση LCD
    lcd.backlight();    // Ενεργοποίηση οπίσθιου φωτισμού
     
    updateLCD();        // Ενημέρωση LCD με τις θέσεις
  myServo.write(0);

  myStepper.setSpeed(15); // Ταχύτητα περιστροφής (RPM)
  pinMode(hallSensorPin, INPUT);
}

void loop() {
  char key = keypad.getKey();  // Διαβάζουμε το κουμπί που πατήθηκε
  if (millis()>barOpenMillis+5000 && barClosedAgainAfterEntry==false){
    myServo.write(0);
    updateLCD();
    barClosedAgainAfterEntry=true;
  }
  
  if (key) {
    // Αν πατηθεί το "A", τότε ξεκινάει η διαδικασία εισαγωγής αυτοκινήτου
    if (key == 'A') {
      
      if (availableSpots >0)
      {
        
        availableSpots--;  // Decrease the variable
        for (int i = 0; i < 5; i++) {
            if (cars[i] == 0) {  // Βρίσκουμε μια κενή θέση
              cars[i] = 1;  // Βάζουμε την πινακίδα εκεί
              parkedTime[i] = millis();  // Καταγράφουμε τον χρόνο στάθμευσης
              Serial.print("Το αυτοκίνητο τοποθετήθηκε στη θέση  ");
              Serial.println(i);
              moveTo(i);
              updateClientLCD(i);
              break;
            }
          }
          myServo.write(90);// Εδώ θα πρέπει να σηκώνεται η μπάρα και στην οθόνη να φαίνεται για λίγο η θέση που μπήκε
        barOpenMillis=millis();
        barClosedAgainAfterEntry=false;

    

          
        updateESP();
        Serial.print("Variable: ");
        Serial.println(availableSpots);
      }
      
      
    }
    //όταν αναχωρεί ένα αυτοκίνητο θα πατηθεί το B
    if (key == 'B') {
      if (availableSpots <MAX_SPOTS)
      {
        availableSpots++;  // Αυξησε the variable

      }
      int plate=getPlateInput();
      if (plate >= 0 && plate <= 4) {  // Αν η πινακίδα είναι έγκυρη

        moveTo(plate); //γυρίζουμε το σύστημα για να έρθει το όχημα
        unsigned long elapsedTime = millis() - parkedTime[plate];  // Υπολογίζουμε τον χρόνο που πέρασε

            
            // Μετατροπή του χρόνου σε ώρες και λεπτά
            unsigned long minutes = elapsedTime / 60000;  // Υπολογίζουμε τα λεπτά
            unsigned long hours = minutes / 60;  // Υπολογίζουμε τις ώρες
            minutes = minutes % 60;  // Υπολογίζουμε τα υπόλοιπα λεπτά
            
            // Εμφανίζουμε την ώρα στάθμευσης
            
            Serial.print("Το αυτοκίνητο αναχωρεί μετά από ");
            Serial.print(hours);
            Serial.print(" ώρες και ");
            Serial.print(minutes);
            Serial.println(" λεπτά.");
            updateCostLCD(hours,minutes);
            // Απενεργοποιούμε το αυτοκίνητο
            cars[plate] = 0;
            parkedTime[plate] = 0;  // Επαναφέρουμε το χρόνο στάθμευσης

            myServo.write(90);// Εδώ θα πρέπει να σηκώνεται η μπάρα και στην οθόνη να φαίνεται για λίγο η θέση που μπήκε
        barOpenMillis=millis();
        barClosedAgainAfterEntry=false;
    //updateLCD(); //αυτό γίνεται όταν κλείσει η μπάρα
    updateESP();
        // Ψάχνουμε αν η πινακίδα υπάρχει ήδη στον πίνακα
        
          
            // Αν βρούμε την πινακίδα, απενεργοποιούμε το αυτοκίνητο
            
            
          
        
        
        // Αν δεν βρέθηκε η πινακίδα, ενεργοποιούμε το αυτοκίνητο
        
        
        printCars();  // Εμφανίζουμε την ενημερωμένη κατάσταση των αυτοκινήτων
      } else {
        Serial.println("Μη έγκυρη πινακίδα.");
      }
      
    Serial.print("Variable: ");
    Serial.println(availableSpots);

    }
  }
}
void updateLCD() {
    lcd.clear();               // Καθαρισμός της οθόνης
    lcd.setCursor(0, 0);       // Θέση του κέρσορα στην πρώτη γραμμή
    if (availableSpots == 0) {
        lcd.print("FULL - No Parking");
    } else {
        lcd.print("Available: ");
        lcd.print(availableSpots);  // Εμφάνιση των διαθέσιμων θέσεων
    }
}

void printCars() {
  // Εμφανίζουμε την κατάσταση των αυτοκινήτων στον Serial Monitor
  for (int i = 0; i < 5; i++) {
    if (cars[i] == 0) {
      Serial.print("Θέση ");
      Serial.print(i);
      Serial.println(": Δεν υπάρχει αυτοκίνητο.");
    } else {
      Serial.print("Θέση ");
      Serial.print(i );
      Serial.print(": Αριθμός ");
      Serial.print(cars[i]);
      Serial.println(" - Ενεργό.");
    }
  }
  Serial.println();  // Κενή γραμμή για να ξεχωρίζουμε τις καταστάσεις
}

int getPlateInput() {
  String plateStr = "";  // Αρχικοποιούμε την πινακίδα
  char key;
  Serial.println("Εισάγετε την πινακίδα (ψηφίο 0-4):");

  // Διαβάζουμε ένα ψηφίο από το Keypad
  while (plateStr.length() < 1) {
    key = keypad.getKey();
    if (key) {
      Serial.print(key);  // Εμφανίζουμε το ψηφίο στον Serial Monitor
      plateStr += key;  // Προσθέτουμε το ψηφίο στην πινακίδα
    }
  }

  Serial.println();  // Ενότητα γραμμής στο τέλος
  return plateStr.toInt();  // Επιστρέφουμε την πινακίδα ως ακέραιο
}
void updateESP(){
  
  Serial1.println(availableSpots);  // Στέλνει τον ακέραιο αριθμό στο ESP8266
  Serial.println("Sent to ESP: " + String(availableSpots));  // Εμφανίζει τον αριθμό στο Serial Monitor του Arduino Mega
 
}

void updateClientLCD(int position){
   lcd.clear();               // Καθαρισμός της οθόνης
    lcd.setCursor(0, 0);       // Θέση του κέρσορα στην πρώτη γραμμή
    
        lcd.print("Your number is:"+String(position));
    
}
void updateCostLCD(int hours,int minutes){
  lcd.clear();               // Καθαρισμός της οθόνης
    lcd.setCursor(0, 0);       // Θέση του κέρσορα στην πρώτη γραμμή
    
        lcd.print("Time spent:");
        lcd.setCursor(0,1);
        lcd.print(String(hours) + " hours " + String(minutes) + " minutes");
}

void moveTo(int position)
{
  Serial.println("current position" + String(currentPosition));
  int placesToMove=0;
  if(currentPosition<=position)
  {
    placesToMove=position-currentPosition;
  }
  else
  {
    placesToMove=4-currentPosition+position+1;
  }
  Serial.println("moving positions:" + String(placesToMove));
  currentPosition=position;
  myStepper.step(placesToMove * 6625/5);
}
