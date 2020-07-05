#include "babylonreader.h"

#include <stdio.h>
#include <fstream>

#include "babylon.h"
#include "utils.h"


BabylonReader::BabylonReader(string filename, string outFile) {
  m_babylon = new Babylon(filename);
  m_outFile = outFile;
  m_entriescount = 0;
}

bool BabylonReader::addHeadword(string word, string def, vector<string> alternates) {
  m_entriescount++;
  struct entry entry;
  string definition = def;
  entry.position = m_definition.length();
  entry.size = definition.length();
  string headword;

  headword = word;
  dic.insert(make_pair(headword, entry));
}


bool BabylonReader::convert() {
  // --- Read
  bool res = m_babylon->open();
  if(!res) {
    printf("Error openning %s\n", m_babylon->filename().c_str());
    return false;
  }
  
  res = m_babylon->read();
  
  printf("numEntries: %d\n", m_babylon->numEntries());
  printf("title: %s\n", m_babylon->title().c_str());
  printf("sourceLang: %s\n", m_babylon->sourceLang().c_str());
  printf("targetLang: %s\n", m_babylon->targetLang().c_str());
  printf("defaultCharset: %s\n", m_babylon->defaultCharset().c_str());
  printf("sourceCharset: %s\n", m_babylon->sourceCharset().c_str());
  printf("targetCharset: %s\n", m_babylon->targetCharset().c_str());
  
  
  // --- Process
  fstream f(m_outFile, ios::out);
  if (!f.is_open()) {
    printf("File open ERROR\n");
    return false;
  }
  
  f << "#NAME \"" << m_babylon->title() << "\"" << endl;
  f << "#INDEX_LANGUAGE \"" << m_babylon->sourceLang() << "\"" << endl;
  f << "#CONTENTS_LANGUAGE \"" << m_babylon->targetLang() << "\"" << endl << endl;
  
  bgl_entry entry;
  
  while (true) {
    entry = m_babylon->readEntry();
    if (entry.end) break;
    if (entry.headword.length() == 0 || entry.definition.length() == 0) continue;
    
    string alts = "";
    for (auto alt: entry.alternates) {
      alts += alt + "\n";
    }
    
    f << entry.headword << "\n";
    f << alts;
    f << entry.definition << "\n\n";
  }
  
  f.close();
  m_babylon->close();
  
  return true;
}
