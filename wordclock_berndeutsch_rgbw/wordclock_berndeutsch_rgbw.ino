#include <DS3231_Simple.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN           6
#define NUMPIXELS   114

#define BUTTONMODE 3
#define BUTTONHOURS 4
#define BUTTONMINUTES  5

DS3231_Simple Clock;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

// Time variables
int h;
int m;
int s;

// Buttonstates
int hourpressed = 0;
int minutespressed = 0;
int modepressed = 0;

// Configs
boolean purist = true; // puristmode turns on "it is" only on full and half hours
boolean dim = true;
int dimintensity = 10; // percent
int brightness = 255;
boolean turnoffminuteswhiledimmed = true;
boolean turnoffitiswhiledimmed = true;
int hourfrom = 23;
int minutefrom = 0;
int hourto = 6;
int minuteto = 0;
int mode = 0;
int customColor[4] = {0,0,0,0};

// Other
boolean debug = false;
boolean bootedup = false;

//Declare integer array with size corresponding to number of Neopixels
int individualPixels[NUMPIXELS]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Define the color modes
int modes[6][4] = { { 0,0,0,255 },          // Warm White (default)
                    { 255,0,0,0 },          // Red
                    { 0,255,0,0 },          // Green
                    { 0,0,255,0 },          // Blue
                    { 248,24,148,0 },       // Pink
                    { 255,255,255,255 } };  // Cold White
void setup()
{
  Serial.begin(9600);
  pixels.begin();
  Clock.begin();

  pinMode(BUTTONHOURS, INPUT);
  pinMode(BUTTONMINUTES, INPUT);
  pinMode(BUTTONMODE, INPUT);
}

void bootup(){
  if (!bootedup){
    pixels.setBrightness(brightness);
    for (int i=0; i<NUMPIXELS; i++){
      pixels.setPixelColor(i,pixels.Color(0,0,0,255));
    }
    pixels.show();

    delay(2000);
    bootedup = true;
  }
}

