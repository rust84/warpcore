// Listing of functions and arguments
// void active(int pixel)                     //sets pixel active for array operations
// void inactive(int pixel)                   //sets pixel inactive for array operations
// void chase(int r, int g, int b, int stayon, int startpixel, int endpixel, int delaytime)                                     //stayon: 1=YES, !1=NO
// void chasecircle(int r, int g, int b, int direct, int stayon, int startpixel, int delaytime)                                 //Direct: CW=1, CCW = !1, stayon: 1=YES, !1=NO
// void flash(int r, int g, int b, int numflashes, int delaytime)                                                               //turns all pixels on
// void flashpixel(int r, int g, int b, int pixel, int numflashes, int delaytime)                                               //turns and individual pixel on
// void lightning(int r, int g, int b, int dir, int delaytime)                                                                  //chases, then flashes
// void loadpixarray()                                                                                                          //loads pixarray[][] from current values
// void pingpong(int r1, int g1, int b1, int r2, int g2, int b2, int direct, int stayon, int delaytime)                         //Direct: CW=1, CCW = !1
// void pingpongcircle(int r1, int g1, int b1, int r2, int g2, int b2, int direct, int stayon, int startpixel, int delaytime)   //Direct: CW=1, CCW = !1
// void rollup(int r, int g, int b, int delaytimeup, int delaytimedn)                                                            //lights house from bottom up
// void rotatepixarray(int dir)                                                                                                  //rotate array elements CW or CCW
// void sparkle(int numsparkles, int maxbrightness, unsigned int numtimes, int delaytime)                                       //randomly turns on and off pixels with random color
// void sparkleonecolor(int r, int g, int b, int numsparkles, unsigned int numtimes, int delaytime)                             //randomly generates specified RGB lights
// void setall(int onoff)                                                                                                       //sets pixarray[][] to either all active or all inactive
// void setpixel(int pixel, int r, int g int b)                                                                                 //sets pixel to given color. still need to call pixels.show
// void setpixarray(int pixel, int onoff, int r, int g, int b)                                                                  //sets pixel in pixarray to rgb
// void showpixarray()                                                                                                          //loads pixarray into the LED strip. still need to call pixels.show
// void spooky(int r, int g, int b, int pixel, int delaytime)                                                                   //brings a pixel up from black to RGB, then back down
// void spookyall(int r, int g, int b, int delaytime)                                                                           //brings all pixels up from black to RGB, then back down
// void spookyuppixarray(int delaytime)
// void spookydownpixarray(int delaytime)
// void wipepixarray()                                                                                                          //sets all array elements to 0;
//
void active(int pixel) {
  pixarray[pixel][0] = 1;
}
void inactive(int pixel) {
  pixarray[pixel][0] = 0;
}


void chase(int r, int g, int b, int stayon, int startpixel, int endpixel, int delaytime) { //Direct: CW=1, CCW = !1
  uint8_t i;
  uint8_t oldr;
  uint8_t oldg;
  uint8_t oldb;
  uint32_t fullrgb;
  /*  if (endpixel == startpixel) {
      fullrgb = pixels.getPixelColor(startpixel);
      pixels.setPixelColor(startpixel, pixels.Color(r, g, b));
      pixels.show(); // This sends the updated pixel color to the hardware.
      if (!stayon){
        pixels.setPixelColor(startpixel, fullrgb);
      }
      delay(delaytime);
    }
  */
  if (endpixel >= startpixel) {
    for (i = startpixel; i <= endpixel; i++ ) {
      if (i > NUMPIXELS)                  //somehow an error occurred
        return;
      if (i < 0)                          //somehow an error occurred
        return;
      fullrgb = pixels.getPixelColor(i);
      //      oldr = getred(fullrgb);
      //      oldg = getgreen(fullrgb);
      //      oldb = getblue(fullrgb);
      pixels.setPixelColor(i, pixels.Color(r, g, b));
      pixels.show(); // This sends the updated pixel color to the hardware.
      if (!stayon)
        //        pixels.setPixelColor(i, pixels.Color(oldr, oldg, oldb));
        pixels.setPixelColor(i, fullrgb);
      delay(delaytime);
    }
  }
  else {
    for (i = startpixel; i >= endpixel; i--) {
      if (i > NUMPIXELS)                  //somehow an error occurred
        return;
      if (i < 0)                          //somehow an error occurred
        return;
      fullrgb = pixels.getPixelColor(i);
      //      oldr = getred(fullrgb);
      //      oldg = getgreen(fullrgb);
      //      oldb = getblue(fullrgb);
      pixels.setPixelColor(i, pixels.Color(r, g, b));
      pixels.show(); // This sends the updated pixel color to the hardware.
      if (!stayon)
        //        pixels.setPixelColor(i, pixels.Color(oldr, oldg, oldb));
        pixels.setPixelColor(i, fullrgb);
      delay(delaytime);
    }
  }
  pixels.show(); //cleanup so the last pixel isn't left on
}

