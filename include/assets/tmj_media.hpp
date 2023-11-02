#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "external/pdjson.h"
#include "media.hpp"

//TODO: change tabs
// TODO: maybe use enum class (avoid namespace collisions in the future)
enum TMJKeys {
    invalidValue = 0,
    mapa = 1, mapLayer, mapTileset,
    mapWidth, mapHeight, mapInfinite,
    mapCompressionLevel, mapOrientation,
    mapRenderOrder, mapTiledVersion, mapVersion,
    mapTileWidth, mapTileHeight,
    mapNextLayerId, mapNextObjectId,
    mapType, mapNotSupported,
    //mapTilesetN, mapLayerN,

    layerData, layerId, layerVisible,
    layerWidth, layerHeight,
    layerX, layerY,
    layerCompression, layerEncoding,
    layerType, layerName,
    layerOpacity,
    layerNotSupported,

    tilesetFirstGid, tilesetColumns,
    tilesetTileCount,
    tilesetTileWidth, tilesetTileHeight,
    tilesetName, tilesetImage,
    tilesetImageWidth, tilesetImageHeight,
    tilesetMargin, tilesetSpacing,
    tilesetNotSupported,
};

struct Layer {
    i32 id;
    i32 width, height;
    i32 x, y; // <- Have no idea what the use for this is
    bool visible;
    f64 opacity;
    std::string compression;//[16]; // Uncompressed, zlib or Zstandard (Only zlib supported for now)
    std::string encoding;//[16]; // CSV or Base64 (Only Base64 supported for now)
    std::string type;//[32]; // Only tilelayer supported for now
    //unsigned i32 ti32Color; // Of type #xxxxxxxx, not yet supported
    // float opacity;
    std::string name;//[256];
    std::string encodedData;
    std::shared_ptr<i32[]> data; // Base64-zlib converted to i32 array
};

struct TileSet{
    i32 firstGid;
    i32 columns;
    i32 tileCount;
    i32 tileWidth, tileHeight;
    std::string name;//[256];
    std::string image;//[256];
    i32 imageWidth, imageHeight;
    i32 margin, spacing;
};

// NOTE: this may be changed later
enum Layers{
    FLOOR = 0, WALLS, CEILING, // This order has to be respected in Tiled
    MAX_LAYERS                   // This means Ceiling on top, then Walls, then Floor
};

// TODO: put this inside the class
//i32* GetLayerBuffer(struct TmjMedia* md, enum Layers ly);
//i32 GetMapWidth(struct TmjMedia* md);
//i32 GetMapHeight(struct TmjMedia* md);

class TmjMedia : private Media{
private:
  // NOTE: size == len
  bool initialized;
  i32 width, height;
  bool infinite;
  i32 compressionLevel; // Hopefully always -1
  std::string orientation;//[32]; // Orthogonal, Isometric, Isometric (Staggered), Hexagonal (Staggered)
  std::string renderOrder;//[16]; // Right Down, Right Up, Left Down, Left Up,
  std::string tiledVersion;//[16]; // Made for Tiled version 1.10.1
  std::string version;//[16]; // Made for version 1.10
  i32 tileWidth, tileHeight;
  std::string type;//[16]; // Only map is supported for now
  i32 nextLayerId, nextObjectId;
  
  std::vector<Layer> layers;//[MAX_LAYERS]; // Ceiling, Walls and Floor (for now)
  std::vector<TileSet> tilesets;//[MAX_TILESETS];

public:
  TmjMedia(std::string file, u8* bin, size_t sz);
  // TODO:
  //~TmjMedia();

  TMJKeys lexify(const char* token, TMJKeys object);
  void prepareParser(json_stream *json, TMJKeys object);
  void parse(json_stream *json, TMJKeys object);
  void loadMap();
  void loadLayer(Layers ly);
  void unloadLayer(Layers ly);
  void unloadMap();
  std::shared_ptr<i32[]> getLayerData(Layers ly);
  i32 getWidth();
  i32 getHeight();
};

i32 base64_decode(const std::string *input, char *output, size_t output_len);
std::shared_ptr<i32[]> bytes_to_integers(const char* input, i32 input_len, i32* output_len);

#define ___include_tmj(filename, extension) \
  media_binary(filename, extension) \
  extern TmjMedia filename

#define ___define_tmj(filename, extension) \
  TmjMedia filename (#filename, media_start(filename, extension), media_size(filename, extension))

