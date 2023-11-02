#include <format>

#include "assets/ogg_files.hpp"
#include "assets/png_files.hpp"
#include "assets/tmj_files.hpp"
#include "sylphid.hpp"

// TODO: const'ify every member method if not altering "this" pointer

i32 main(void) {
  WindowManager wm(600, 600, L"Serokuchi");
  wm.createWindow(1, 600, 600, L"Layer 0");
  wm.createWindow(2, 600, 600, L"Layer 0.5");
  wm.createWindow(3, 600, 600, L"Layer 1");

  thread_local unsigned constexpr long static long const int * const 何 = {nullptr};

  test_music.loadBGM();

  Sprite test_spr(&aristotelio_portrait_black);

  wm[L"Layer 1"]->c(0xFF0000FF);
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
    if (wm('Q').isPressed){
      wm[L"Layer 0.5"]->setWindowSize(600, 600);
    }
    if (wm('W').isPressed){
      wm[L"Layer 0.5"]->setWindowSize(800, 800);
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
        wm[L"Layer 1"]->d(i, j, 0);
      }
    }

    wm[L"Layer 0"]->c(0xFFFFFF00);
    wm[L"Layer 0"]->drawSprite(&test_spr);
    wm[L"Layer 0"]->drawText(font8x8, 0, 30, "FPS: " + std::to_string(wm.getFPS()), 0xFFFF0000, 8, -2, 10);
  }

  return 0;
}
