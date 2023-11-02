#include "assets/media.hpp"

Media::Media(u8 *binStart, size_t len) : binStart(binStart), len(len){}
const u8* Media::getRawData(){ return binStart; }
const size_t Media::size(){ return len; }

