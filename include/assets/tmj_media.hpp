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
    std::string compression; // Uncompressed, zlib or Zstandard (Only zlib supported for now)
    std::string encoding; // CSV or Base64 (Only Base64 supported for now)
    std::string type; // Only tilelayer supported for now
    // u32 tintColor; // Of type #xxxxxxxx, not yet supported
    // f32 opacity;
    std::string name;
    std::string encodedData;
    std::shared_ptr<i32[]> data; // Base64-zlib converted to i32 array
};

struct TileSet{
    i32 firstGid;
    i32 columns;
    i32 tileCount;
    i32 tileWidth, tileHeight;
    std::string name;
    std::string image;
    i32 imageWidth, imageHeight;
    i32 margin, spacing;
};

class TmjMedia : private Media{
private:
  // NOTE: size == len
  bool initialized;
  i32 width, height;
  bool infinite;
  i32 compressionLevel; // Hopefully always -1
  std::string orientation; // Orthogonal, Isometric, Isometric (Staggered), Hexagonal (Staggered)
  std::string renderOrder; // Right Down, Right Up, Left Down, Left Up,
  std::string tiledVersion; // Made for Tiled version 1.10.1
  std::string version; // Made for version 1.10
  i32 tileWidth, tileHeight;
  std::string type; // Only map is supported for now
  i32 nextLayerId, nextObjectId;
  
  std::vector<Layer> layers;
  std::map<std::string, Layer> layersMap;
  std::vector<TileSet> tilesets;
  std::map<std::string, TileSet> tilesetsMap;

public:
  TmjMedia(std::string file, u8* bin, size_t sz);

  TMJKeys lexify(const char* token, TMJKeys object);
  void parse(json_stream *json, TMJKeys object);
  void loadLayer(std::string ly);
  void unloadLayer(std::string ly);
  void unloadLayers();
  i32 getLayerData(std::string ly, i32 index);
  i32 getWidth();
  i32 getHeight();
};

i32 base64_decode(const std::string *input, char *output, size_t output_len);

#define ___include_tmj(filename, extension) \
  media_binary(filename, extension) \
  extern TmjMedia filename

#define ___define_tmj(filename, extension) \
  TmjMedia filename (#filename, media_start(filename, extension), media_size(filename, extension))

