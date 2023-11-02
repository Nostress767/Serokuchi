#include "assets/png_media.hpp"
#include "external/stb_image.h"

PngMedia::PngMedia(u8* bin, size_t sz) : Media(bin, sz){
  width = height = channels = -1;
}

PngMedia::~PngMedia(){
  unload();
}

std::vector<std::vector<u8>>& PngMedia::getData(){ return data; }
i32 PngMedia::getWidth(){ return width; }
i32 PngMedia::getHeight(){ return height; }
i32 PngMedia::getChannels(){ return channels; }

void PngMedia::load(){ if(data.empty()) decode(); }

void PngMedia::decode(){
  u8 *reverseRows = stbi_load_from_memory(getRawData(), size(), &width, &height, &channels, 4);

  // Reverse row order (from "left to right, top to bottom" to "left to right, bottom to top")
  data.resize(height);
  for(i32 i = 0; i < height; i++){
    data[i].resize(4 * width);
    i64 offset = (height - 1 - i) * 4 * width;
    std::copy(reverseRows + offset, reverseRows + offset + 4 * width, data[i].begin());
  }

  stbi_image_free(reverseRows);
}

void PngMedia::unload(){
  data.clear();
  width = height = channels = -1;
}

