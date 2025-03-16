#include "utils.h"

#include <stdarg.h>

#include <iconv.h>


// string logLevel = "DEBUG";
string logLevel = "INFO";

void log(const string& msg, ...) {
  va_list ap;
  va_start(ap, msg);
  string format = msg + "\n";
  vfprintf(stdout, format.c_str(), ap);
  fflush(stdout);
  va_end(ap);
}

void logd(const string& msg, ...) {
  if (logLevel == "INFO") return;
  va_list ap;
  va_start(ap, msg);
  string format = msg + "\n";
  vfprintf(stdout, format.c_str(), ap);
  fflush(stdout);
  va_end(ap);
}


namespace Utils {
  
  string readFile(string filename) {
    string result = "";
    
    fstream f(filename, ios::in | ios::ate);
    if (f.is_open()) {
      // cout << "File opened" << endl;
      size_t size = f.tellg();
      char buf[size];
      f.seekg(0, ios::beg);
      f.read(buf, size);
      f.close();
      result = string(buf, size);
    }
    else {
      cout << "Error opening file: " << filename << endl;
    }
    
    return result;
  }

  string trim(const string& str) {
    string chars = "\t\n\v\f\r ";
    size_t first = str.find_first_not_of(chars);
    if (string::npos == first) return str;
    size_t last = str.find_last_not_of(chars);
    return str.substr(first, (last - first + 1));
  }
  
  string replace_string(const string& str, const string& from_rx, const string& replace) {
    regex rx(from_rx);
    string res = regex_replace(str, rx, replace);
    return res;
  }
  
  vector<string> split_string(const string& str, const string& delimiter) {
    vector<string> strings;
    
    string::size_type pos = 0;
    string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != string::npos) {
      strings.push_back(str.substr(prev, pos - prev));
      prev = pos + 1;
    }
    
