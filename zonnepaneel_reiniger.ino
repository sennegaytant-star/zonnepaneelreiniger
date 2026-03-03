//  MOTOR 1 - LINKS 
int R_EN1 = 2;
int L_EN1 = 4;
int R_PWM1 = 3;
int L_PWM1 = 5;

//  MOTOR 2 - RECHTS 
int R_EN2 = 6;  
int L_EN2 = 7;   
int R_PWM2 = 8;  
int L_PWM2 = 11; 

//  HC-SR04 SENSOR 
int TRIG_PIN = 9;
int ECHO_PIN = 10;

// waterpomp
int POMP_PIN = x;

const int draaiTijd90  = 600;   // (aanpassen na testen)
const int verschuifTijd = 500;  // (aanpassen na testen)

const float grensAfstand = 12.0;  // cm - aanpassen na meten!


// State 0: Rijden
// State 1: Eerste 90° draaien
// State 2: Stukje vooruitrijden (rij opschuiven)
// State 3: Tweede 90° draaien
int state = 0;
int rijRichting = 1;      // 1 = vooruit, -1 = achteruit
int draaiKant = 1;        // 1 = rechts draaien, -1 = links draaien
int verschuifFase = 0;    // bijhoudt welke fase van verschuiven we in zitten

unsigned long faseStart = 0;

//DECLARATIES
float leesAfstand();
void vooruitRijden();
void achteruitRijden();
void rechtsDraaien();
void linksDraaien();
void Stoppen();

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(R_EN1, OUTPUT); 
  pinMode(L_EN1, OUTPUT);
  pinMode(R_PWM1, OUTPUT); 
  pinMode(L_PWM1, OUTPUT);
  pinMode(R_EN2, OUTPUT); 
  pinMode(L_EN2, OUTPUT);
  pinMode(R_PWM2, OUTPUT); 
  pinMode(L_PWM2, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(POMP_PIN, OUTPUT);

  digitalWrite(R_EN1, HIGH); 
  digitalWrite(L_EN1, HIGH);
  digitalWrite(R_EN2, HIGH); 
  digitalWrite(L_EN2, HIGH);

}


void loop() {
  float afstand_cm = leesAfstand();

  Serial.print("Afstand: "); 
  Serial.print(afstand_cm);
  Serial.print(" cm | State: "); 
  Serial.println(state);

  switch (state) {

    // RIJDEN 
    case 0:
      if (rijRichting == 1) {
        vooruitRijden();
      } else {
        achteruitRijden();
      }

      // Rand gedetecteerd → start met draaien
      if (afstand_cm > grensAfstand && afstand_cm != 0.0 && afstand_cm < 80.0) {
        Stoppen();
        delay(300);
        faseStart = millis();
        state = 1;  // Eerste 90° draaien
      }
      break;

    // EERSTE 90° DRAAIEN 
    case 1:
      if (draaiKant == 1) {
        rechtsDraaien();
      } else {
        linksDraaien();
      }

      if (millis() - faseStart > draaiTijd90) {
        Stoppen();
        delay(200);
        faseStart = millis();
        state = 2;  // Stukje vooruitrijden
      }
      break;

    // STUKJE VOORUIT 
    case 2:
      vooruitRijden();  // altijd vooruit voor het opschuiven

      if (millis() - faseStart > verschuifTijd) {
        Stoppen();
        delay(200);
        faseStart = millis();
        state = 3;  // Tweede 90° draaien
      }
      break;

    // TWEEDE 90° DRAAIEN 
    case 3:
      if (draaiKant == 1) {
        rechtsDraaien();
      } else {
        linksDraaien();
      }

      if (millis() - faseStart > draaiTijd90) {
        Stoppen();
        delay(200);

        rijRichting = -rijRichting;   // Rijrichting omkeren
        draaiKant   = -draaiKant;     // Volgende keer andere kant draaien
        state = 0;                    // Terug naar rijden
      }
      break;
  }
}


float leesAfstand() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration == 0) {
    return 99.0;  // Geen echo = rand
  }
  return (duration * 0.034) / 2.0;
}


void vooruitRijden() {
  analogWrite(R_PWM1, 180); 
  analogWrite(L_PWM1, 0);

  analogWrite(R_PWM2, 180);
  analogWrite(L_PWM2, 0);
}

void achteruitRijden() {
  analogWrite(R_PWM1, 0); 
  analogWrite(L_PWM1, 180);

  analogWrite(R_PWM2, 0); 
  analogWrite(L_PWM2, 180);
}

void linksDraaien() {
  analogWrite(R_PWM1, 0);   
  analogWrite(L_PWM1, 150);

  analogWrite(R_PWM2, 150); 
  analogWrite(L_PWM2, 0);
}

void rechtsDraaien() {
  analogWrite(R_PWM1, 150); 
  analogWrite(L_PWM1, 0);

  analogWrite(R_PWM2, 0);   
  analogWrite(L_PWM2, 150);
}

void Stoppen() {
  analogWrite(R_PWM1, 0); 
  analogWrite(L_PWM1, 0);
  
  analogWrite(R_PWM2, 0); 
  analogWrite(L_PWM2, 0);
}
