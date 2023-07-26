#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <rope.h>
#include <stozer.h>



stozer::Stozer *STOZER;

int main(void) {
  // your setup ...
    stozer::Stozer &stz = stozer::Stozer::instance();
    STOZER = &stz;
  // your clean-up...

    stz.start();

    int result = Catch::Session().run();

    // while(!stz.shouldEnd){
    //      stz.update();

    // }

    stz.end();

    return result;
}