void loop()
{
  // Run bootupsequence (selftest leds)
  bootup();

  // Reset all the pixels
  resetPixels();
  
  // Create a DateTime object to hold the current Time
  DateTime CurrentTime;
  
  // Ask read DateTime from clock
  CurrentTime = Clock.read();

  // Get current time
  h=CurrentTime.Hour;
  m=CurrentTime.Minute;
  s=CurrentTime.Second;

  // Print out current time if debug is turned on
  if (debug){
    Serial.print(h);
    Serial.print(":");
    Serial.print(m);
    Serial.print(":");
    Serial.println(s);
  }
  
  // Configure time and mode via buttons
  hourpressed = digitalRead(BUTTONHOURS);
  minutespressed = digitalRead(BUTTONMINUTES);
  modepressed = digitalRead(BUTTONMODE);

  if (hourpressed == HIGH){
    if (h==23){
      h = -1;
    }
    DateTime newTime;
    newTime.Day    = 1;
    newTime.Month  = 1;
    newTime.Year   = 19; 
    newTime.Hour   = h+1;
    newTime.Minute = m;
    newTime.Second = 00;
    Clock.write(newTime);
    delay(500);
  }

  if (minutespressed == HIGH){
    if (m==59){
      m = -1;
    }
    DateTime newTime;
    newTime.Day    = 1;
    newTime.Month  = 1;
    newTime.Year   = 19; 
    newTime.Hour   = h;
    newTime.Minute = m+1;
    newTime.Second = 00;
    Clock.write(newTime);
    delay(200);
  }

  if (modepressed == HIGH){
    if (mode == 5){
      mode = 0;
    } else {
      mode++;
    }
    delay(300);
  }

  // Parse time values to light corresponding pixels
  if (purist){
    if((m>=0 && m<5) || (m>=30 && m<35)){
      showItIs(1);
    } else {
      showItIs(0);
    }
  } else {
    showItIs(1);
  }

  // turn off if dim and turnedoffwhiledimmed
  if (dim && turnoffitiswhiledimmed && timeInRange()){
    showItIs(0);
  }
  
  // Minutes exactly 0 - "GENAU"|"JUSTE"
  if ((m==0)){
    individualPixels[28]=1; // G
    individualPixels[29]=1; // E
    individualPixels[30]=1; // N
    individualPixels[31]=1; // A
    individualPixels[32]=1; // U
  }
  
  // Minutes between 5-10 or 55-60 or 25-30 or 35-40 - "FÜF"
  if ((m>=5 && m<10) || (m>=55 && m<60) || (m>=25 && m<30) || (m>=35 && m<40)){
    individualPixels[8]=1; // F
    individualPixels[9]=1; // Ü
    individualPixels[10]=1; // F
  }
  
  // Minutes between 10-15 or 50-55 - "ZÄÄ"
  if ((m>=10 && m<15) || (m>=50 && m<55)){
    individualPixels[13]=1; // Z
    individualPixels[12]=1; // Ä
    individualPixels[11]=1; // Ä
  }
  
  // Minutes between 15-20 or 45-50 - "VIERTU"
  if ((m>=15 && m<20) || (m>=45 && m<50)){
    individualPixels[21]=1; // V
    individualPixels[20]=1; // I
    individualPixels[19]=1; // E
    individualPixels[18]=1; // R
    individualPixels[17]=1; // T
    individualPixels[16]=1; // U
  }
  
  // Minutes between 20-25 or 40-45 - "ZWÄNZG"
  if ((m>=20 && m<25) || (m>=40 && m<45)){
    individualPixels[22]=1; // Z
    individualPixels[23]=1; // W
    individualPixels[24]=1; // Ä
    individualPixels[25]=1; // N
    individualPixels[26]=1; // Z
    individualPixels[27]=1; // G
  }  

  // Minutes between 25-30 or 55-60 - "VOR"
  if ((m>=25 && m<30) || (m>=55 && m<60) || (m>=40 && m<55)){
    individualPixels[43]=1; // V
    individualPixels[42]=1; // O
    individualPixels[41]=1; // R
  }
  
  // Minutes between 5-10 or 35-40 - "AB"
  if ((m>=5 && m<25) ||(m>=35 && m<40)){
    individualPixels[40]=1; // A
    individualPixels[39]=1; // B
  }

  // Minutes between 25-35 - "HAUBI"
  if ((m>=25 && m<40)){
    individualPixels[37]=1; // H
    individualPixels[36]=1; // A
    individualPixels[35]=1; // U
    individualPixels[34]=1; // B
    individualPixels[33]=1; // I
  }

  // Add one hour when at least half hour has passed
  if (m>=25){
    h=h+1;
  }
 
  // Hour=1 - "EIS"
  if (h==1 || h==13){
    individualPixels[44]=1; // E
    individualPixels[45]=1; // I
    individualPixels[46]=1; // S
  }
  
  // Hour=2 - "ZWÖI"
  if (h==2 || h==14){
    individualPixels[47]=1; // Z
    individualPixels[48]=1; // W
    individualPixels[49]=1; // Ö
    individualPixels[50]=1; // I
  }
  
  // Hour=3 - "DRÜ"
  if (h==3 || h==15){
    individualPixels[52]=1; // D
    individualPixels[53]=1; // R
    individualPixels[54]=1; // Ü
  }
  
  // Hour=4 - "VIERI"
  if (h==4 || h==16){
    individualPixels[65]=1; // V
    individualPixels[64]=1; // I
    individualPixels[63]=1; // E
    individualPixels[62]=1; // R
    individualPixels[61]=1; // I
  }
  
  // Hour=5 - "FÜFI"
  if (h==5 || h==17){
    individualPixels[60]=1; // F
    individualPixels[59]=1; // Ü
    individualPixels[58]=1; // F
    individualPixels[57]=1; // I
  }
  
  // Hour=6 - "SÄCHSI"
  if (h==6 || h==18){
    individualPixels[66]=1; // S
    individualPixels[67]=1; // Ä
    individualPixels[68]=1; // C
    individualPixels[69]=1; // H
    individualPixels[70]=1; // S
    individualPixels[71]=1; // I
  }
  
  // Hour=7 - "SIBNI"
  if (h==7 || h==19){
    individualPixels[72]=1; // S
    individualPixels[73]=1; // I
    individualPixels[74]=1; // B
    individualPixels[75]=1; // N
    individualPixels[76]=1; // I
  }
  
  // Hour=8 - "ACHTI"
  if (h==8 || h==20){
    individualPixels[87]=1; // A
    individualPixels[86]=1; // C
    individualPixels[85]=1; // H
    individualPixels[84]=1; // T
    individualPixels[83]=1; // I
  }
  
  // Hour=9 - "NÜNI"
  if (h==9 || h==21){
    individualPixels[82]=1; // N
    individualPixels[81]=1; // Ü
    individualPixels[80]=1; // N
    individualPixels[79]=1; // I
  }
  
  // Hour=10 - "ZÄNI"
  if (h==10 || h==22){
    individualPixels[88]=1; // Z
    individualPixels[89]=1; // Ä
    individualPixels[90]=1; // N
    individualPixels[91]=1; // I
  }
  
  // Hour=11 - "EUFI"
  if (h==11 || h==23){
    individualPixels[95]=1; // E
    individualPixels[96]=1; // U
    individualPixels[97]=1; // F
    individualPixels[98]=1; // I
  }
  
  // Hour=12 - "ZWÖUFI"
  if (h==12 || h==0 || h==24){
    individualPixels[109]=1; // Z
    individualPixels[108]=1; // W
    individualPixels[107]=1; // Ö
    individualPixels[106]=1; // U
    individualPixels[105]=1; // F
    individualPixels[104]=1; // I
  }

  // Minutes - Turn off minutes according config while dimmed
  turnOnMinutes(m);
  if (dim && turnoffminuteswhiledimmed && timeInRange()){
    turnOffMinutes();
  }

  // Light pixels corresponding to current time
  pixels.setBrightness(255); // full brightness

  // Dimm down brightness during night time */
  if (dim && timeInRange()){
    // Calculate brightness
    brightness = (int)(255/dimintensity);
    pixels.setBrightness(brightness);
  }

  for (volatile int i=0; i<NUMPIXELS; i++){
    if (individualPixels[i]==1){
      pixels.setPixelColor(i, pixels.Color(modes[mode][0],modes[mode][1],modes[mode][2],modes[mode][3]));
    }
    else{
      pixels.setPixelColor(i,pixels.Color(0,0,0,0));
    }
  }
    
  pixels.show();
}

