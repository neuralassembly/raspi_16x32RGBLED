#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2
// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);

int counter = 0;
int color[3];

void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.fillScreen(0);
}

void loop() {

  if (Serial.available()) {
    char c = Serial.read();

    if(c>='0' && c<='9' || c>='a' && c<='f' || c>='A' && c<='F'){ // Only [0, F] are accepted. 

      if(c>='0' && c<='9'){
        c = c - '0';
      }
      else if(c>='a' && c<='f'){
        c = c - 'a' + 10;
      }
      else{
        c = c - 'A' + 10;
      }

      color[counter%3] = c;

      if(counter%3 == 2){
        matrix.drawPixel((counter/3)%matrix.width(), (counter/3)/matrix.width(), matrix.Color444(color[0], color[1], color[2]));
      }
      counter++;
    }
    else if(c== '.'){
      matrix.swapBuffers(false);
      counter = 0;
    }

  }
}
