/**
 * @file StickerSheet.h
 * Contains your declaration of the interface for the StickerSheet class.
 */
#pragma once
#include "Image.h"
 //#include

namespace cs225{
  class StickerSheet{
  public:
    StickerSheet(const Image &picture, unsigned max);
    ~StickerSheet();
    StickerSheet(const StickerSheet &other);
    const StickerSheet & operator=(const StickerSheet &other);
    void changeMaxStickers(unsigned max);
    int addSticker(Image &sticker,unsigned x,unsigned y);
    bool translate(unsigned index, unsigned x, unsigned y);
    void removeSticker(unsigned index);
    Image * getSticker(unsigned index);
    Image render()const;
    void delete_memory();
    void copy_(const StickerSheet &other);
  private:
    Image * pic;
    Image ** sticker_;
    unsigned* x_;
    unsigned* y_;
    unsigned max_;
    unsigned counter_;

  };
}
