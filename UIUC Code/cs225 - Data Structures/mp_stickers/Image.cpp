#include "Image.h"
#include <cmath>
#include <iostream>



using namespace std;


namespace cs225 {
//  void setWidth()
  void Image::lighten(){
    //get image from png

     for(unsigned x = 0;x< this->width();x++){
       for(unsigned y = 0; y< this->height(); y++){
         HSLAPixel & pixel = this->getPixel(x,y);
         if(pixel.l + .1 > 1){
           pixel.l = 1;
         }
         else{
         pixel.l = pixel.l + .1;
}
//gets an image and raises its luminance by .1
//if the luminance is greater than .9 then simply
//set the luminance to 1 as to not go out of bounds
       }
     }
   }
   void Image::lighten(double amount){
     for(unsigned x = 0;x< this->width();x++){
       for(unsigned y = 0; y< this->height(); y++){
         HSLAPixel & pixel = (*this).getPixel(x,y);
         if(pixel.l + amount > 1){
           pixel.l = 1;
         }
         else{pixel.l = pixel.l + amount;
         }
       }
   }
}
    void Image::darken(){
      for(unsigned x = 0;x<width();x++){
        for(unsigned y = 0; y<height(); y++){
          HSLAPixel & pixel = this->getPixel(x,y);
          if(pixel.l - .1 < 0){
            pixel.l = 0;
          }
          else{pixel.l = pixel.l - .1;
          }
    }
  }
}
    void Image::darken(double amount){
      for(unsigned x = 0;x<width();x++){
        for(unsigned y = 0; y<height(); y++){
          HSLAPixel & pixel = this->getPixel(x,y);
          if(pixel.l - amount < 0){
            pixel.l = 0;
          }
          else{pixel.l = pixel.l - amount;
          }
    }
  }
}
    void Image::saturate(){
      for(unsigned x = 0;x<width();x++){
        for(unsigned y = 0; y<height(); y++){
          HSLAPixel & pixel = this->getPixel(x,y);
          if(pixel.s + .2 > 1){
            pixel.s = 1;
          }
          else{pixel.s = pixel.s + .1;}
}}}


    void Image::saturate(double amount){
      for(unsigned x = 0;x<width();x++){
        for(unsigned y = 0; y<height(); y++){
          HSLAPixel & pixel = this->getPixel(x,y);
          if(pixel.s + amount > 1){
            pixel.s = 1;
          }
          else{pixel.s = pixel.s + amount;
          }

    }
  }
}
    void Image::desaturate(){
      for(unsigned x = 0;x<width();x++){
        for(unsigned y = 0; y<height(); y++){
          HSLAPixel & pixel = this->getPixel(x,y);
          if(pixel.s - .1 < 0){
            pixel.s = 0;
          }
          else{
          pixel.s = pixel.s - .1;
        }
      }
    }
}

    void Image::desaturate(double amount){
      for(unsigned x = 0;x<width();x++){
        for(unsigned y = 0; y<height(); y++){
          HSLAPixel & pixel = this->getPixel(x,y);
          if(pixel.s - amount < 0){
            pixel.s = 0;
          }
          else{
          pixel.s = pixel.s - amount;
        }
      }
    }
  }
    void Image::grayscale() {
      for (unsigned x = 0; x < width(); x++) {
        for (unsigned y = 0; y < height(); y++) {
          HSLAPixel & pixel = this->getPixel(x, y);
          pixel.s = 0;
      }
    }
  }
    void Image::rotateColor(double degrees){
      for(unsigned x = 0; x<width();x++){
        for(unsigned y = 0;y<height();y++){
          HSLAPixel & pixel = this->getPixel(x, y);
          pixel.h = pixel.h + degrees;
          if(pixel.h > 360){
            pixel. h = pixel.h - 360;
          }
          if(pixel.h < 0){
            pixel. h = pixel.h + 360;
          }
        }
      }

    }
    void Image::illinify(){
      for(unsigned x = 0; x<width();x++){
        for(unsigned y = 0;y<height();y++){
          HSLAPixel & pixel = this->getPixel(x, y);
          int orange = 11;
          int blue = 216;
        	if(pixel.h>113 && pixel.h<294){ // critical numbers(numbers that are barely closer to one or the other)
        		pixel.h = blue;}
        	else{
        		pixel.h = orange;}
        	}
}
    }
    void Image::scale(double factor){
      unsigned newWidth = (this->width() * factor);
      unsigned newHeight = (this->height() * factor);
      PNG * datImage = new PNG(*this);
      resize(newWidth,newHeight);
      for(unsigned x = 0; x<newWidth;x++){
        for(unsigned y = 0;y<newHeight;y++){
          HSLAPixel & datPixel = datImage->getPixel(double(x)/factor,double(y)/factor);
          HSLAPixel & pixel = this->getPixel(x,y);
          pixel.h = datPixel.h;
          pixel.s = datPixel.s;
          pixel.l = datPixel.l;
          pixel.a = datPixel.a;
    }
  }
  delete(datImage);
  datImage=0;
}
    void Image::scale(unsigned w,unsigned h){
      PNG * datImage = new PNG(*this);
      resize(w,h);
      double xFactor = (w*1.0)/(this->width()*1.0);
      double yFactor = (h*1.0)/(this->height()*1.0);
      for(unsigned x=0;x<w;x++){
        for(unsigned y=0;y<h;y++){
          HSLAPixel & datPixel = datImage->getPixel((x*1.0)/xFactor,(y*1.0)/yFactor);
          HSLAPixel & pixel = this->getPixel(x,y);
          pixel.h = datPixel.h;
          pixel.s = datPixel.s;
          pixel.l = datPixel.l;
          pixel.a = datPixel.a;
        }
      }


    }


}
