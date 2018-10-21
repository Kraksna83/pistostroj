/*

Pistostroj ktery neco dela.

nejnovejsi na https://github.com/Kraksna83/pistostroj/

*/

//debug rezim ktery bude psat do seriaku co se deje :

const boolean KOMENTATOR = true;


//-------------------------------------------------
// Definice vyckavacich konstant
//-------------------------------------------------


const int CEKANI_PIST_1_2 = 500; //cekani po vypnuti pistu 1 a 2 v neautomatickem rezimu
const int CEKANI_PIST_2 = 500; //cekani pistu 2 v automatickem rezimu
const int CEKANI_PIST_3 = 500; // cekani pistu 3 v neautomatickem rezimu

//-------------------------------------------------
// Definice vstupnich pinu
//-------------------------------------------------


const int LASERPRERUSENI = 0; // Laserova zavora preruseni 0 (pin 2)
const int PIST_1_STRED = 1; // pist 1 - cidlo ve stredu, napojene na preruseni 1 (pin 3)



const int PIST_1_NAHORE = 6; // horni cidlo pistu 1
//pist 1 stred je na preruseni, definovano vyse.
const int PIST_1_DOLE = 7; // spodni cidlo pistu 1
const int PIST_2_NAHORE = 8; // horni cidlo pistu 2
const int PIST_3_NAHORE = 9; // horni cidlo pistu 3


//-------------------------------------------------
// Definice vystupnich pinu
//-------------------------------------------------


const int MOTOR = 10; // spinac motoru
const int PIST_1 = 11; // spinac pistu 1
const int PIST_2 = 12; // spinac pistu 2
const int PIST_3 = 13; // spinac pistu 3

//tohle mozna neni potreba.... zalezi esli muzu cekat v preruseni nebo ne...
volatile boolean jepistvestredu = false;
volatile boolean strihej = false; //pokud je tohle false, stroj dodela kolo a dalsi uz nepojede.


void setup() {
   Serial.begin(57600);

   pinMode(2, INPUT); // prerusovaci pin laser
   pinMode(3, INPUT); // prerusovaci pin pist 1 stred

   pinMode(PIST_1_NAHORE, INPUT);
   pinMode(PIST_1_DOLE, INPUT);
   pinMode(PIST_2_NAHORE, INPUT);
   pinMode(PIST_3_NAHORE, INPUT);


   pinMode(MOTOR, OUTPUT);
   pinMode(PIST_1, OUTPUT);
   pinMode(PIST_2, OUTPUT);
   pinMode(PIST_3, OUTPUT);

   attachInterrupt(PIST_1_STRED, pistvpozici, RISING);
   attachInterrupt(LASERPRERUSENI, vypnipisty_1_2, CHANGE);


}

void strihni() {
    //vypni pisty 1 a 2 
    if (KOMENTATOR) { Serial.println("Zacina strihani. posilam pisty 1 a 2 do vychozi polohy."); }
    digitalWrite(PIST_1, LOW);
    digitalWrite(PIST_2, LOW);

    //kontrola ze pist 3 je nahore.. 
    if (KOMENTATOR) { Serial.println("Ujistuju se ze mam pist 3 nahore"); }
     while (!( digitalRead(PIST_3_NAHORE) == HIGH )){
       delay(1);
     }

     digitalWrite(PIST_3, HIGH);
     if (KOMENTATOR) { Serial.println("Poslal jsem pist 3, ted du cekat konstantu "); }

     delay(CEKANI_PIST_3);



     digitalWrite(PIST_3, LOW);

     if (KOMENTATOR) { Serial.println("Konstanta docekana, p3 jede nahoru, ted cekam az tam dojede."); }

     while (!( digitalRead(PIST_3_NAHORE) == HIGH )){
       delay(1);
     }

     digitalWrite(MOTOR, HIGH);

     if (KOMENTATOR) { Serial.println("Pist dojel. zapl jsem motor."); }

     while ( strihej == false ){
       delay(1);
     }

     digitalWrite(MOTOR, LOW);
     if (KOMENTATOR) { Serial.println("Laser je spojen, vypl jsem motor."); }
     
 }


void loop() {
     
     // kontrola esli sou pisty ve vychozi pozici :
     
     if (KOMENTATOR) { Serial.println("Cekam na pist 3 az bude nahore"); }
     while (!(digitalRead(PIST_3_NAHORE) == HIGH)) {
       delay ( 1 );
     }
 
     if (KOMENTATOR) { Serial.println("Cekam na pist 2 az bude nahore"); }
     while (!(digitalRead(PIST_2_NAHORE) == HIGH)) {
         delay ( 1 );
     }
     if (KOMENTATOR) { Serial.println("Pist 2 je nahore"); }


     if (KOMENTATOR) { Serial.println("Cekam na pist 1 az bude nahore"); }
     while (!(digitalRead(PIST_1_NAHORE) == HIGH)) {
         delay ( 1 );
     }
     if (KOMENTATOR) { Serial.println("pist 1 je nahore"); }
   
    //zacina prace.. 
   
     if (KOMENTATOR) { Serial.println("Posilam pist 2"); }
     digitalWrite(PIST_2,HIGH);

     if (KOMENTATOR) { Serial.println("Cekam konstantu az dojede"); }
     //delay(CEKANI_PIST_2);
         
     for (int i=0; i <= CEKANI_PIST_2; i++){
       if (strihej) { strihni(); return 0; }  //pokud bylo preruseni laserem vseho nech a bez strihat. pak restartni loop()
       delay(1);
     }

     if (KOMENTATOR) { Serial.println("Posilam pist 1 a cekam na preruseni"); }
     digitalWrite(PIST_1,HIGH);



     while (!(jepistvestredu)) {
       delay(1); //milisekunda delay, ale asi se to muze aj oddelat...
       if (strihej) { strihni(); return 0; }  //pokud bylo preruseni laserem vseho nech a bez strihat. pak restartni loop()
     }

     digitalWrite(PIST_2,LOW);
     if (KOMENTATOR) { Serial.println("Preruseno jest, vypnul jsem pist 2, ted du cekat na pist 1 az dosahne konce."); }

     while (!( digitalRead(PIST_1_DOLE) == HIGH )){
       if (strihej) { strihni(); return 0; }  //pokud bylo preruseni laserem vseho nech a bez strihat. pak restartni loop()
       delay(1);
     }
     digitalWrite(PIST_1, LOW);
     if (KOMENTATOR) { Serial.println("OK, je tam - byl vypnut. ted budu cekat az bude pist 1 nahore. "); }


     while (!( digitalRead(PIST_1_NAHORE) == HIGH )){
       if (strihej) { strihni(); return 0; }  //pokud bylo preruseni laserem vseho nech a bez strihat. pak restartni loop()
       delay(1);
     }

     if (KOMENTATOR) { Serial.println("pist 1 nahore, cyklus konci. "); }

     //pist 1 mi zase projel stredem, coz by melo spustit preruseni. tady si to vynuluju aby to na zacatku cyklu nebylo blbe
     jepistvestredu = false;


}


void pistvpozici() {

  jepistvestredu = true;


}


void vypnipisty_1_2() {
  if (digitalRead(2) == LOW) {
    strihej = true;
  }  else {
    strihej = false;
  }
} 
