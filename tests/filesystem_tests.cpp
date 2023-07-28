#include <catch2/catch_test_macros.hpp>
#include <stozer.h>
#include <utils.h>
#include <raylib.h>

#include <string>


extern stozer::Stozer *STOZER;
const std::string SEPARATOR = "\\";

TEST_CASE( "Get working directory", "[stozer.getWorkingDirectory]" ) {
    
    SECTION("get current working directory"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( wdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(wdPath.c_str()) == true);
    }
}

TEST_CASE( "Get root directory", "[stozer.getRootDirectory]" ) {
    
    SECTION("get current working directory"){
        REQUIRE( STOZER != nullptr);
        std::string rdPath = STOZER->getRootDirectory();

        //just check if it exists
        REQUIRE( rdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(rdPath.c_str()) == true);
    }
}



/*
    creates:
        dir - working_directory/test
        dir - wd/test/123
*/
TEST_CASE( "Make dir", "[stozer.makeDirectory]" ) {
    
    SECTION("creating new dir at given path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string ndPath = wdPath+SEPARATOR+"test";
        //check that there is nothing there before calling makeDirectory
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == false);
        int8_t res = STOZER->makeDirectory("test");
        REQUIRE( res == 1 );
        //check if it has been made
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == true);
    }

    SECTION("creating new dir at given path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string ndPath = wdPath+SEPARATOR+"test/123";
        //check that there is nothing there before calling makeDirectory
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == false);
        int8_t res = STOZER->makeDirectory("test/.././test/123");
        REQUIRE( res == 1 );
        //check if it has been made
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == true);
    }

    SECTION("creating new dir at given path, unicode "){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string ndPath = wdPath+SEPARATOR+"test/šućur";
        //check that there is nothing there before calling makeDirectory
        REQUIRE( stozer::filesystem::is_dir(ndPath) == false);
        int8_t res = STOZER->makeDirectory("test/.././test/šućur");
        REQUIRE( res == 1 );
        //check if it has been made
        REQUIRE( stozer::filesystem::is_dir(ndPath) == true);
    }
}

TEST_CASE( "NEGATIVE Make dir", "[stozer.makeDirectory]" ) {

    SECTION("creating new dir at given path, invalid name"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        int8_t res = STOZER->makeDirectory("...");
        //check if error is returned
        REQUIRE( res == -2);
    }

    SECTION("creating new dir at given path, when there is dir there already"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string ndPath = wdPath+SEPARATOR+"test";
        //check that there is dir there before calling makeDirectory
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == true);
        int8_t res = STOZER->makeDirectory("test");
        //check if error is returned
        REQUIRE( res == 0);
        //check if it is still there
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == true);
    }

    SECTION("creating new dir at given path, when there is dir there already, unicode "){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string ndPath = wdPath+SEPARATOR+"test/šućur";
        //check that there is dir there before calling makeDirectory
        REQUIRE( stozer::filesystem::is_dir(ndPath) == true);
        int8_t res = STOZER->makeDirectory("test");
        //check if error is returned
        REQUIRE( res == 0);
        //check if it is still there
        REQUIRE( stozer::filesystem::is_dir(ndPath) == true);
    }

    SECTION("creating new dir at given path, without permission"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string nd = ".."+SEPARATOR;
        std::string ndPath = wdPath;
        REQUIRE( stozer::filesystem::move_path(ndPath, nd, STOZER->getRootDirectory()) == true );
        ndPath +=  SEPARATOR + "test";
        nd += "test";
        //check that there is nothing there before calling makeDirectory
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == false);
        int8_t res = STOZER->makeDirectory(nd);
        //check if error is returned
        REQUIRE( res == -1);
        //check if it is still nothing there
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == false);
    }

}


