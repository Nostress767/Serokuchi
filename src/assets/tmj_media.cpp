#include "assets/tmj_media.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "engine/clock.hpp"
#include "external/stb_image.h"

// TODO
TmjMedia::TmjMedia(std::string file, u8* bin, size_t sz) : Media(bin, sz) {
  loadMap();
}

//TODO: change tabs
int base64_decode(const std::string input, char *output, size_t output_len) {
  size_t input_len = input.size();
  // Base64 characters lookup table
  const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  size_t input_idx = 0, output_idx = 0;
  unsigned int buffer = 0;
  int bits_remaining = 0;

  while (input_idx < input_len) {
    char c = input[input_idx++];
    if (c == '=') break; // Padding, end of data

    // Find the character's position in the Base64 lookup table
    const char *ptr = strchr(base64_chars, c);
    if (ptr == NULL) {
      // Invalid character, not part of Base64 encoding
      return -1;
    }
    int value = ptr - base64_chars;

    // Append the bits of the current Base64 character to the buffer
    buffer = (buffer << 6) | value;
    bits_remaining += 6;

    // Convert 8 bits at a time to binary and store in the output
    while (bits_remaining >= 8 && output_idx < output_len) {
      bits_remaining -= 8;
      output[output_idx++] = (buffer >> bits_remaining) & 0xFF;
    }
  }

  return output_idx; // Return the length of the decoded binary data
}

std::shared_ptr<i32[]> bytes_to_integers(const char* input, int input_len, int* output_len) {
  if (input_len % sizeof(int) != 0) {
    log_error("Input length is not a multiple of the integer size.");
    return NULL;
  }

  *output_len = input_len / sizeof(int);
  std::shared_ptr<i32[]> output_array = std::make_shared<i32[]>(*output_len);

  for (int i = 0; i < *output_len; i++) {
    output_array[i] = 0;

    // Convert each byte to an integer in little-endian format
    for (size_t j = 0; j < sizeof(int); j++) {
      output_array[i] |= (unsigned char)input[i * sizeof(int) + j] << (8 * j);
    }
  }

  return output_array;
}

TMJKeys TmjMedia::lexify(const char* token, TMJKeys object){
  // NOTE: this might be better written (maintainable) with X-Macros
  switch(object){
    case mapa:
      if(     strcmp(token, "layers"          ) == 0) return mapLayer;
      else if(strcmp(token, "tilesets"        ) == 0) return mapTileset;
      else if(strcmp(token, "width"           ) == 0) return mapWidth;
      else if(strcmp(token, "height"          ) == 0) return mapHeight;
      else if(strcmp(token, "infinite"        ) == 0) return mapInfinite;
      else if(strcmp(token, "compressionlevel") == 0) return mapCompressionLevel;
      else if(strcmp(token, "orientation"     ) == 0) return mapOrientation;
      else if(strcmp(token, "renderorder"     ) == 0) return mapRenderOrder;
      else if(strcmp(token, "tiledversion"    ) == 0) return mapTiledVersion;
      else if(strcmp(token, "version"         ) == 0) return mapVersion;
      else if(strcmp(token, "tilewidth"       ) == 0) return mapTileWidth;
      else if(strcmp(token, "tileheight"      ) == 0) return mapTileHeight;
      else if(strcmp(token, "type"            ) == 0) return mapType;
      else if(strcmp(token, "nextlayerid"     ) == 0) return mapNextLayerId;
      else if(strcmp(token, "nextobjectid"    ) == 0) return mapNextObjectId;
      else return mapNotSupported;
    case mapLayer:
      if(     strcmp(token, "data"       ) == 0) return layerData;
      else if(strcmp(token, "id"         ) == 0) return layerId;
      else if(strcmp(token, "visible"    ) == 0) return layerVisible;
      else if(strcmp(token, "opacity"    ) == 0) return layerOpacity;
      else if(strcmp(token, "width"      ) == 0) return layerWidth;
      else if(strcmp(token, "height"     ) == 0) return layerHeight;
      else if(strcmp(token, "x"          ) == 0) return layerX;
      else if(strcmp(token, "y"          ) == 0) return layerY;
      else if(strcmp(token, "compression") == 0) return layerCompression;
      else if(strcmp(token, "encoding"   ) == 0) return layerEncoding;
      else if(strcmp(token, "type"       ) == 0) return layerType;
      else if(strcmp(token, "name"       ) == 0) return layerName;
      else return layerNotSupported;
    case mapTileset:
      if(     strcmp(token, "firstgid"   ) == 0) return tilesetFirstGid;
      else if(strcmp(token, "columns"    ) == 0) return tilesetColumns;
      else if(strcmp(token, "tilecount"  ) == 0) return tilesetTileCount;
      else if(strcmp(token, "tilewidth"  ) == 0) return tilesetTileWidth;
      else if(strcmp(token, "tileheight" ) == 0) return tilesetTileHeight;
      else if(strcmp(token, "name"       ) == 0) return tilesetName;
      else if(strcmp(token, "image"      ) == 0) return tilesetImage;
      else if(strcmp(token, "imagewidth" ) == 0) return tilesetImageWidth;
      else if(strcmp(token, "imageheight") == 0) return tilesetImageHeight;
      else if(strcmp(token, "margin"     ) == 0) return tilesetMargin;
      else if(strcmp(token, "spacing"    ) == 0) return tilesetSpacing;
      else return tilesetNotSupported;
    default:{
      log_error("Invalid object " << object);
      return invalidValue;
    }
  }
}

