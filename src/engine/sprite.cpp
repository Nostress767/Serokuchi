#include "engine/sprite.hpp"

Sprite::Sprite(PngMedia *im){
  x = y = 0;

  if(im->getData().empty())
    im->decode();

  width = im->getWidth();
  height = im->getHeight();
  channels = im->getChannels();

  data = im->getData();
}