std::string owdPath;//original workingDirectory, starts from here and ends in here
TEST_CASE( "Change working directory", "[stozer.changetWorkingDirectory]" ) {
    SECTION("change current working directory"){
        REQUIRE( STOZER != nullptr);
        owdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( owdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(owdPath.c_str()) == true);
    
        //go to owd/test
        REQUIRE( STOZER->changeWorkingDirectory("test") == true);
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath.c_str()) == true);
        REQUIRE( nwdPath == owdPath + SEPARATOR + "test" );
    }

    SECTION("change current working directory, go back"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( wdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(wdPath.c_str()) == true);
    
        //go to owd
        REQUIRE( STOZER->changeWorkingDirectory("../") == true);
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath.c_str()) == true);
        REQUIRE( owdPath.empty() == false);
        REQUIRE( nwdPath == owdPath );
    }

    SECTION("change current working directory, unicode "){
        REQUIRE( STOZER != nullptr);
        owdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( owdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(owdPath.c_str()) == true);
    
        //go to owd/test
        REQUIRE( STOZER->changeWorkingDirectory("test/šućur") == true);
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath) == true);
        REQUIRE( nwdPath == owdPath + SEPARATOR + "test" + SEPARATOR + "šućur" );
    }

    SECTION("change current working directory, go back"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( wdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(wdPath) == true);
    
        //go to owd
        REQUIRE( STOZER->changeWorkingDirectory("../../") == true);
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath) == true);
        REQUIRE( owdPath.empty() == false);
        REQUIRE( nwdPath == owdPath );
    }

    SECTION("change current working directory, stay"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( wdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(wdPath.c_str()) == true);
    
        //stay
        REQUIRE( STOZER->changeWorkingDirectory("./") == true );
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath.c_str()) == true);
        REQUIRE( nwdPath == wdPath );
    }

    SECTION("change current working directory, back and forth"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( wdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(wdPath.c_str()) == true);
    
        //stay
        REQUIRE( STOZER->changeWorkingDirectory("test/../test/../test/../test/./../test/../test/././././././../test/..") == true);
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath.c_str()) == true);
        REQUIRE( nwdPath == wdPath );
    }

    SECTION("change current working directory, / path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( wdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(wdPath.c_str()) == true);
    
        //stay
        std::string rPath = stozer::filesystem::relative_path( STOZER->getRootDirectory(), wdPath );
        REQUIRE( STOZER->changeWorkingDirectory(rPath + SEPARATOR + "test/../test/../test/../test/./../test/../test/././././././../test/..") == true);
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath.c_str()) == true);
        REQUIRE( nwdPath == wdPath );
    }

}

TEST_CASE( "NEGATIVE Change working directory", "[stozer.changetWorkingDirectory]" ) {
    SECTION("change current working directory, invalid path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( wdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(wdPath.c_str()) == true);
    
        //go to owd/test
        REQUIRE( STOZER->changeWorkingDirectory("tes") == false );
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath.c_str()) == true);
        REQUIRE( nwdPath == wdPath );
    }

    SECTION("change current working directory, invalid path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( wdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(wdPath.c_str()) == true);
    
        //go to owd/test
        REQUIRE( STOZER->changeWorkingDirectory("test/...") == false );
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath.c_str()) == true);
        REQUIRE( nwdPath == wdPath );
    }

    SECTION("change current working directory, invalid path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();

        //just check if it exists
        REQUIRE( wdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(wdPath.c_str()) == true);
    
        //go to owd/test
        REQUIRE( STOZER->changeWorkingDirectory("../test") == false );
        std::string nwdPath = STOZER->getWorkingDirectory();
        REQUIRE( nwdPath.empty() == false );
        REQUIRE( stozer::filesystem::is_dir(nwdPath.c_str()) == true);
        REQUIRE( nwdPath == wdPath );
    }

}

