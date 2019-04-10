//Still working on this. contains lots of unused artifacts from code that I still have to put in here 
//or tried out and did not clean up yet. Code is working though, Pir active light bright, pir inactive light dims.

//Now to try and get my fire routine in here and a solid colour to blend to on PIR activation.


//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;      
 
//the time when the sensor outputs a low impulse
long unsigned int lowIn;        
 
//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 8000;
 
boolean lockLow = true;
boolean takeLowTime;
 
int pirPin = 16;    //the digital pin connected to the PIR sensor's output
// int ledPin = 13;

//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER // Only use when using nodeMCU pin numbers like D4,D5,D12,D15 etc...

#include <FastLED.h> // I am using the real GPIO numbers, not the D# nodeMCU numbers. If you want to use the NODEMCU specific numbers you need to define (#define FASTLED_ESP8266_NODEMCU_PIN_ORDER)
                     // Before including (#include <FastLED.h>)

#define LED_PIN_0     12 //288 Here I define the pins per ledstrip 
#define LED_PIN_1     13 //288 Not all pins are unused so be careful
#define LED_PIN_2     14 //288 to choose the ones that are availeable on the NODEMCU
#define LED_PIN_3     4  //30    
#define LED_PIN_4     5  //30    
#define CLOCK_PIN     15 //     Here I define the single clockpin So dont use a clockpin per ledstrip. Al the clock wires from all the strips use this single clockpin.
#define COLOR_ORDER BGR  //     Defining the colour order of the 3 colours the APA102 led has. Other choices can be BRG, RGB, RBG, GRB, GBR 
#define CHIPSET     APA102 // Defining the actual LED chip used that Fastled can recognise. 
#define NUM_LEDS_0    288 // Define nr of leds on the strip connected to pin 12 
#define NUM_LEDS_1    288 // Define nr of leds on the strip connected to pin 13
#define NUM_LEDS_2    288 // Define nr of leds on the strip connected to pin 14
#define NUM_LEDS_3    30  // Define nr of leds on the strip connected to pin 4
#define NUM_LEDS_4    30  // Define nr of leds on the strip connected to pin 5
#define NUM_STRIPS 5 // Total number of strips

#define MILLI_AMPS         30000 // IMPORTANT!!: set the max milli-Amps of your powersupply (30A = 30000mA) as my powersupply is rated up to 30A.
                                 // Fastled calculates power consumption by using known consumption specs per led and chipset, to lower brightness when 
                                 // you try and draw more power then your powersupply can handle. This to not damage your electronics.

#define BRIGHTNESS  25        // Overall Brightnes from 0 up to 255
#define FRAMES_PER_SECOND 120   // Speed of the frames. Higher number is more speed.
//#define DIMMING             8

bool gReverseDirection = false;

CRGB leds[NUM_LEDS_0 + NUM_LEDS_1 + NUM_LEDS_2 + NUM_LEDS_3 + NUM_LEDS_4];  // Add all the leds together here

uint8_t stripLength; // Used to store a strip length.
uint8_t gHue = 0; // rotating "base color" used by many of the Fastled patterns

CRGBPalette16 gPal;
 
 
/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  // pinMode(LED_PIN_0, LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4 OUTPUT);
  digitalWrite(pirPin, LOW);
 
  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(0);


delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN_0, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS_0).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN_1, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS_1).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN_2, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS_2).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN_3, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS_3).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_PIN_4, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS_4).setCorrection( TypicalLEDStrip );
  
  FastLED.setBrightness( BRIGHTNESS );

  gPal = CRGBPalette16( CRGB::Black, CRGB::OrangeRed, CRGB::Gold, CRGB::LightYellow);
    
    }
 
////////////////////////////
//LOOP
void loop()



{
 
     if(digitalRead(pirPin) == HIGH){
  //     digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
       if(lockLow){
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;
                  
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec");
         delay(0000);
         FastLED.show();
         FastLED.delay(1000 / FRAMES_PER_SECOND);
         
         }        
         takeLowTime = true;        
       }
 
     if(digitalRead(pirPin) == LOW){      
//       digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
 
       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause,
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){
           //makes sure this block of code is only executed again after
           //a new motion sequence has been detected
           lockLow = true;
                             
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           delay(0);
           fadeToBlackBy( leds, NUM_LEDS_0 + NUM_LEDS_1 + NUM_LEDS_2 + NUM_LEDS_3 + NUM_LEDS_4, 200);
           FastLED.show();
          
           
           }

           
  //fill_solid( leds, NUM_LEDS, CRGB::Linen );
  for(int i = 0; i<NUM_LEDS_0;i++){
   leds[i] = CRGB (250, 248, 80);
  }
       }
       
  } 
