#include <stozer.h>
#include <krsh.h>
#include <sat.h>
#include <kalendar.h>
#include <gde.h>
#include <idi.h>
#include <napravi.h>
#include <listaj.h>
#include <ukloni.h>
#include <pomeri.h>
#include <kopiraj.h>
#include <uredi.h>
#include <izvrsi.h>
#include <posta.h>
#include <fakture.h>

using namespace stozer;

//start here
int main(void){
    Stozer &stz = Stozer::instance();

    stz.start();

    //load programs
    stz.processLoad(std::make_unique<Krsh>(stz, ""));
    stz.processLoad(std::make_unique<Sat>(stz, ""));
    stz.processLoad(std::make_unique<Kalendar>(stz, ""));
    stz.processLoad(std::make_unique<Gde>(stz, ""));
    stz.processLoad(std::make_unique<Idi>(stz, ""));
    stz.processLoad(std::make_unique<Napravi>(stz, ""));
    stz.processLoad(std::make_unique<Listaj>(stz, ""));
    stz.processLoad(std::make_unique<Ukloni>(stz, ""));
    stz.processLoad(std::make_unique<Pomeri>(stz, ""));
    stz.processLoad(std::make_unique<Kopiraj>(stz, ""));
    stz.processLoad(std::make_unique<Uredi>(stz, ""));
    stz.processLoad(std::make_unique<Izvrsi>(stz, ""));
    stz.processLoad(std::make_unique<Posta>(stz, ""));
    stz.processLoad(std::make_unique<Fakture>(stz, ""));


    //run krsh
    stz.processRun("krsh", "", stz.logStream);



    while(!stz.shouldEnd){
         stz.update();
    }

    stz.end();



    return 0;
}