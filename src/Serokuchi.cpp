#include <format>

#include "assets/ogg_files.hpp"
#include "assets/png_files.hpp"
#include "assets/tmj_files.hpp"
#include "sylphid.hpp"

// TODO: const'ify every member method if not altering "this" pointer

// NOTE:
// Portrait sprites should be in 2x scale
// Fonts should be in ? scale
// Map sprites should be in 3x scale

/* NOTE: about layers (in maps):
 *
 * 
 *
 *
 * 2 - Characters sprites
 * 1 - 
 * 0 - Map/Background tiles (can have animation)
 *
*/

i32 main(void) {
  // 1920x1080 (1) 480x270 (4)
  // 1600x900  (1) 400x225 (4)
  // 960x540   (1) 240x135 (4)
  //WindowManager wm(L"Serokuchi", 320, 180, 3);
  WindowManager wm(L"Serokuchi", 480, 270, 2);
  wm.createWindow(L"Layer 0", 1);
  wm.createWindow(L"Layer 0.5", 2);
  wm.createWindow(L"Layer 1", 3);

  thread_local unsigned constexpr long static long const int * const 何 = {nullptr};

  test_music.loadBGM();

  Sprite test_spr(&aristotelio_portrait_black);
  Sprite test_spr2(&aristotelio_map);
  test_spr.x = 40;
  test_spr.y = 40;
  test_spr2.x = 240;
  test_spr2.y = 40;

  wm[L"Layer 1"]->c(0);
  while(wm.shouldRun()){
    if (wm('F').isPressed){
      map_test.loadMap();
      auto test = map_test.getLayerData(FLOOR);
      i32 w = map_test.getWidth(), h = map_test.getHeight();
      for(i32 i = 0; i <  w * h; i++){
        if(i % w == 0)
          std::cout << "\n";
        std::cout << std::format("{:03}", test[i]) << " ";
      }
      std::cout << "\n";
    }
    if (wm('E').isPressed){
      test_music.toggle();
      //test_sfx_1.play();
    }
    if (wm('P').isPressed){
      break;
    }
    if (wm('R').isPressed){
      wm[L"Layer 0.5"]->c(0);
      //wm.removeWindow("Layer 0.5");
    }
    if (wm('T').isPressed){
      wm[L"Layer 0.5"]->c(0xFF00FF00);
    }

    for(i32 i = 40; i < 560; i++){
      for(i32 j = 20; j < 200; j++){
        wm[L"Layer 1"]->d(i, j, 0, true);
      }
    }

    wm[L"Layer 0"]->c(0);
    wm[L"Layer 0"]->drawSprite(&test_spr);
    wm[L"Layer 0"]->drawSprite(&test_spr2);
    wm[L"Layer 0"]->drawText(font8x8, 0, 30, "FPS: " + std::to_string(wm.getFPS()), 0xFFFF0000, 2, -2, 10);
    wm[L"Layer 0"]->drawText(font8x8, 0, 10, "We had a lot of fun growing up", 0xFFFF0000, 2, -2, 10);
  }

  return 0;
}