/*
    creates:
        file - working_directory/test/test_file
        file - wd/test/123/test_file
*/
TEST_CASE( "Make file", "[stozer.makeFile]" ) {
    
    SECTION("creating new file at given path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string pdPath = wdPath+SEPARATOR+"test";
        std::string fPath = wdPath+SEPARATOR+"test"+SEPARATOR+"test_file.txt";
        //check that parent dir exiss
        REQUIRE( stozer::filesystem::is_dir(pdPath.c_str()) == true);
        //check that there is nothing on file path
        REQUIRE( FileExists(fPath.c_str()) == false );
        int8_t res = STOZER->makeFile("test" + SEPARATOR + "test_file");
        REQUIRE( res == 1 );
        //check if it has been made
        REQUIRE( FileExists(fPath.c_str()) == true);
    }

    SECTION("creating new file at given path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string pdPath = wdPath+SEPARATOR+"test"+SEPARATOR+"123";
        std::string fPath = wdPath+SEPARATOR+"test"+SEPARATOR+"123"+SEPARATOR+"test_file.txt";
        //check that parent dir exiss
        REQUIRE( stozer::filesystem::is_dir(pdPath.c_str()) == true);
        //check that there is nothing on file path
        REQUIRE( FileExists(fPath.c_str()) == false );
        int8_t res = STOZER->makeFile("test" + SEPARATOR + "123" + SEPARATOR + "test_file");
        REQUIRE( res == 1 );
        //check if it has been made
        REQUIRE( FileExists(fPath.c_str()) == true);
    }

    SECTION("creating new file at given path, unicode"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string pdPath = wdPath+SEPARATOR+"test"+SEPARATOR+"šućur";
        std::string fPath = wdPath+SEPARATOR+"test"+SEPARATOR+"šućur"+SEPARATOR+"žđ.txt";
        //check that parent dir exiss
        REQUIRE( stozer::filesystem::is_dir(pdPath.c_str()) == true);
        //check that there is nothing on file path
        REQUIRE( FileExists(fPath.c_str()) == false );
        int8_t res = STOZER->makeFile("test" + SEPARATOR + "šućur" + SEPARATOR + "žđ");
        REQUIRE( res == 1 );
        //check if it has been made
        REQUIRE( stozer::filesystem::is_valid_path(fPath) == true);
    }
}

TEST_CASE( "NEGATIVE Make file", "[stozer.makeFile]" ) {

    SECTION("creating new file at given path, invalid name"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        int8_t res = STOZER->makeFile("...");
        //check if error is returned
        REQUIRE( res == -2);
    }

    SECTION("creating new file at given path, when there is file there already"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string pdPath = wdPath+SEPARATOR+"test";
        std::string fPath = wdPath+SEPARATOR+"test"+SEPARATOR+"test_file.txt";
        //check that there is file there before calling makeFile
        REQUIRE( FileExists( fPath.c_str()) == true);
        int8_t res = STOZER->makeFile("test" + SEPARATOR + "test_file");
        //check if error is returned
        REQUIRE( res == 0);
        //check if it is still there
        REQUIRE( FileExists(fPath.c_str()) == true);
    }

    SECTION("creating new file at given path, without permission"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string f = ".."+SEPARATOR+".."+SEPARATOR;
        std::string fPath = wdPath;
        REQUIRE( stozer::filesystem::move_path(fPath, f, STOZER->getRootDirectory()) == true );
        fPath += SEPARATOR + "test_file.txt";
        f += "test_file";
        //check that there is nothing there before calling makeFile
        REQUIRE( FileExists(fPath.c_str()) == false);
        int8_t res = STOZER->makeFile(f);
        //check if error is returned
        REQUIRE( res == -1);
        //check if it is still nothing there
        REQUIRE( FileExists(fPath.c_str()) == false);
    }

}



TEST_CASE( "NEGATIVE Remove dir", "[stozer.removeFileOrDir]" ) {
    
    SECTION("removing dir at given path, non empty"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+"test";
        //check that there is dir there before calling removeDirectory
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
        REQUIRE( STOZER->removeFileOrDir("test", false) == -2);
        //check if it has been removed
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
    }

    SECTION("removing dir at given path, invalid path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+"tst";
        //check that there is no  dir there before calling removeDirectory
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == false);
        REQUIRE( STOZER->removeFileOrDir("tst", false) == 0);
        //check if it has been removed
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == false);
    }

    SECTION("removing dir at given path, no permission"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+".."+SEPARATOR+"..";
        //check that there is dir there before calling removeDirectory
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
        REQUIRE( STOZER->removeFileOrDir(".."+SEPARATOR+"..", false) == -1);
        //check if it has been removed
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
    }
}