void chasecircle(int r, int g, int b, int direct, int stayon, int startpixel, int delaytime) { //Direct: CW=1, CCW = !1
  int dir;  // direction
  int i;
  int endcount;
  int increment;
  int circlecount = startpixel;
  int leds[NUMPIXELS];
  for (i = 0; i < NUMPIXELS; i++) { //fill led[] with the right order of pixels
    if (circlecount == NUMPIXELS) {
      circlecount = 0;
    }
    leds[i] = circlecount++;
  }
  if (direct) {
    dir = 0;
    endcount = NUMPIXELS;
    increment = 1;
  }
  else {
    dir = NUMPIXELS - 1;
    endcount = -1;
    increment = -1;
  }
  for (i = dir; i != endcount; i += increment) {
    if (direct && i > endcount)
      return;
    if (!direct && i < 0)
      return;
    pixels.setPixelColor(leds[i], pixels.Color(r, g, b));
    pixels.show(); // This sends the updated pixel color to the hardware.
    if (!stayon)
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    delay(delaytime);
  }
  pixels.show(); //cleanup so the last pixel isn't left on
}

//Flashes the entire string at a given color for a specified number of times and delay
void flash(int r, int g, int b, int numflashes, int delaytime) {
  int i;  //LED counter
  int c;
  for (c = 0; c < numflashes; c++) {
    for (i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(r, g, b));
    }
    pixels.show();
    delay(delaytime);
    for (i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }
}

//Flashes a single pixel at a given color for a specified number of times and delay
void flashpixel(int r, int g, int b, int pixel, int numflashes, int delaytime) {
  int i;  //LED counter
  int c;
  for (c = 0; c < numflashes; c++) {
    pixels.setPixelColor(pixel, pixels.Color(r, g, b));
    pixels.show();
    delay(delaytime);
    pixels.setPixelColor(pixel, pixels.Color(0, 0, 0));
    pixels.show();
  }
}

uint8_t getred(uint32_t color)  // Returns the Red component of a 32-bit color
{
  return (color >> 16) & 0xFF;
}
uint8_t getgreen(uint32_t color) // Returns the Green component of a 32-bit color
{
  return (color >> 8) & 0xFF;
}
uint8_t getblue(uint32_t color)  // Returns the Blue component of a 32-bit color
{
  return color & 0xFF;
}


//Basically a chase through the string with a flash at the end
//strip is set to black afterwards.
void lightning(int r, int g, int b, int dir, int delaytime) {
  if (dir)
    chase(r, g, b, 0, 0, NUMPIXELS, delaytime);  //Direct: CW=1, CCW = !1
  else
    chase(r, g, b, 0, NUMPIXELS, 0, delaytime); //Direct: CW=1, CCW = !1
  if (r < 80) r += 20;
  if (g < 80) g += 20;
  if (b < 80) b += 20;
  flash(r, g, b, 5, 10);
  delay(10);
  chase(0, 0, 0, 1, 0, NUMPIXELS, 10);  //Direct: CW=1, CCW = !1
}

//fills pixarray with the current colors in the LED strip
void loadpixarray() {
  int c = 0;
  uint32_t fullrgb;
  for (c = 0; c < NUMPIXELS; c++) {
    fullrgb = pixels.getPixelColor(c);
    pixarray[c][1] = (int)getred(fullrgb);
    pixarray[c][2] = (int)getgreen(fullrgb);
    pixarray[c][3] = (int)getblue(fullrgb);
  }
}

