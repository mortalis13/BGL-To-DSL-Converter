
#ifndef BABYLON_H
#define BABYLON_H

#include <stdlib.h>
#include <zlib.h>

#include <string>
#include <vector>
#include <map>
#include <regex>

using namespace std;

#define CHARSETS_NUM 14

typedef unsigned int uint;

const string bgl_language[] = {
	"English", 
	"French",
	"Italian",
	"Spanish",
	"Dutch",
	"Portuguese",
	"German",
	"Russian",
	"Japanese",
	"Traditional Chinese",
	"Simplified Chinese",
	"Greek",
	"Korean",
	"Turkish",
	"Hebrew",
	"Arabic",
	"Thai",
	"Other",
	"Other Simplified Chinese dialects",
	"Other Traditional Chinese dialects",
	"Other Eastern-European languages",
	"Other Western-European languages",
	"Other Russian languages",
	"Other Japanese languages",
	"Other Baltic languages",
	"Other Greek languages",
	"Other Korean dialects",
	"Other Turkish dialects",
	"Other Thai dialects",
	"Polish",
	"Hungarian",
	"Czech",
	"Lithuanian",
	"Latvian",
	"Catalan",
	"Croatian",
	"Serbian",
	"Slovak",
	"Albanian",
	"Urdu",
	"Slovenian",
	"Estonian",
	"Bulgarian",
	"Danish",
	"Finnish",
	"Icelandic",
	"Norwegian",
	"Romanian",
	"Swedish",
	"Ukrainian",
	"Belarusian",
	"Farsi",
	"Basque",
	"Macedonian",
	"Afrikaans",
	"Faeroese",
	"Latin",
	"Esperanto",
	"Tamazight",
	"Armenian"};


const string bgl_charsetname[] = {
	"Default" ,
	"Latin",
	"Eastern European",
	"Cyrillic",
	"Japanese",
	"Traditional Chinese",
	"Simplified Chinese",
	"Baltic",
	"Greek",
	"Korean",
	"Turkish",
	"Hebrew",
	"Arabic",
	"Thai"};

const string bgl_charset[CHARSETS_NUM] = {
  // "ISO-8859-1", /*Default*/
  "CP1252", /*Default*/
	"ISO-8859-1", /*Latin*/
  // "CP1252", /*Latin*/
	"ISO-8859-2", /*Eastern European*/
	"ISO-8859-5", /*Cyriilic*/
	"ISO-8859-14", /*Japanese*/
	"ISO-8859-14", /*Traditional Chinese*/
	"ISO-8859-15", /*Simplified Chinese*/
	"CP1257", /*Baltic*/
	"CP1253", /*Greek*/
	"ISO-8859-15",  /*Korean*/
	"ISO-8859-9", /*Turkish*/
	"ISO-8859-9", /*Hebrew*/
	"CP1256", /*Arabic*/
	"CP874"  /*Thai*/};

static map<int, string> partOfSpeech = {
  {0x30, "noun"},
  {0x31, "adjective"},
  {0x32, "verb"},
  {0x33, "adverb"},
  {0x34, "interjection"},
  {0x35, "pronoun"},
  {0x36, "preposition"},
  {0x37, "conjunction"},
  {0x38, "suffix"},
  {0x39, "prefix"},
  {0x3A, "article"},
  {0x3B, ""},
  {0x3C, "abbreviation"},
  {0x3D, "masculine noun and adjective"},
  {0x3E, "feminine noun and adjective"},
  {0x3F, "masculine and feminine noun and adjective"},
  {0x40, "feminine noun"},
  {0x41, "masculine and feminine noun"},
  {0x42, "masculine noun"},
  {0x43, "numeral"},
  {0x44, "participle"},
  {0x45, ""},
  {0x46, ""},
  {0x47, ""},
};

typedef struct {
	unsigned type;
	unsigned length;
	char * data;
} bgl_block;

typedef struct {
        string headword;
        string definition;
        vector<string> alternates;
        bool end = false;
} bgl_entry;

class Babylon
{
public:
    Babylon(string);
    ~Babylon();

    bool open();
    void close();
    bool readBlock(bgl_block&);
    bool read();
    bgl_entry readEntry();

    inline string title() const {return m_title;};
    inline string author() const {return m_author;};
    inline string email() const {return m_email;};
    inline string description() const {return m_description;};
    inline string copyright() const {return m_copyright;};
    inline string sourceLang() const {return m_sourceLang;};
    inline string targetLang() const {return m_targetLang;};
    inline uint numEntries() const {return m_numEntries;};
    inline string defaultCharset() const {return m_defaultCharset;};
    inline string sourceCharset() const {return m_sourceCharset;};
    inline string targetCharset() const {return m_targetCharset;};

    inline string filename() const {return m_filename;};

private:
    unsigned int bgl_readnum(int);
    void convertToUtf8(string &, uint = 0);
    
    string m_filename;
    gzFile file;

    string m_title;
    string m_author;
    string m_email;
    string m_description;
    string m_copyright;
    string m_sourceLang;
    string m_targetLang;
    uint m_numEntries;
    string m_defaultCharset;
    string m_sourceCharset;
    string m_targetCharset;

    enum CHARSET {DEFAULT_CHARSET, SOURCE_CHARSET, TARGET_CHARSET};
};

#endif // BABYLON_H
