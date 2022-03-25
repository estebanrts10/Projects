#include "StickerSheet.h"
#include <cmath>
#include <iostream>


using namespace std;

namespace cs225 {
  StickerSheet::StickerSheet(const Image &picture, unsigned max){
    unsigned max_ = max;
    x_ = new unsigned[max_];
    y_ = new unsigned[max_];
    pic = new Image(picture);
    counter_ = 0;
    sticker_ = new Image*[max_];
    for(unsigned i=0; i<max;i++){
      sticker_[i] = new Image();
      x_[i] = 0;
      y_[i] = 0;
    }
  }

  StickerSheet::~StickerSheet(){
    delete_memory();
  }
  void StickerSheet::delete_memory(){
    if(pic != NULL){
      delete pic;
      pic = NULL;
    }
    for(unsigned i = 0; i<max_;i++){
      if(sticker_[i]!=NULL){
        delete sticker_[i];
        sticker_[i] = NULL;
      }
    }
    delete x_;
    x_ = NULL;
    delete y_;
    y_ = NULL;


  }


  StickerSheet::StickerSheet(const StickerSheet &other){
    //delete_memory();
    this->pic = new Image();
    *(this->pic) = *(other.pic);
    this->max_ = other.max_;
    x_ = new unsigned[max_];
    y_ = new unsigned[max_];
    for(unsigned i =0; i<max_;i++){
      this->sticker_[i] = other.sticker_[i];
      this->x_[i] = other.x_[i];
      this->y_[i] = other.y_[i];
  }
    }




  const StickerSheet & StickerSheet::operator=(const StickerSheet &other){
    delete_memory();
    this->pic = new Image();
    *(this->pic) = *(other.pic);
    this->max_ = other.max_;
    x_ = new unsigned[max_];
    y_ = new unsigned[max_];
    for(unsigned i =0; i<max_;i++){
      this->sticker_[i] = other.sticker_[i];
      this->x_[i] = other.x_[i];
      this->y_[i] = other.y_[i];
  }
    return *this;
  }


  void StickerSheet::changeMaxStickers(unsigned max){
    unsigned cMax=max_;
    while(max<cMax){
      delete sticker_[cMax-1];
      sticker_[cMax-1] = NULL;
      cMax--;
      counter_--;
    }
    if(max_<max){
      max_=max;
    }


  }


  int StickerSheet::addSticker(Image &sticker,unsigned x,unsigned y){
    if(counter_<max_){
      sticker_[counter_] = &sticker;
      x_[counter_] = x;
      y_[counter_] = y;
      counter_++;
      return counter_;
    }
    else
      return -1;

}



  bool StickerSheet::translate(unsigned index, unsigned x, unsigned y){
    if(sticker_[index]==NULL){
      return false;
    }
    x_[index] = x;
    y_[index] = y;

    return true;

  }


  void StickerSheet::removeSticker(unsigned index){
    if(index<max_){
      delete sticker_[index];
      x_[index] = 0;
      y_[index] = 0;
      counter_--;
    }
  }


  Image * StickerSheet::getSticker(unsigned index){
    if(index>=max_){
      return NULL;
    }
    return sticker_[index];

  }


  Image StickerSheet::render()const{
    unsigned maxWidth = pic->width();
    unsigned maxHeight = pic->height();
    Image outPic;
    outPic.resize(maxWidth,maxHeight);
    for(unsigned i =0;i<max_;i++){
      if(maxWidth < sticker_[i]->width() + *(x_)){
        maxWidth = sticker_[i]->width() + *(x_);
      }
      if(maxHeight < sticker_[i]->height() + *(y_)){
        maxHeight = sticker_[i]->height() + *(y_);
      }
      for(unsigned x=0;x<pic->width();x++){
        for(unsigned y=0;y<pic->height();y++){
          HSLAPixel & pixel = pic->getPixel(x,y);
          HSLAPixel & outPixel = outPic.getPixel(x,y);
          outPixel = pixel;

        }
      }
      for(unsigned x=0;x<sticker_[i]->width()+*(x_);x++){
        for(unsigned y=0;y<sticker_[i]->height()+*(y_);y++){
          HSLAPixel & pixel = sticker_[i]->getPixel(x,y);
          HSLAPixel & outPixel = outPic.getPixel(x,y);
          outPixel.h = pixel.h;
          outPixel.s = pixel.s;
          outPixel.l = pixel.l;
          outPixel.a = pixel.a;
        }
      }
    }



    return outPic;



  }
}