void TmjMedia::prepareParser(json_stream *json, TMJKeys object){
  enum json_type type = json_next(json);
  switch (type) {
    case JSON_STRING:
      size_t str_len; // This includes the NULL terminator
      json_get_string(json, &str_len);
      switch(object){
        default: break;
      }
      break;
    case JSON_ARRAY: // Consumed '['
      while (json_peek(json) != JSON_ARRAY_END && !json_get_error(json))
        prepareParser(json, object);
      json_next(json); // Consumed ']'
      break;
    case JSON_OBJECT: // Consumed '{'
      while (json_peek(json) != JSON_OBJECT_END && !json_get_error(json)) {
        json_next(json);

        const char* next_object_str = json_get_string(json, NULL);
        TMJKeys next_object = lexify(next_object_str, object);

        switch(next_object){
          case mapNotSupported: case layerNotSupported: case tilesetNotSupported:{
            // NOTE: this wasn't supposed to have a newline ('\n')
            // maybe alter log_* to add a newline'less version?
            log_error("[" << __func__ << "] WARNING: (key:value) pair (\"" << next_object_str << "\":");
          } break;
          default: break;
        }

        prepareParser(json, next_object);

        switch(next_object){
          case mapNotSupported:
            log_error(next_object_str << ") is not supported.");
            break;
          case layerNotSupported:
            log_error(next_object_str << ") from Layer " << layers.size() << " is not supported.");
            break;
          case tilesetNotSupported:
            log_error(next_object_str << ") from Tileset " << tilesets.size() << " is not supported.");
            break;
          default: break;
        }
      }
      switch(object){
        case mapLayer: layers.push_back({}); break;
        case mapTileset: tilesets.push_back({}); break;
        default: break;
      }
      json_next(json); // Consumed '}'
      break;
    case JSON_OBJECT_END: case JSON_ARRAY_END: return;
    case JSON_ERROR:{
      log_error("error: " << json_get_lineno(json) <<": " << json_get_error(json));
      exit(EXIT_FAILURE);
    }
    default: break;
  }
}

