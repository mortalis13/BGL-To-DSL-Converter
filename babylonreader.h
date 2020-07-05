
#ifndef BABYLONREADER_H
#define BABYLONREADER_H

#include "babylon.h"

#include <string>
#include <map>

using namespace std;

class BabylonReader
{
  public:
    BabylonReader(string filename, string outFile);
    bool convert();
    inline string filename() const {return m_babylon->filename();};
  
  private:
    bool addHeadword(string word, string def, vector<string> alternates = vector<string>());
    
  protected:
    struct entry {
      unsigned long position;
      unsigned long size;
    };

    typedef unsigned int uint32;
    typedef map<string, entry> dictionary;
    
    Babylon *m_babylon;
    
    string m_outFile;
    
    uint32 m_entriescount;
    string m_definition;
    dictionary dic;
};

#endif // BABYLONREADER_H
