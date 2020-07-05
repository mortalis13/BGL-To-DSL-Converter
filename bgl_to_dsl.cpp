#include <stdio.h>

#include <iostream>
#include <string>

#include "babylonreader.h"
#include "utils.h"

using namespace std;

#define VERSION "1.0.0"

void convert_bgl_to_dsl(char* dict_path) {
  string filename(dict_path);
  string outFile = Utils::replace_string(filename, "\\.(bgl|BGL)$", "") + ".dsl";
  
  cout << "-- Input file: " << filename << endl;
  cout << "-- Output file: " << outFile << endl << endl;
  
  BabylonReader* reader = new BabylonReader(filename, outFile);
  reader->convert();
}

int main(int argc, char* argv []) {
  cout << "BGL to DSL converter v" << VERSION << endl << endl;
  
  if (argc != 2) {
    cout << "Usage: [EXE] [DIST_PATH].bgl\n";
  }
  else {
    convert_bgl_to_dsl(argv[1]);
  }
  
  return 0;
}