void TmjMedia::parse(json_stream *json, TMJKeys object){
  enum json_type type = json_next(json);
  switch (type) {
    /// NOTE: this might not be working correctly ## TEST LATER ##
    default: break;
    ///
    case JSON_NULL:{
      log_error("[" << __func__ << "] null value for enum \"" << object << "\".");
    }break;
    case JSON_TRUE:
      switch(object){
        case mapInfinite: infinite = true; break;
        case layerVisible: layers.back().visible = true; break;
        default: break;
      }
    case JSON_FALSE:
      switch(object){
        case mapInfinite: infinite = false; break;
        case layerVisible: layers.back().visible = false; break;
        default: break;
      }
    case JSON_NUMBER:{
      double n = json_get_number(json);
      switch(object){
        case mapWidth: width = (int)n; break;
        case mapHeight: height = (int)n; break;
        case mapCompressionLevel: compressionLevel = (int)n; break;
        case mapTileWidth: tileWidth = (int)n; break;
        case mapTileHeight: tileHeight = (int)n; break;
        case mapNextLayerId: nextLayerId = (int)n; break;
        case mapNextObjectId: nextObjectId = (int)n; break;
        case layerId: layers.back().id = (int)n; break;
        case layerWidth: layers.back().width = (int)n; break;
        case layerHeight: layers.back().height = (int)n; break;
        case layerX: layers.back().x = (int)n; break;
        case layerY: layers.back().y = (int)n; break;
        case layerOpacity: layers.back().opacity = n; break;
        case tilesetFirstGid: tilesets.back().firstGid = (int)n; break;
        case tilesetColumns: tilesets.back().columns = (int)n; break;
        case tilesetTileCount: tilesets.back().tileCount = (int)n; break;
        case tilesetTileWidth: tilesets.back().tileWidth = (int)n; break;
        case tilesetTileHeight: tilesets.back().tileHeight = (int)n; break;
        case tilesetImageWidth: tilesets.back().imageWidth = (int)n; break;
        case tilesetImageHeight: tilesets.back().imageHeight = (int)n; break;
        case tilesetMargin: tilesets.back().margin = (int)n; break;
        case tilesetSpacing: tilesets.back().spacing = (int)n; break;
        default: break;
      }
    } break;
    case JSON_STRING:{
      size_t str_len;
      const char* str = json_get_string(json, &str_len);
      switch(object){
        case mapOrientation: orientation = str; break;
        case mapRenderOrder: renderOrder = str; break;
        case mapTiledVersion: tiledVersion = str; break;
        case mapVersion: version = str; break;
        case mapType: this->type = str; break;
        case layerData: layers.back().encodedData = str; break;
        case layerCompression: layers.back().compression = str; break;
        case layerEncoding: layers.back().encoding = str; break;
        case layerType: layers.back().type = str; break;
        case layerName: layers.back().name = str; break;
        case tilesetName: tilesets.back().name = str; break;
        case tilesetImage: tilesets.back().image = str; break;
        default: break;
      }
      } break;
    case JSON_ARRAY: // Consumed '['
      while (json_peek(json) != JSON_ARRAY_END && !json_get_error(json))
        parse(json, object);
      json_next(json); // Consumed ']'
      break;
    case JSON_OBJECT: // Consumed '{'
      while (json_peek(json) != JSON_OBJECT_END && !json_get_error(json)) {
        json_next(json);

        const char* next_object_str = json_get_string(json, NULL);
        TMJKeys next_object = lexify(next_object_str, object);

        parse(json, next_object);
      }
      switch(object){
        case mapLayer: layers.push_back({}); break;
        case mapTileset: tilesets.push_back({}); break;
        default: break;
      }
      json_next(json); // Consumed '}'
      break;
    case JSON_OBJECT_END: case JSON_ARRAY_END: return;
  }
}

void TmjMedia::loadMap(){
  json_stream json;
  json_open_string(&json, (const char*)getRawData());
	json_set_streaming(&json, false);

  prepareParser(&json, mapa);

  if (json_get_error(&json)) {
    log_error("error: " << json_get_lineno(&json) << ": " << json_get_error(&json));
    exit(EXIT_FAILURE);
  }
  json_close(&json);

  json_open_string(&json, (const char*)getRawData());
	json_set_streaming(&json, false);
  parse(&json, mapa);
  json_close(&json);

  // TODO: verify that this is not needed anymore (since we use C++ and not just C now)
  //initialized = true;
}

void TmjMedia::loadLayer(Layers ly){
  //if(!initialized)
    //loadMap();

  if(!layers[ly].data){
    size_t base64_len = layers[ly].encodedData.size();
    // Max output length for Base64 decoding
    size_t max_output_len = (base64_len * 3) / 4;
    // First we decode Base64 to binary, to be able to decompress from zlib
    // TODO: shared'ify everything (every malloc)
    char* binary_data = (char*)malloc(max_output_len);
    int base64_decoded_len = base64_decode(layers[ly].encodedData, binary_data, max_output_len);

    if (base64_decoded_len < 0) {
      log_error("Invalid Base64 data from Map");
      exit(EXIT_FAILURE);
    }

    int zlib_decoded_len;
    char* zlib_decoded_data = stbi_zlib_decode_malloc(binary_data, base64_decoded_len, &zlib_decoded_len);
    int int_array_len;
    layers[ly].data = bytes_to_integers(zlib_decoded_data, zlib_decoded_len, &int_array_len);

    free(zlib_decoded_data);
    free(binary_data);
  }
}

void TmjMedia::unloadLayer(Layers ly){
  //free(layers[ly].data);
  //struct Layer tmp = {0};
  //memcpy(&layers[ly], &tmp, sizeof(tmp));
}

// TODO: make sure png and mp3 also free and zero everything
void TmjMedia::unloadMap(){
  for(int i = 0; i < MAX_LAYERS; i++){
    // XXX
    unloadLayer(static_cast<Layers>(i));
    layers[i].encodedData.clear();
  }
  //struct TmjMedia tmp = {0};
  //memcpy(&tmp, sizeof(tmp));
}

std::shared_ptr<i32[]> TmjMedia::getLayerData(Layers ly){
  loadLayer(ly);
  return layers[ly].data;
}

//int* GetLayerBuffer(enum Layers ly){
//  return get_layer_data(ly);
//}

i32 TmjMedia::getWidth(){
  return width;
}

i32 TmjMedia::getHeight(){
  return height;
}

