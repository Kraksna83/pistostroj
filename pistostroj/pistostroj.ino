/*
Pistostroj ktery neco dela.
nejnovejsi na https://github.com/Kraksna83/pistostroj/
*/

//debug rezim ktery bude psat do seriaku co se deje :
const boolean KOMENTATOR = false ;

//-------------------------------------------------
// Definice vyckavacich konstant
//-------------------------------------------------

const int CEKANI_PIST_1_2 = 500; //cekani po vypnuti pistu 1 a 2 v neautomatickem rezimu
const int CEKANI_PIST_2 = 500; //cekani pistu 2 v automatickem rezimu
const int CEKANI_PIST_3 = 500; // cekani pistu 3 v neautomatickem rezimu

const int MINIMALNI_MOTOR = 50; // minimalni doba jizdy motoru po strihu.
const int MINIMALNI_PROSTOJ = 200; //minimalni doba mezi jednotlivyma strihama
const int DOBA_SEPNUTI_STRIHACICH_PINU = 10; // doba sepnuti strihu.
const int DOBA_STRIHU = 50; // doba jakou pist jede dolu - podle ni se spusti motor.

//-------------------------------------------------
// Definice vstupnich pinu
//-------------------------------------------------

const int PIST_1_STRED = 0; // pist 1 - cidlo ve stredu, napojene na preruseni 1 (pin 2)
const int LASER = 3;
const int PIST_1_NAHORE = 10; // horni cidlo pistu 1 //zmeneno z 6 - PREDPOKLADAM ZE "NAHORE" ZNAMENA "ZACATEK", pokud ne, je potreba prehodit.
//pist 1 stred je na preruseni, definovano vyse.
const int PIST_1_DOLE = 9; // spodni cidlo pistu 1 //zmeneno z 7 - PREDPOKLADAM ZE "DOLE" ZNAMENA "KONEC"
const int PIST_2_NAHORE = 8; // horni cidlo pistu 2
//const int PIST_3_NAHORE = 11; // horni cidlo pistu 3

//-------------------------------------------------
// Definice vystupnich pinu
//-------------------------------------------------
const int MOTOR = 5; // spinac motoru
const int PIST_1 = 13; // spinac pistu 1
const int PIST_2 = 4; // spinac pistu 2
const int STRIH_1 = 6; // Prvni strihaci pin.
const int STRIH_2 = 7; // Druhy strihaci pin.

//tohle mozna neni potreba.... zalezi esli muzu cekat v preruseni nebo ne...
volatile boolean jepistvestredu = false;
boolean prvni_narade = true;
long posledni_strih = 0;

void setup() {
    Serial.begin(57600);

    pinMode(2, INPUT); // pin preruseni.
    pinMode(LASER, INPUT);

    pinMode(PIST_1_NAHORE, INPUT);
    pinMode(PIST_1_DOLE, INPUT);
    pinMode(PIST_2_NAHORE, INPUT);

    pinMode(MOTOR, OUTPUT);
    pinMode(PIST_1, OUTPUT);
    pinMode(PIST_2, OUTPUT);
    pinMode(STRIH_1, OUTPUT);
    pinMode(STRIH_2, OUTPUT);
   
    attachInterrupt(PIST_1_STRED, pistvpozici, FALLING);
}


void strihni() {
      //kontrola zdali od minuleho strihu uz uplynulo dost casu...
      if ((millis() - posledni_strih) < MINIMALNI_PROSTOJ){
        if (KOMENTATOR) { Serial.print("X"); }
        return ;
      }
     
      if (prvni_narade) {
        digitalWrite(STRIH_1, HIGH);
        cekejtisevestrihu(DOBA_SEPNUTI_STRIHACICH_PINU);
        digitalWrite(STRIH_1, LOW);
        if (KOMENTATOR) { Serial.println("Cvaknul jsem pinem 1 na danou dobu."); }
      }else {
        digitalWrite(STRIH_2, HIGH);
        cekejtisevestrihu(DOBA_SEPNUTI_STRIHACICH_PINU);
        digitalWrite(STRIH_2, LOW);
        if (KOMENTATOR) { Serial.println("Cvaknul jsem pinem 2 na danou dobu."); }
      }
     
      prvni_narade = !prvni_narade; //aby priste strihal jiny pist.
      posledni_strih = millis(); //ulozi si kdy naposledy strihal.

      //Cekani na dojeti strihaciho pistu dolu :

      cekejtisevestrihu(DOBA_STRIHU);

      //zapnuti motoru.
      digitalWrite(MOTOR, HIGH);
      if (KOMENTATOR) { Serial.println("Strih dokoncen. zapl jsem motor."); }     

      cekejtisevestrihu(MINIMALNI_MOTOR);  // jed minimalne tolik
      while ( (digitalRead(LASER) == LOW) ){ //cekej dokud neodjede tycka.
        cekejtisevestrihu(1);
      }
      cekejtisevestrihu(MINIMALNI_MOTOR); // jeste chvilku jed at tycka odjede z obou laseru

      digitalWrite(MOTOR, LOW);
      if (KOMENTATOR) { Serial.println("Laser je odpojen, vypl jsem motor, ted resetnu strihaci indikatory"); }
}


boolean zkontrolujlaser() {
  if (digitalRead(LASER) == LOW) { //pokud je tam matros, vrati true, jinak false.  //prehozeno
   return true;
  } else {
   return false;
  }
}

void cekejtisevestrihu(int kolik){
     for (int i=0; i <= kolik; i++){
        if (jepistvestredu) {
          if (KOMENTATOR) { Serial.println("STRIH! Pist 1 dojel doprostred, posilam pist 2 zpatky"); }
          digitalWrite(PIST_2, LOW);
          jepistvestredu = false;
        }

        delay(1);
     }
}

void cekejtise(int kolik){
     for (int i=0; i <= kolik; i++){
        if (zkontrolujlaser()) {
            strihni();
        }

        if (jepistvestredu) {
          if (KOMENTATOR) { Serial.println("Pist 1 dojel doprostred, posilam pist 2 zpatky"); }
          digitalWrite(PIST_2, LOW);
          jepistvestredu = false;
        }

        delay(1);
     }
}


void loop() {
      //pist 1 mi zase projel stredem, coz by melo spustit preruseni. tady si to vynuluju aby to na zacatku cyklu nebylo blbe
      jepistvestredu = false;

      if (KOMENTATOR) { Serial.println("Cekam na pist 1 az bude nahore"); }
      while (!(digitalRead(PIST_1_NAHORE) == LOW)) { //prehozeno??
          cekejtise ( 1 );
      }
      if (KOMENTATOR) { Serial.println("pist 1 je nahore"); }
      jepistvestredu = false;

      if (KOMENTATOR) { Serial.println("Posilam pist 2"); }
      digitalWrite(PIST_2,HIGH);
      if (KOMENTATOR) { Serial.println("Cekam konstantu az se rozjede"); }
      cekejtise(CEKANI_PIST_2);

      if (KOMENTATOR) { Serial.println("Posilam pist 1 a cekam az dojede."); }
      digitalWrite(PIST_1,HIGH);
      while (!( digitalRead(PIST_1_DOLE) == LOW )){ //prehozeno
        cekejtise(1);
      }
     
      digitalWrite(PIST_1, LOW);
      if (KOMENTATOR) { Serial.println("OK, je tam - byl vypnut. ted budu cekat az bude pist 1 nahore. "); }
}

void pistvpozici() {
   jepistvestredu = true;
}
