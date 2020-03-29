/*
This program gives graph functionality for ventilator project 
Developer : Md. Tamjid Rayhan, Reference : https://github.com/KrisKasprzak/GraphingFunction/blob/master/Graph.ino
Date : 3/29/2020
License: Open to public
*/

#include <SPI.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <pressureSensor.h>



#define TFT_CS        10//15//10
#define TFT_RST        8//89//8 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        9//18// 9

#define DKBLUE    0x000D
#define RED       0xF800
#define GREEN     0x07E0
#define WHITE     0xFFFF
#define BLACK     0x0000




//Define a pressure sensor object
pressureSensor ps(A0);


//TFT Display object and related variables
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
int dWid ;//this is the display width (For ST7735 and rotation 3 this value is 160)
int dHi; //this is the display height (For ST7735 and rotation 3 this value is 128)



//This are flags to redraw the current screen
boolean displayPressure = true;

//Helper variables for drawing graph
double oldx ; double oldy  ;
int numPointsDrawn = 0;
double xMargin;
double yMargin;


//Time keeping variables
uint16_t nowTime;
uint16_t prevTime = 0;
uint16_t elapsedTime;


void setup() {

  Serial.begin(9600);

  pinMode(A0, INPUT);
  
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
 

  tft.setRotation(3);
  
  tft.fillScreen(BLACK);
  
  int dWid = tft.width(); 
  int dHi = tft.height(); 
  double xMargin = dWid/16;
  double yMargin = dHi/10;

}


void loop(void) {

nowTime = millis();
elapsedTime = nowTime - prevTime;

  if (elapsedTime>=250 && numPointsDrawn<= 96){
    prevTime = nowTime;
    double x = numPointsDrawn* 0.25;
    double y = ps.readCmWater();
    
    dWid = tft.width(); //this is the display width (For ST7735 and rotation 3 this value is 160)
    dHi = tft.height(); //this is the display height (For ST7735 and rotation 3 this value is 128)
    xMargin = dWid/16;
    yMargin = dHi/10;
    drawGraphPoint( tft ,/*Adafruit_ST7735 &d --> Give the display object name*/ 
                          x, /*double x --> x coordinate of point to be drawn in graph coordinates*/
                          y, /*double y --> y coordinate of point to be drawn in graph coordinates*/
                         xMargin ,/*double gx --> Give the x coordinate of graph origin point in display coordinates*/
                         dHi - 2*yMargin , /* double gy --> Give the y coordinate of graph origin point in display coordinates*/
                         dWid - 2*xMargin, /* double w --> Give the width of the graph or horizontal measurement*/
                         dHi - 3*yMargin, /*double h --> Give the height of the graph or vertical measurement*/
                          0,/*double xlo --> give the lower bound of x coordinate*/
                          24,/*double xhi --> give the upper bound of x coordinate*/
                          4,/*double xinc --> give the division as distance for x coordinate*/
                          xMargin, /*double xMargin --> Give the vertical margin width*/
                          0, /*double ylo --> give the lower bound of y coordinate*/
                          45, /*double yhi --> give the upper bound of y coordinate*/
                          10, /*double yinc --> give the division as distance for y coordinate*/
                          yMargin, /*double yMargin --> Give the horizontal margin width*/
                          "Pressure", /*String title --> Give the Title of the graph*/ 
                          "Time (s)",/*String xlabel --> Give label for x axis*/ 
                          "Pressure (cmH2O)",/*String ylabel --> Give label for y axis*/
                          DKBLUE, /*unsigned int gcolor --> Graph line colors*/
                          RED, /*unsigned int acolor --> axis line colors*/
                          GREEN, /*unsigned int pcolor --> color of plotted data*/
                          WHITE, /*unsigned int tcolor --> text color*/
                          BLACK, /*unsigned int bcolor --> Background color*/
                          displayPressure /* boolean &redraw --> Flag to redraw graph*/);
      numPointsDrawn++;
  }
  else if(elapsedTime>=250 && numPointsDrawn >96){
    numPointsDrawn = 0;
    displayPressure = true;
    }
  else{
    }

}

/*
  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn
  huge arguement list
  &d name of your display object
  x = x data point
  y = y datapont
  gx = x graph location (lower left)
  gy = y graph location (lower left)
  w = width of graph
  h = height of graph
  xlo = lower bound of x axis
  xhi = upper bound of x asis
  xinc = division of x axis (distance not count)
  ylo = lower bound of y axis
  yhi = upper bound of y asis
  yinc = division of y axis (distance not count)
  title = title of graph
  xlabel = x asis label
  ylabel = y asis label
  gcolor = graph line colors
  acolor = axi ine colors
  pcolor = color of your plotted data
  tcolor = text color
  bcolor = background color
  &redraw = flag to redraw graph on fist call only
*/






/* Draw the coordinate system from scratch if redraw is true
Otherwise just plot the current point*/
void drawGraphPoint(Adafruit_ST7735 &d, double x, double y, double gx, double gy, double w, double h,
                          double xlo, double xhi, double xinc, double xMargin,
                          double ylo, double yhi, double yinc, double yMargin,
                          String title, String xlabel, String ylabel,
                          unsigned int gcolor, unsigned int acolor, unsigned int pcolor,
                          unsigned int tcolor, unsigned int bcolor,
                          boolean &redraw){
  double i;
  double temp;
  
  if (redraw == true) {

    redraw = false;
    tft.fillScreen(BLACK);
    
    oldx = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oldy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    
        //Draw y scale axis and graph lines
        for ( i = ylo; i <= yhi; i += yinc) {
          // compute the transform
          temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    
          if (i == ylo) {
            d.drawLine(gx, temp, gx + w, temp, acolor);
          }
          else {
            d.drawLine(gx, temp, gx + w, temp, gcolor);
          }
          //write the yticks along y axis
          d.setTextSize(1);
          d.setTextColor(tcolor, bcolor);
          d.setCursor(gx - xMargin, temp-yMargin/2);
          // precision is default Arduino--this could really use some format control
          d.println(int(i));
        }

       // draw x scale
        for (i = xlo; i <= xhi; i += xinc) {
    
          // compute the transform
    
          temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
          if (i == xlo) {
            d.drawLine(temp, gy, temp, gy - h, acolor);
          }
          else {
            d.drawLine(temp, gy, temp, gy - h, gcolor);
          }
          //write the xticks along x axis
          d.setTextSize(1);
          d.setTextColor(tcolor, bcolor);
          d.setCursor(temp - xMargin/2, gy + yMargin/4);
          // precision is default Arduino--this could really use some format control
          d.println(int(i));
        }

        //write the xlabel along x axis
        d.setTextSize(1);
        d.setTextColor(acolor, bcolor);
        d.setCursor(gx + w/2 - xMargin, gy + 1.25* yMargin);
        d.println(xlabel);
    
        d.setTextSize(1);
        d.setTextColor(acolor, bcolor);
        d.setCursor(gx - xMargin, gy - h - yMargin);
        d.println(ylabel);
    }

    //draw graph points
    x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
    y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    d.drawLine(oldx, oldy, x, y, pcolor);
    //d.drawLine(ox, oy + 1, x, y + 1, pcolor);
    //d.drawLine(ox, oy - 1, x, y - 1, pcolor);
    oldx = x;
    oldy = y;
  }