void resetPixels(){
  for (int i=0; i<NUMPIXELS; i++){
    individualPixels[i]=0;
  }
}

void showItIs(int state) {
  // ES|AS
  individualPixels[0]=state; // E
  individualPixels[1]=state; // S
      
  // ISCH
  individualPixels[3]=state; // I
  individualPixels[4]=state; // S
  individualPixels[5]=state; // C
  individualPixels[6]=state; // H
}

void turnOnMinutes(int m) {
  if (m%5>=1){
    individualPixels[112]=1; // 1
  }
  if (m%5>=2){
    individualPixels[111]=1; // 2
  }
  if (m%5>=3){
    individualPixels[110]=1; // 3
  }
  if (m%5==4){
    individualPixels[113]=1; // 4
  }
}

void turnOffMinutes() {
  individualPixels[112]=0; // 1
  individualPixels[111]=0; // 2
  individualPixels[110]=0; // 3
  individualPixels[113]=0; // 4
}

int getTimestamp(int h, int m){
  return (h * 100) + m;
}

boolean timeInRange(){
  // Time crosses midnight
  if (getTimestamp(hourfrom, minutefrom) > getTimestamp(hourto, minuteto)){
    if ( getTimestamp(h, m) < getTimestamp(hourfrom, minutefrom) && getTimestamp(h, m) > getTimestamp(hourto,minuteto)){
      return true;
    }
  } else {
    // Time doesnt cross midnight
    if ( getTimestamp(h, m) > getTimestamp(hourfrom, minutefrom) && getTimestamp(h, m) < getTimestamp(hourto, minuteto)){
      return true;
    }
  }
  return false;
}