//Bounces two colors of lights from either midpoint to ends or ends to midpoint
void pingpong(int r1, int g1, int b1, int r2, int g2, int b2, int direct, int stayon, int delaytime) { //Direct: CW=1, CCW = !1
  int dir;  // direction
  int i;
  int p1;
  int p1inc; //pixel 1 increment
  int p2;
  int p2inc; //pixel 2 increment
  int endcount;
  int increment;

  if (direct) {
    endcount = NUMPIXELS / 2;
    if (NUMPIXELS % 2) endcount++; //account for odd number of pixels
    p1 = 0;
    p1inc = 1;
    p2 = NUMPIXELS - 1;
    p2inc = -1;
    increment = 1;
  }
  else {
    endcount = NUMPIXELS / 2;
    if (NUMPIXELS % 2) endcount++; //account for odd number of pixels
    p1 = NUMPIXELS / 2 - 1;
    p1inc = -1;
    p2 = NUMPIXELS / 2;
    p2inc = 1;
    increment = 1;
  }
  for (i = 0; i < endcount; i++) {
    if (direct && i > endcount)
      return;
    if (!direct && i < 0)
      return;
    pixels.setPixelColor(p1, pixels.Color(r1, g1, b1));
    pixels.setPixelColor(p2, pixels.Color(r2, g2, b2));
    pixels.show(); // This sends the updated pixel color to the hardware.
    if (!stayon) {
      pixels.setPixelColor(p1, pixels.Color(0, 0, 0));
      pixels.setPixelColor(p2, pixels.Color(0, 0, 0));
    }
    delay(delaytime);
    pixels.show(); //cleanup so we don't leave the last pixel on
    p1 += p1inc;
    p2 += p2inc;
    //    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
}

void pingpongcircle(int r1, int g1, int b1, int r2, int g2, int b2, int direct, int stayon, int startpixel, int delaytime) { //Direct: CW=1, CCW = !1
  int dir;  // direction
  int i;
  int p1;
  int p1inc; //pixel 1 increment
  int p2;
  int p2inc; //pixel 2 increment
  int endcount;
  int increment;
  int circlecount = startpixel;
  int leds[NUMPIXELS];
  for (i = 0; i < NUMPIXELS; i++) { //fill led[] with the right order of pixels
    if (circlecount == NUMPIXELS) {
      circlecount = 0;
    }
    leds[i] = circlecount++;
  }
  if (direct) {
    endcount = NUMPIXELS / 2;
    p1 = 0;
    p1inc = 1;
    p2 = NUMPIXELS - 1;
    p2inc = -1;
    increment = 1;
  }
  else {
    endcount = NUMPIXELS / 2;
    p1 = NUMPIXELS / 2 - 1;
    p1inc = -1;
    p2 = NUMPIXELS / 2;
    p2inc = 1;
    increment = 1;
  }
  for (i = 0; i < endcount; i++) {
    if (direct && i > endcount)
      return;
    if (!direct && i < 0)
      return;
    pixels.setPixelColor(leds[p1], pixels.Color(r1, g1, b1));
    pixels.setPixelColor(leds[p2], pixels.Color(r2, g2, b2));
    pixels.show(); // This sends the updated pixel color to the hardware.
    if (!stayon) {
      pixels.setPixelColor(p1, pixels.Color(0, 0, 0));
      pixels.setPixelColor(p2, pixels.Color(0, 0, 0));
    }
    delay(delaytime);
    pixels.show(); //cleanup so we don't leave the last pixel on
    p1 += p1inc;
    p2 += p2inc;
    //    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
}


//rotates pixels in pixarry CW or CCW
void rotatepixarray(int dir) {
  int temparray[1][4] = {{0}};
  int c;
  if (dir) {
    temparray[0][0] = pixarray[NUMPIXELS - 1][0]; // store the last pixel's data
    temparray[0][1] = pixarray[NUMPIXELS - 1][1];
    temparray[0][2] = pixarray[NUMPIXELS - 1][2];
    temparray[0][3] = pixarray[NUMPIXELS - 1][3];
    for (c = NUMPIXELS - 1; c > 0; c--) {       //copy adjacent pixels
      pixarray[c][0] = pixarray[c - 1][0];
      pixarray[c][1] = pixarray[c - 1][1];
      pixarray[c][2] = pixarray[c - 1][2];
      pixarray[c][3] = pixarray[c - 1][3];
    }
    pixarray[0][0] = temparray[0][0];       //restore pixel 0 from stored NUMPIXEL
    pixarray[0][1] = temparray[0][1];
    pixarray[0][2] = temparray[0][2];
    pixarray[0][3] = temparray[0][3];
  }
  else {
    temparray[0][0] = pixarray[0][0];   // store the first pixel's data
    temparray[0][1] = pixarray[0][1];
    temparray[0][2] = pixarray[0][2];
    temparray[0][3] = pixarray[0][3];
    for (c = 0; c < NUMPIXELS; c++) {         //copy adjacent pixels
      pixarray[c][0] = pixarray[c + 1][0];
      pixarray[c][1] = pixarray[c + 1][1];
      pixarray[c][2] = pixarray[c + 1][2];
      pixarray[c][3] = pixarray[c + 1][3];
    }
    pixarray[NUMPIXELS - 1][0] = temparray[0][0];     //restore last pixel from stored NUMPIXEL
    pixarray[NUMPIXELS - 1][1] = temparray[0][1];
    pixarray[NUMPIXELS - 1][2] = temparray[0][2];
    pixarray[NUMPIXELS - 1][3] = temparray[0][3];
  }
}


//sets the active/inactive status for the entire pixarray 
void setall(int onoff) {
  for (int c = 0; c < NUMPIXELS; c++) {
    pixarray[c][0] = onoff;
  }
}

//sets a pixel in pixarray[][] to either active or inactive an a given color
void setpixarray(int pixel, int onoff, int r, int g, int b) {
  pixarray[pixel][0] = onoff;
  pixarray[pixel][1] = r;
  pixarray[pixel][2] = g;
  pixarray[pixel][3] = b;
}


// sets an individual pixel to given color
// must still call pixels.show()
void setpixel(int pixel, int r, int g, int b) {
  pixels.setPixelColor(pixel, pixels.Color(r, g, b));
}

//loads the LED strip with colors from pixarray.  Must still call pixels.show()
void showpixarray() {
  int c = 0;
  for (c = 0; c < NUMPIXELS; c++) {
    pixels.setPixelColor(c, pixels.Color(pixarray[c][1], pixarray[c][2], pixarray[c][3]));
  }
}

//randomly generates colored lights
void sparkle(int numsparkles, int maxbrightness, unsigned int numtimes, int delaytime) {
  // Randomly flashes numsparkles pixels on and off numtimes with a delay
  if (numsparkles > NUMPIXELS || numsparkles < 1 ) // Check bounds of the LEDs
    return;
  int counter = 0;
  int i;
  int c;
  int sparkles[numsparkles][4];  // first element holds the led number, the next three hold RGB values
  for (counter = 0; counter < numtimes; counter++) {
    for (i = 0; i < numsparkles; i++) {
      sparkles[i][0] = random(0, NUMPIXELS);
      sparkles[i][1] = random(0, maxbrightness);
      sparkles[i][2] = random(0, maxbrightness);
      sparkles[i][3] = random(0, maxbrightness);
    }
    for (i = 0; i < numsparkles; i++) {
      pixels.setPixelColor(sparkles[i][0], pixels.Color(sparkles[i][1], sparkles[i][2], sparkles[i][3])); //random pixels, random colors
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    for (i = 0; i < numsparkles; i++) {
      pixels.setPixelColor(sparkles[i][0], pixels.Color(0, 0, 0)); //random pixel, random color
    }
    delay(delaytime); // Delay for a period of time (in milliseconds).
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}

//randomly generates specified RGB lights
void sparkleonecolor(int r, int g, int b, int numsparkles, unsigned int numtimes, int delaytime) {
  // Randomly flashes numsparkles pixels on and off numtimes with a delay
  if (numsparkles > NUMPIXELS || numsparkles < 1 ) // Check bounds of the LEDs
    return;
  int counter = 0;
  int i;
  int c;
  int sparkles[numsparkles][4];  // first element holds the led number, the next three hold RGB values
  for (counter = 0; counter < numtimes; counter++) {
    for (i = 0; i < numsparkles; i++) {
      sparkles[i][0] = random(0, NUMPIXELS);
      sparkles[i][1] = r;
      sparkles[i][2] = g;
      sparkles[i][3] = b;
    }
    for (i = 0; i < numsparkles; i++) {
      pixels.setPixelColor(sparkles[i][0], pixels.Color(sparkles[i][1], sparkles[i][2], sparkles[i][3])); //random pixels, RGB color
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    for (i = 0; i < numsparkles; i++) {
      pixels.setPixelColor(sparkles[i][0], pixels.Color(0, 0, 0)); //set pixels to black
    }
    delay(delaytime); // Delay for a period of time (in milliseconds).
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}


//using pixarray[][], fades the active pixels up from black to the color stored in the array
void spookyuppixarray(int delaytime) {
  int i;
  int c;
  int rowcount = 0;
  int colcount = 0;
  float rscale;
  float gscale;
  float bscale;
  float rval;
  float gval;
  float bval;
  float pixarrayscale[NUMPIXELS][4] = {{0}};
  for (rowcount = 0; rowcount < NUMPIXELS; rowcount++) {
    pixarrayscale[rowcount][colcount] = (float) pixarray[rowcount][colcount];       //copy the first element to indicate ON or OFF
    for (colcount = 1; colcount < 4; colcount++) {
      if (pixarray[rowcount][colcount] == 0) pixarrayscale[rowcount][colcount] = 0; else pixarrayscale[rowcount][colcount] = (float) pixarray[rowcount][colcount] / 35; //set the RGB scale factors
    }
    colcount = 0;
  }
  for (i = 0; i < 35; i++) {
    for (rowcount = 0; rowcount < NUMPIXELS; rowcount++) {
      rval = (i * pixarrayscale[rowcount][1]);
      gval = (i * pixarrayscale[rowcount][2]);
      bval = (i * pixarrayscale[rowcount][3]);
      if (rval > pixarray[rowcount][1]) rval = pixarray[rowcount][1];
      if (gval > pixarray[rowcount][2]) gval = pixarray[rowcount][2];
      if (bval > pixarray[rowcount][3]) bval = pixarray[rowcount][3];
      if (pixarray[rowcount][0]) pixels.setPixelColor(rowcount, pixels.Color((int)rval, (int)gval, (int)bval));
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delaytime);
  }
  /*  for (i = 35; i >= 0; i--) {
    rval = i * rscale;
    gval = i * gscale;
    bval = i * bscale;
    if (rval > r) rval = r;
    if (gval > g) gval = g;
    if (bval > b) bval = b;
    for (c = 0; c < NUMPIXELS; c++) {
      pixels.setPixelColor(c, pixels.Color((int)rval, (int)gval, (int)bval));
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delaytime);
    }
  */
}

//using pixarray[][], takes the active pixels in the array down to black
void spookydownpixarray(int delaytime) {
  int i;
  int c;
  int rowcount = 0;
  int colcount = 0;
  float rscale;
  float gscale;
  float bscale;
  float rval;
  float gval;
  float bval;
  float pixarrayscale[NUMPIXELS][4] = {{0}};
  for (rowcount = 0; rowcount < NUMPIXELS; rowcount++) {
    pixarrayscale[rowcount][colcount] = (float) pixarray[rowcount][colcount];       //copy the first element to indicate ON or OFF
    for (colcount = 1; colcount < 4; colcount++) {
      if (pixarray[rowcount][colcount] == 0) pixarrayscale[rowcount][colcount] = 0; else pixarrayscale[rowcount][colcount] = (float) pixarray[rowcount][colcount] / 35; //set the RGB scale factors
    }
    colcount = 0;
  }
  for (i = 35; i >= 0; i--) {
    for (rowcount = 0; rowcount < NUMPIXELS; rowcount++) {
      rval = (i * pixarrayscale[rowcount][1]);
      gval = (i * pixarrayscale[rowcount][2]);
      bval = (i * pixarrayscale[rowcount][3]);
      if (rval > pixarray[rowcount][1]) rval = pixarray[rowcount][1];
      if (gval > pixarray[rowcount][2]) gval = pixarray[rowcount][2];
      if (bval > pixarray[rowcount][3]) bval = pixarray[rowcount][3];
      if (pixarray[rowcount][0]) pixels.setPixelColor(rowcount, pixels.Color((int)rval, (int)gval, (int)bval));
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delaytime);
  }
  /*  for (i = 35; i >= 0; i--) {
      rval = i * rscale;
      gval = i * gscale;
      bval = i * bscale;
      if (rval > r) rval = r;
      if (gval > g) gval = g;
      if (bval > b) bval = b;
      for (c = 0; c < NUMPIXELS; c++) {
        pixels.setPixelColor(c, pixels.Color((int)rval, (int)gval, (int)bval));
      }
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(delaytime);
    }
  */
}

//fades the entire string up from black and then back down
void spookyall(int r, int g, int b, int delaytime) {
  int i;
  int c;
  float rscale;
  float gscale;
  float bscale;
  float rval;
  float gval;
  float bval;
  if (r == 0) {
    rscale = 0;
  }
  else {
    rscale = (float)r / 35;
  }
  if (g == 0) {
    gscale = 0;
  } else {
    gscale = (float)g / 35;
  }
  if (b == 0) {
    bscale = 0;
  } else {
    bscale = (float)b / 35;
  }
  for (i = 0; i < 35; i++) {
    rval = i * rscale;
    gval = i * gscale;
    bval = i * bscale;
    if (rval > r) rval = r;
    if (gval > g) gval = g;
    if (bval > b) bval = b;
    for (c = 0; c < NUMPIXELS; c++) {
      pixels.setPixelColor(c, pixels.Color((int)rval, (int)gval, (int)bval));
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delaytime);
  }
  for (i = 35; i >= 0; i--) {
    rval = i * rscale;
    gval = i * gscale;
    bval = i * bscale;
    if (rval > r) rval = r;
    if (gval > g) gval = g;
    if (bval > b) bval = b;
    for (c = 0; c < NUMPIXELS; c++) {
      pixels.setPixelColor(c, pixels.Color((int)rval, (int)gval, (int)bval));
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delaytime);
  }
}

//fades a single pixel up from black and then back down
void spooky(int r, int g, int b, int pixel, int delaytime) {
  int i;
  float rscale;
  float gscale;
  float bscale;
  float rval;
  float gval;
  float bval;
  if (r == 0) {
    rscale = 0;
  }
  else {
    rscale = (float)r / 35;
  }
  if (g == 0) {
    gscale = 0;
  } else {
    gscale = (float)g / 35;
  }
  if (b == 0) {
    bscale = 0;
  } else {
    bscale = (float)b / 35;
  }
  for (i = 0; i < 35; i++) {
    rval = i * rscale;
    gval = i * gscale;
    bval = i * bscale;
    if (rval > r) rval = r;
    if (gval > g) gval = g;
    if (bval > b) bval = b;
    pixels.setPixelColor(pixel, pixels.Color((int)rval, (int)gval, (int)bval));
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delaytime);
  }
  for (i = 35; i >= 0; i--) {
    rval = i * rscale;
    gval = i * gscale;
    bval = i * bscale;
    if (rval > r) rval = r;
    if (gval > g) gval = g;
    if (bval > b) bval = b;
    pixels.setPixelColor(pixel, pixels.Color((int)rval, (int)gval, (int)bval));
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delaytime);
  }
}

//clears all values in pixarray[][]
void wipepixarray() {
  int r; //row
  int c; //column
  for (r = 0; r < NUMPIXELS; r++) {
    for (c = 0; c < 4; c++) {
      pixarray[r][c] = 0;
    }
  }
}
