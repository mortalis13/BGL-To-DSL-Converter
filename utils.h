#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <regex>

using namespace std;


void log(const string& msg, ...);
void logd(const string& msg, ...);


namespace Utils {
  
  static vector<pair<string, string>> replace_map = {
    {"<br.*?>", "\n"},
    {"</p>", "\n"},
    {"</div>", "\n"},
    // {"\\\\n", "\n"},
    
    {"<(/?b).*?>", "[$1]"},
    {"<(/?i).*?>", "[$1]"},
    {"<(/?u).*?>", "[$1]"},
    {"<(/?sub).*?>", "[$1]"},
    {"<(/?sup).*?>", "[$1]"},
    {"<(/?ex).*?>", "[$1]"},
    
    {"<(/?)abr.*?>", "[$1p]"},
    {"<(/?)a.*?>", "[$1ref]"},
    {"<(/?)kref.*?>", "[$1ref]"},
    {"<(/?)iref.*?>", "[$1url]"},
    
    {"<c(( ?)c=['\"](.+?)['\"])?>", "[c$2$3]"},
    
    {"<rref>.+?</rref>", ""},
    {"<!--.*?-->", ""},
  };

  static string rx_remove_tag = "<(/?).*?>";
  
  
  static vector<string> tags_literal = {
    "b",
    "i",
    "u",
    "sub",
    "sup",
    "ex"
  };

  static vector<pair<string, string>> tags_change = {
    {"abr", "p"},
    {"a", "ref"},
    {"kref", "ref"},
    {"iref", "url"},
  };

  static vector< pair<string, pair<string, string>> > tags_replace = {
    {"c", {"<c(( ?)c=['\"](.+?)['\"])?>", "[c$2$3]"}},
  };
  
  
  // -----------------------------------------------
  string readFile(string filename);
  
  string trim(const string& str);
  string replace_string(const string& str, const string& from_rx, const string& replace);
  vector<string> split_string(const string& str, const string& delimiter);
  size_t codePointToUtf(unsigned long cp, char* out);
  
  string getTagName(const string& text);
  string getTagAttrs(const string& text);
  map<string, string> parseTagAttrs(const string& attrs);
  bool findTag(const string& text, const string& tag, bool openTag);
  
  // -------------- BGL ---------------
  bool isControlChar(char ch);
  
  string stripDollar(const string& word);
  string stripSlash(const string& word);
  
  string convertHtmlEntities(const string& inText);
  string decodeCharsetTags(const string& inText);
  string html_to_dsl(const string& inText);
  
  string stripDollarIndexes(const string& word);
  
}

#endif