    if (prev < str.size()-1) strings.push_back(str.substr(prev));
    return strings;
  }

  size_t codePointToUtf(unsigned long cp, char* out) {
    unsigned char* _out = (unsigned char*) out;
    
    if (cp < 0x80) {
      *_out++ = (unsigned char) cp;
    }
    else if (cp < 0x800) {
      *_out++ = 0xC0 | (cp >> 6);
      *_out++ = 0x80 | (cp & 0x3F);
    }
    else if (cp < 0x10000) {
      *_out++ = 0xE0 | (cp >> 12);
      *_out++ = 0x80 | ((cp >> 6) & 0x3F);
      *_out++ = 0x80 | (cp & 0x3F);
    }
    else {
      *_out++ = 0xF0 | (cp >> 18);
      *_out++ = 0x80 | ((cp >> 12) & 0x3F);
      *_out++ = 0x80 | ((cp >> 6) & 0x3F);
      *_out++ = 0x80 | (cp & 0x3F);
    }
    
    return _out - (unsigned char*) out;
  }
  
  
  string getTagName(const string& text) {
    string tagName = "";
    size_t tagNameStart = text.find_first_not_of("< /");
    size_t tagNameEnd = text.find_first_of(" >", tagNameStart);
    
    if (tagNameStart != string::npos) {
      tagName = text.substr(tagNameStart, tagNameEnd - tagNameStart);
    }
    
    for (int i = 0; i < tagName.size(); i++){
      tagName[i] = tolower(tagName[i]);
    }
    
    return tagName;
  }

  string getTagAttrs(const string& text) {
    string tagAttrs = "";
    
    size_t tagNameStart = text.find_first_not_of("< /");
    size_t tagNameEnd = text.find_first_of(" >", tagNameStart);
    
    size_t tagAttrsStart = text.find_first_not_of(" >", tagNameEnd);
    size_t tagAttrsEnd = text.find_first_of(">", tagAttrsStart);
    
    if (tagAttrsStart != string::npos) {
      tagAttrs = text.substr(tagAttrsStart, tagAttrsEnd - tagAttrsStart);
    }
    
    return tagAttrs;
  }

  map<string, string> parseTagAttrs(const string& attrs) {
    map<string, string> attrsMap;
    
    string key = "";
    string value = "";
    bool keyProcess = true;
    bool valueProcess = false;
    bool quoteOpened = false;
    
    int len = attrs.size();
    for (int i = 0; i < len; ++i) {
      char c = attrs[i];
      
      if (c == '=') {
        keyProcess = false;
        valueProcess = true;
      }
      else if (c == ' ' && !quoteOpened || i == len-1) {
        if (i == len-1 && (!quoteOpened || c != '"' && c != '\'')) value.push_back(c);
        
        if (!keyProcess) {
          attrsMap[key] = value;
          key.clear();
          value.clear();
        }
        
        keyProcess = true;
        valueProcess = false;
      }
      else if (keyProcess) {
        key.push_back(c);
      }
      else if (valueProcess) {
        if (c == '"' || c == '\'') {
          quoteOpened = (value.size() == 0);
        }
        else {
          value.push_back(c);
        }
      }
    }
    
    return attrsMap;
  }
  
  bool findTag(const string& text, const string& tag, bool openTag) {
    size_t start_pos = 1;
    if (!openTag) start_pos = 2;
    
    bool result = false;
    if (start_pos < text.size() && text.substr(start_pos, tag.size()) == tag) {
      if (start_pos + tag.size() < text.size()) {
        string last_char = text.substr(start_pos+tag.size(), 1);
        result = (last_char == " " || last_char == ">");
      }
    }
    
    return result;
  }
  
  
  // =============== BGL ================
  
  bool isControlChar(char ch) {
    unsigned char uch = (unsigned char) ch;
    // if (uch == 0x1e || uch == 0x1f) return true;     // image links
    if (uch >= 0x00 && uch <= 0x08) return true;
    if (uch >= 0x0e && uch <= 0x1f) return true;
    if (uch == 0x0c) return true;
    return false;
  }

  string stripDollar(const string& word) {
    if (word.find("$") == string::npos || word == "$") return word;
    string result = "";
    bool d_start = false;
    int d0 = -1;
    
    int len = word.size();
    for (int i = 0; i < len; i++) {
      if (word[i] == '$') {
        if (!d_start) {
          d_start = true;
          // -- close the group immidiately if the preceding char was $ (removes $$$... sequences)
          if (i == d0 + 1 && d0 != -1) d_start = false;
          // -- remember the $ index
          d0 = i;
        }
        else {
          // -- shift the $ index for consecutive $ chars
          if (i == d0 + 1) d0 = i;
          d_start = false;
        }
      }
      else if (d_start) {
        if (word[i] >= '0' && word[i] <= '9') {
          // -- skip
        }
        else {
          d_start = false;
          // -- restore skipped chars when non-digit char found (not closed sequences)
          for (int j = d0; j <= i; ++j) {
            result += word[j];
          }
        }
      }
      else {
        result += word[i];
      }
    }
    
    return result;
  }

  string stripSlash(const string& word) {
    if (word.find("/") == string::npos) return word;
    string result = "";
    
    int len = word.size();
    for (int i = 0; i < len; i++) {
      if (word[i] == '/' && (i == 0 || word[i-1] == ' ')) {
        // -- skip
      }
      else {
        result += word[i];
      }
    }
    
    return result;
  }
  
  string convertHtmlEntities(const string& inText) {
    string text;
    string entityText;
    bool entityStart = false;
    
    text.reserve(inText.size());
    
    size_t len = inText.size();
    for (size_t strPos = 0; strPos < len; strPos++) {
      char c = inText[strPos];
      
      if (c == '&' && strPos != len-1 && inText[strPos+1] == '#') {
        entityStart = true;
      }
      else if (entityStart) {
        if (c == ';') {
          entityStart = false;
          if (entityText.size() > 0) {
            int base = 10;
            if (entityText[0] == 'x') {
              entityText = entityText.substr(1);
              base = 16;
            }
            
            char* end;
            unsigned long codeVal = strtoul(entityText.c_str(), &end, base);
            char buf[4];
            char* conv_buf = buf;
            size_t buf_len = 0;
            
            if (codeVal <= 0xff) {
              buf[0] = (unsigned char) codeVal;
              size_t inSize = 1;
              size_t outSize = 10;
              char res[outSize];
              
              const char* pIn = buf;
              char* pOut = res;
              
              size_t inRemains = inSize;
              size_t outRemains = outSize;
              
              iconv_t cb = iconv_open("UTF-8", "CP1252");
              size_t cvtlen = iconv(cb, (char**) &pIn, &inRemains, (char**) &pOut, &outRemains);
              iconv_close(cb);
              
              size_t resSize = outSize - outRemains;
              buf_len = resSize;
              conv_buf = res;
            }
            else {
              printf("WARNING: HTML entity code > 255: %ld [0x%lx]\n", codeVal, codeVal);
              buf_len = codePointToUtf(codeVal, buf);
            }
            
            for (int i = 0; i < buf_len; ++i) {
              text.push_back(conv_buf[i]);
            }
          }
          
          entityText.clear();
        }
        else if (c != '#') {
          entityText.push_back(c);
        }
      }
      else {
        text.push_back(c);
      }
    }
    
    return text;
  }
  
  string decodeCharsetTags(const string& inText) {
    string tag;
    string tagContent;
    string text;
    
    char charsetType;
    
    bool tagStarted = false;
    bool tagOpened = false;
    
    text.reserve(inText.size());
    
    size_t len = inText.size();
    for (size_t strPos = 0; strPos < len; strPos++) {
      char c = inText[strPos];
      
      if (tagStarted) {
        tag.push_back(c);
        
        if (c == '>') {
          tagStarted = false;
          bool isOpenTag = (tag[1] != '/');
          
          bool tagProcessed = false;
          if (isOpenTag) {
            tagOpened = true;
            string name = getTagName(tag);
            
            if (name == "br") {
              tagOpened = false;
            }
            
            if (name == "charset") {
              string attrs = getTagAttrs(tag);
              auto attrsMap = parseTagAttrs(attrs);
              charsetType = tolower(attrsMap["c"][0]);
            }
            else {
              text.append(tag);
            }
          }
          else {
            tagOpened = false;
            string name = getTagName(tag);
            
            if (name == "charset") {
              // cout << "tagContent: " << tagContent << endl;
              
              if (charsetType == 't') {
                auto codes = split_string(tagContent, ";");
                for (auto code: codes) {
                  char* end;
                  unsigned long codeVal = strtoul(code.c_str(), &end, 16);
                  char buf[4];
                  
                  size_t buf_len = codePointToUtf(codeVal, buf);
                  for (int i = 0; i < buf_len; ++i) {
                    text.push_back(buf[i]);
                  }
                }
              }
              else if (charsetType == 'k' || charsetType == 'e') {/*sourceEncoding*/}
              else if (charsetType == 'g') {/*gbk*/}
              else {
                char *end;
                unsigned int codeVal = strtoul(tagContent.c_str(), &end, 16);
                char bytes[4];
                bytes[0] = (codeVal >> 8) & 0xFF;
                bytes[1] = codeVal & 0xFF;
                
                text.push_back(bytes[0]);
                text.push_back(bytes[1]);
              }
            }
            else {
              text.append(tagContent);
              text.append(tag);
            }
            
            tagContent.clear();
          }
          
          tag.clear();
        }
      }
      else if (c == '<') {
        tagStarted = true;
        tag.push_back(c);
      }
      else if (tagOpened) {
        tagContent.push_back(c);
      }
      else {
        text.push_back(c);
      }
    }
    
    return text;
  }
  
  string html_to_dsl(const string& inText) {
    string tag;
    string tempTag;
    string text = "  ";
    
    bool tagStarted = false;
    bool newLineInserted = false;
    
    size_t len = inText.size();
    for (size_t strPos = 0; strPos < len; strPos++) {
      char c = inText[strPos];
      
      if (tagStarted) {
        tag.push_back(c);
        
        if (c == '>') {
          tagStarted = false;
          bool isOpenTag = (tag[1] != '/');
          string close_ch = isOpenTag ? "" : "/";
          
          bool tagProcessed = false;
          if (isOpenTag) {
            // cout << "OPEN: " << tag << endl;
            tempTag = tag;
            
            if (findTag(tag, "br", isOpenTag)) {
              text += "\n  ";
              newLineInserted = true;
              tagProcessed = true;
            }
          }
          else {
            // cout << "CLOSE: " << tag << endl;
            tempTag.clear();
            
            if (findTag(tag, "p", isOpenTag) || findTag(tag, "div", isOpenTag)) {
              text += "\n  ";
              newLineInserted = true;
              tagProcessed = true;
            }
          }
          
          if (!tagProcessed) {
            for (auto item: tags_literal) {
              if (findTag(tag, item, isOpenTag)) {
                text += "[" + close_ch + item + "]";
                tagProcessed = true;
                break;
              }
            }
          }
          
          if (!tagProcessed) {
            for (auto item: tags_change) {
              if (findTag(tag, item.first, isOpenTag)) {
                text += "[" + close_ch + item.second + "]";
                tagProcessed = true;
                break;
              }
            }
          }
          
          if (!tagProcessed) {
            for (auto item: tags_replace) {
              if (findTag(tag, item.first, isOpenTag)) {
                if (isOpenTag) text += regex_replace(tag, regex(item.second.first), item.second.second);
                else text += "[/" + item.first + "]";
                break;
              }
            }
          }
          
          tag.clear();
        }
      }
      else if (c == '<') {
        tagStarted = true;
        tag.push_back(c);
      }
      else if (c == '[' || c == ']') {
        text.push_back('\\');
        text.push_back(c);
      }
      else if (c == '\n') {
        if (newLineInserted) continue;
        if (c == '\n') text += "\n  ";
        newLineInserted = false;
      }
      else {
        if (c == 0x00) continue;
        if (tempTag.size() && findTag(tempTag, "rref", true)) continue;
        
        text.push_back(c);
      }
    }
    
    return text;
  }
  
}
