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
 * 6 - Cursor
 * 5 - Menu text/icons/portraits
 * 4 - Cursor selection
 * 3 - Menu frames/etc
 * 2 - Characters sprites
 * 1 - Selection
 * 0 - Map/Background tiles (can have animation)
 *
*/

i32 main(void) {
  // 1920x1080 (1) 480x270 (4)
  // 1600x900  (1) 400x225 (4)
  // 960x540   (1) 240x135 (4)
  //WindowManager wm(L"Serokuchi", 320, 180, 3);
  WindowManager wm(L"Serokuchi", 960, 540);
  wm.createWindow(L"Layer 6", 8, 480, 270, 2);
  wm.createWindow(L"Layer 5", 7, 480, 270, 2);
  wm.createWindow(L"Layer 4", 6, 480, 270, 2);
  wm.createWindow(L"Layer 3", 5, 480, 270, 2);
  wm.createWindow(L"Layer 2.5", 4, 480, 270, 2);
  wm.createWindow(L"Layer 2", 3, 320, 180, 3);
  wm.createWindow(L"Layer 1", 2, 480, 270, 2);
  wm.createWindow(L"Layer 0", 1, 480, 270, 2);

  thread_local unsigned constexpr long static long const int * const 何 = {nullptr};

  test_music.loadBGM();

  Sprite test_spr(&aristotelio_portrait_black);
  Sprite test_spr2(&aristotelio_map);
  Sprite test_spr2_5(&aristotelio_map);
  Sprite test_spr3(&overworld);
  Sprite test_spr4(&aristotelio_reduced_portrait_black);
  test_spr.x = 40;
  test_spr.y = 40;
  test_spr2.x = 296;
  test_spr2.y = 40;
  test_spr2_5.x = 456;
  test_spr2_5.y = 30;
  test_spr4.x = 386;
  test_spr4.y = 140;

  //wm[L"Layer 1"]->c(0);
  wm[L"Layer 0"]->dSprite(&test_spr3);
  wm[L"Layer 2"]->dSprite(&test_spr2);
  wm[L"Layer 2.5"]->dSprite(&test_spr2_5);

  wm[L"Layer 1"]->dSquare(24, 50, 180, ARGB(1, 0, 32, 0), true);
  wm[L"Layer 1"]->dSquare(24, 80, 180, ARGB(1, 0, 64, 0), true);
  wm[L"Layer 1"]->dSquare(24, 110, 180, ARGB(1, 0, 96, 0), true);

  wm[L"Layer 1"]->dSquare(24, 200, 30, ARGB(1, 32, 0, 0), true);
  wm[L"Layer 1"]->dSquare(24, 230, 30, ARGB(1, 64, 0, 0), true);
  wm[L"Layer 1"]->dSquare(24, 260, 30, ARGB(1, 96, 0, 0), true);

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

    wm[L"Layer 5"]->c(0);
    wm[L"Layer 5"]->dSprite(&test_spr);
    wm[L"Layer 5"]->dSprite(&test_spr4);
    wm[L"Layer 5"]->drawText(font8x8, 0, 30, "FPS: " + std::to_string(wm.getFPS()), 0xFFFF0000, 2, -2, 10);
    wm[L"Layer 5"]->drawText(font8x8, 0, 10, "We had a lot of fun growing up", 0xFFFF0000, 2, -2, 10);
  }

  return 0;
}
