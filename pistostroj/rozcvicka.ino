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

const int MINIMALNI_MOTOR = 30; // minimalni doba jizdy motoru po strihu. 
const int MINIMALNI_PROSTOJ = 500; //minimalni doba mezi jednotlivyma strihama
const int DOBA_SEPNUTI_STRIHACICH_PINU = 10; // doba sepnuti strihu. 
const int DOBA_STRIHU = 30; // doba jakou pist jede dolu - podle ni se spusti motor. 

//-------------------------------------------------
// Definice vstupnich pinu
//-------------------------------------------------


//const int LASERPRERUSENI = 0; // Laserova zavora preruseni 0 (pin 2)
const int PIST_1_STRED = 0; // pist 1 - cidlo ve stredu, napojene na preruseni 1 (pin 3)


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
//const int PIST_3 = 8; // spinac pistu 3

const int STRIH_1 = 7; // Prvni strihaci pin.
const int STRIH_2 = 8; // Druhy strihaci pin. 

//tohle mozna neni potreba.... zalezi esli muzu cekat v preruseni nebo ne...
volatile boolean jepistvestredu = false;
volatile boolean strihej = false; //pokud je tohle false, stroj dodela kolo a dalsi uz nepojede.
volatile boolean zrovna_striham = false;

boolean prvni_narade = true; 
long posledni_strih = 0; 


void setup() {
    Serial.begin(57600);

    pinMode(2, INPUT); // pin preruseni.
    pinMode(LASER, INPUT); 

    pinMode(PIST_1_NAHORE, INPUT);
    pinMode(PIST_1_DOLE, INPUT);
    pinMode(PIST_2_NAHORE, INPUT);
//    pinMode(PIST_3_NAHORE, INPUT);


    pinMode(MOTOR, OUTPUT);
    pinMode(PIST_1, OUTPUT);
    pinMode(PIST_2, OUTPUT);
//    pinMode(PIST_3, OUTPUT);
    pinMode(STRIH_1, OUTPUT);
    pinMode(STRIH_2, OUTPUT);
    
    attachInterrupt(PIST_1_STRED, pistvpozici, FALLING);
    //attachInterrupt(LASERPRERUSENI, vypnipisty_1_2, CHANGE);


}


void strihni() {


      //kontrola zdali od minuleho strihu uz uplynulo dost casu... 

      if ((millis() - posledni_strih) < MINIMALNI_PROSTOJ){
        if (KOMENTATOR) { Serial.println("Jeste neuplynula minimalni doba od minuleho strihu. Koncim."); }
        return ; 
      }

      zrovna_striham = true;

      // Toto provede sepnuti pinu na danou konstantu definovanou nahore. 
      
      if (prvni_narade) {
        digitalWrite(STRIH_1, HIGH);
        cekejtise(DOBA_SEPNUTI_STRIHACICH_PINU);
        digitalWrite(STRIH_1, LOW);
        if (KOMENTATOR) { Serial.println("Cvaknul jsem pinem 1 na danou dobu."); }
      }else {
        digitalWrite(STRIH_2, HIGH);
        cekejtise(DOBA_SEPNUTI_STRIHACICH_PINU);
        digitalWrite(STRIH_2, LOW);
        if (KOMENTATOR) { Serial.println("Cvaknul jsem pinem 2 na danou dobu."); } 
      }
      
      prvni_narade = !prvni_narade; //aby priste strihal jiny pist. 
      posledni_strih = millis(); //ulozi si kdy naposledy strihal. 

      //Cekani na dojeti strihaciho pistu dolu : 

      cekejtise(DOBA_STRIHU);

      //zapnuti motoru. 

      digitalWrite(MOTOR, HIGH);

      if (KOMENTATOR) { Serial.println("Strih dokoncen. zapl jsem motor."); }
      
      cekejtise(MINIMALNI_MOTOR);

      while ( (digitalRead(LASER) == HIGH) ){
        cekejtise(1);
      }

      digitalWrite(MOTOR, LOW);
      if (KOMENTATOR) { Serial.println("Laser je odpojen, vypl jsem motor, ted resetnu strihaci indikatory"); }
      
      zrovna_striham = false;
      strihej = false;
//      return pist1dojel;

}


boolean zkontrolujlaser() {
  if (digitalRead(LASER) == HIGH) { //pokud je tam matros, vrati true, jinak false. 
   return true;
  } else {
   return false;
  }
}


void cekejtise(int kolik){

     for (int i=0; i <= kolik; i++){
        if (digitalRead(PIST_1_DOLE) == HIGH) {
          if (KOMENTATOR) { Serial.println("Pist 1 dojel dolu, posilam nahoru"); }
          digitalWrite(PIST_1, LOW);
        }

        if (jepistvestredu) {
          if (KOMENTATOR) { Serial.println("Pist 1 dojel doprostred, posilam pist 2 zpatky"); }
          digitalWrite(PIST_2, LOW);
          jepistvestredu = false;
        }

        if (zkontrolujlaser() && zrovna_striham==false) {
          strihni();
        }
        delay(1);
     }
}


void loop() {
    Serial.println("Stav pinu");
    Serial.print("Stred : ");
    Serial.println(digitalRead(2));
    Serial.print("Laser : ");
    Serial.println(digitalRead(LASER));
    Serial.print("P1 nahore : ");
    Serial.println(digitalRead(PIST_1_NAHORE));
    Serial.print("P1 dole : ");
    Serial.println(digitalRead(PIST_1_DOLE));
    Serial.print("P2 na zacatku  : ");
    Serial.println(digitalRead(PIST_2_NAHORE));

    delay(5000);
    
    for (int i=0; i <= 100; i++){
        Serial.print(digitalRead(LASER));
        delay(1);
     }
    Serial.println(digitalRead(LASER));

   // digitalWrite(MOTOR,HIGH);

/*
    pinMode(MOTOR, OUTPUT);
    pinMode(PIST_1, OUTPUT);
    pinMode(PIST_2, OUTPUT);
    pinMode(STRIH_1, OUTPUT);
    pinMode(STRIH_2, OUTPUT);
    
*/
}


void pistvpozici() {

   jepistvestredu = true;


}


void vypnipisty_1_2() {
  if (!(zrovna_striham)) {
   if (digitalRead(2) == HIGH) {
     strihej = true;
   }  else {
     strihej = false;
   }
}
}
