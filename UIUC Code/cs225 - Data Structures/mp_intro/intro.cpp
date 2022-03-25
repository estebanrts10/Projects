#include "cs225/PNG.h"
#include "cs225/HSLAPixel.h"
#include <cmath>
#include <string>

using cs225::PNG;
using cs225::HSLAPixel;

void rotate(std::string inputFile, std::string outputFile) {
  PNG * inPic = new PNG();
  inPic->readFromFile(inputFile);


  int width = inPic->width();
  int height = inPic->height();
  PNG * outPic = new PNG(width,height);

  for(int x=width-1;x>=0;x--){
    for(int y=height-1;y>=0;y--){ // image width and height must be changed for real numbers
      HSLAPixel & inPixel = inPic->getPixel(x,y);
      HSLAPixel & outPixel = outPic->getPixel(width-x-1,height-y-1);
      outPixel = inPixel;

    }
  }
       outPic->writeToFile(outputFile);

  }





cs225::PNG myArt(unsigned int width, unsigned int height) {
  //cs225::PNG png(width, height);
  PNG * myPic = new PNG(width,height);

  for(unsigned x=0;x<width;x++){
    for(unsigned y=0;y<height;y++){
      HSLAPixel & pixel = myPic->getPixel(x,y);
      pixel.h = float(x+y)*(x+y)/float((x-y)*(x-y));
      pixel.s = 1;
      pixel.l = (.5-(1.0/(x-y)));;
      pixel.a = 1;

    //  if((float(x+y)*(x+y)/float((x-y)*(x-y)))>360){
    //    pixel.h = 1;
    //  }
/*
//E
      if(y>775 && x < 75){pixel.l = 1;} //bottom horizontal
      if(y>625 && x<25){pixel.l = 1;}   //left E bar
      if(y>700 && y<725 && x<75){pixel.l = 1;}//mid hor
      if(y>625 && y<650 && x<75){pixel.l = 1;}//top hor
//C
      if(y>625 && x>100 && x<125){pixel.l = 1;}
      if(y>625 && y<650 && x>100 && x<175){pixel.l = 1;}//C
      if(y>775 && x>100 && x<175){pixel.l = 1;}
//E
      if(y>775 && x < 275 && x>200){pixel.l = 1;} //bottom horizontal
      if(x>200 && x<225 && y>625){pixel.l = 1;}   //left E bar
      if(y>700 && y<725 && x<275 && x>200){pixel.l = 1;}//mid hor
      if(y>625 && y<650 && x<275 && x>200){pixel.l = 1;}//top hor
*/


    }
  }










  return *myPic;
}