TEST_CASE( "Move dir", "[stozer.moveFileOrDirectory]" ) {
    
    SECTION("move directory"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+"test";
        std::string ndPath = wdPath+SEPARATOR+"ntest";
        //check that there is dir there before calling moveDirectory
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
        int8_t res = STOZER->moveFileOrDir("test", "ntest");
        REQUIRE( res == 1 );
        //check if it has been moved
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == false);
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == true);
    }

    SECTION("move directory"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+"ntest";
        std::string ndPath = wdPath+SEPARATOR+"test";
        //check that there is dir there before calling moveDirectory
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
        int8_t res = STOZER->moveFileOrDir("ntest", "ntest/../test");
        REQUIRE( res == 1 );
        //check if it has been moved
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == false);
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == true);
    }

    SECTION("move directory, unicode "){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+"test"+SEPARATOR+"šućur";
        std::string ndPath = wdPath+SEPARATOR+"test"+SEPARATOR+"šućurđž";
        //check that there is dir there before calling moveDirectory
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
        int8_t res = STOZER->moveFileOrDir("test/šućur", "test/šućurđž");
        REQUIRE( res == 1 );
        //check if it has been moved
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == false);
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == true);
    }
}

TEST_CASE( "NEGATIVE Move dir", "[stozer.moveFileOrDirectory]" ) {

    SECTION("move directory, invalid name"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        int8_t res = STOZER->moveFileOrDir("test", "...");
        //check if error is returned
        REQUIRE( res == -2);
    }

    SECTION("move directory, when there is dir there already"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+"test";
        std::string ndPath = wdPath+SEPARATOR+"test"+SEPARATOR+"123";
        //check that there is dir there before calling move
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == true);
        int8_t res = STOZER->moveFileOrDir("test", "test"+SEPARATOR+"123");
        //check if error is returned
        REQUIRE( res == 0);
        //check if it is still there
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == true);
    }

    SECTION("move directory, without permission"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string nd = ".."+SEPARATOR;
        std::string ndPath = wdPath;
        REQUIRE( stozer::filesystem::move_path(ndPath, nd, STOZER->getRootDirectory()) == true );
        ndPath +=  SEPARATOR + "ntest";
        nd += "ntest";
        //check that there is nothing there before calling makeDirectory
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == false);
        int8_t res = STOZER->moveFileOrDir("test", nd);
        //check if error is returned
        REQUIRE( res == -1);
        //check if it is still nothing there
        REQUIRE( stozer::filesystem::is_dir(ndPath.c_str()) == false);
    }

}



/*
    removes:
        file - working_directory/test/123/test_file

*/
TEST_CASE( "Remove file", "[stozer.removeFileOrDir]" ) {
    SECTION("removing file at given path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string fPath = wdPath+SEPARATOR+"test"+SEPARATOR+"123"+SEPARATOR+"test_file.txt";
        //check that there is file there before calling removeFile
        REQUIRE( FileExists(fPath.c_str()) == true);
        REQUIRE( STOZER->removeFileOrDir("test"+SEPARATOR+"123"+SEPARATOR+"test_file", false) == 1);
        //check if it has been removed
        REQUIRE( FileExists(fPath.c_str()) == false);
    }
}

/*
    removes:
        dir - working_directory/test
        dir - working_directory/test/test_file

*/
TEST_CASE( "Remove dir", "[stozer.removeFileOrDir]" ) {
    SECTION("removing dir at given path"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+"test"+SEPARATOR+"123";
        //check that there is dir there before calling removeDirectory
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
        REQUIRE( STOZER->removeFileOrDir("test"+SEPARATOR+"123", false) == 1);
        //check if it has been removed
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == false);
    }

    SECTION("removing dir at given path, unicode with force"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+"test"+SEPARATOR+"šućurđž";
        //check that there is dir there before calling removeDirectory
        REQUIRE( stozer::filesystem::is_dir(dPath) == true);
        REQUIRE( STOZER->removeFileOrDir("test/šućurđž", true) == 1);
        //check if it has been removed
        REQUIRE( stozer::filesystem::is_dir(dPath) == false);
    }

    SECTION("removing dir at given path, with force"){
        REQUIRE( STOZER != nullptr);
        std::string wdPath = STOZER->getWorkingDirectory();
        std::string dPath = wdPath+SEPARATOR+"test";
        //check that there is dir there before calling removeDirectory
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == true);
        REQUIRE( STOZER->removeFileOrDir("test", true) == 1);
        //check if it has been removed
        REQUIRE( stozer::filesystem::is_dir(dPath.c_str()) == false);
    }
}