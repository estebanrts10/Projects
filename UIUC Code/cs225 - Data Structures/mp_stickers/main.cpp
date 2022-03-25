#include "Image.h"
#include "StickerSheet.h"
using namespace cs225;
int main() {
  Image pic;
  Image iSticker;
  pic.readFromFile("guy.png");
  iSticker.readFromFile("illuminati.png");
  iSticker.scale(100,100);
  StickerSheet sheet(pic,3);
  sheet.addSticker(iSticker,350,250);
  iSticker.scale(50,50);
  sheet.addSticker(iSticker,400,350);
  sheet.addSticker(iSticker,300,350);
  Image picture = sheet.render();
  picture.writeToFile("myImage.png");
  return 0;
}
