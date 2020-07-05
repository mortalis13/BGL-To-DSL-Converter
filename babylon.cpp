#include "babylon.h"

#include <stdlib.h>
#include <stdio.h>

#include <iconv.h>

#include <fstream>
#include <iomanip>

#include "utils.h"

Babylon::Babylon(string filename) {
  m_filename = filename;
  file = NULL;
  m_defaultCharset = bgl_charset[0];
}

Babylon::~Babylon() {}


bool Babylon::open() {
  FILE *f;
  unsigned char buf[6];
  int i;

  f = fopen(m_filename.c_str(), "rb");
  if (f == NULL)
    return false;

  i = fread(buf, 1, 6, f);

  /* First four bytes: BGL signature 0x12340001 or 0x12340002 (big-endian) */
  if (i < 6 || memcmp(buf, "\x12\x34\x00", 3) || buf[3] == 0 || buf[3] > 2)
    return false;
  
  /* Calculate position of gz header */
  i = buf[4] << 8 | buf[5];

  if (i < 6)
    return false;

  fseek(f, i, SEEK_SET);

  if (ferror(f) || feof(f))
    return false;
  
  file = gzdopen(dup(fileno(f)), "r");
  // file = gzopen(tmp_filename.c_str(), "r");
  if (file == NULL)
    return false;
  
  fclose(f);
  return true;
}


void Babylon::close() {
   gzclose(file);
}


bool Babylon::readBlock(bgl_block &block) {
  if (gzeof(file) || file == NULL) {
    return false;
  }

  block.length = bgl_readnum(1);
  block.type = block.length & 0xf;
  if (block.type == 4) {
    return false; // end of file marker
  }
  block.length >>= 4;
  block.length = block.length < 4 ? bgl_readnum(block.length + 1) : block.length - 4 ;
  if (block.length) {
    block.data = (char *) malloc(block.length);
    int n = gzread(file, block.data, block.length);
  }
  
  return true;
}

unsigned int Babylon::bgl_readnum(int bytes) {
  unsigned char buf[4];
  unsigned val = 0;

  if (bytes < 1 || bytes > 4) return (0);

  int n = gzread(file, buf, bytes);
  for (int i = 0; i < bytes; i++) val= (val << 8) | buf[i];
  return val;
}

bool Babylon::read() {
  if (file == NULL) return false;

  bgl_block block;
  uint pos;
  uint type;
  string headword;
  string definition;

  m_numEntries = 0;
  while (readBlock(block)) {
    headword.clear();
    definition.clear();
    
    if (block.type == 0) {
      if (block.data[0] == 0x08) {
        // !! crashes on type > bgl_charset.size()
        // type = (uint)block.data[2];
        // if (type > 64) type -= 65;
        // if (type > 0 && type < CHARSETS_NUM)
        // m_defaultCharset = bgl_charset[type];
      }
    }
    else if (block.type == 1 || block.type == 10) {
      m_numEntries++;
    }
    else if (block.type == 3) {
      string value = "";
      
      if (block.data[1] == 0x01) {
        for (int i=2; i<block.length; i++) value += block.data[i];
        m_title = value;
      }
      else if (block.data[1] == 0x07) {
        m_sourceLang = bgl_language[block.data[5]];
      }
      else if (block.data[1] == 0x08) {
        m_targetLang = bgl_language[block.data[5]];
      }
      else if (block.data[1] == 0x1a) {
        type = (uint)block.data[2];
        if (type > 64) type -= 65;
        if (type < CHARSETS_NUM) m_sourceCharset = bgl_charset[type];
        else log("ERROR: Unknown source charset");
      }
      else if (block.data[1] == 0x1b) {
        type = (uint)block.data[2];
        if (type > 64) type -= 65;
        if (type < CHARSETS_NUM) m_targetCharset = bgl_charset[type];
        else log("ERROR: Unknown target charset");
      }
    }
    
    if (block.length) free(block.data);
  }
  gzseek(file, 0, SEEK_SET);

  convertToUtf8(m_title, DEFAULT_CHARSET);
  convertToUtf8(m_author, DEFAULT_CHARSET);
  convertToUtf8(m_email, DEFAULT_CHARSET);
  convertToUtf8(m_copyright, DEFAULT_CHARSET);
  convertToUtf8(m_description, DEFAULT_CHARSET);
  
  return true;
}


bgl_entry Babylon::readEntry() {
  bgl_entry entry;

  if (file == NULL) {
    entry.end = true;
    return entry;
  }

  bgl_block block;
  uint len, pos;
  string headword;
  string definition;
  vector<string> alternates;
  string alternate;
  
  while (readBlock(block)) {
    if (block.type == 1 || block.type == 10) {
      logd("\n\n..block.type: %d", block.type);
      
      alternate.clear();
      headword.clear();
      definition.clear();
      pos = 0;

      // == Headword
      len = 0;
      len = (unsigned char)block.data[pos++];
      headword.reserve(len);
      
      for (uint a = 0; a < len; a++) {
        if (Utils::isControlChar(block.data[pos])) {
          pos++;
        }
        else {
          headword += block.data[pos++];
        }
      }
      
      headword = Utils::stripDollar(headword);
      convertToUtf8(headword, SOURCE_CHARSET);
      
      
      // == Definition
      len = 0;
      len = (unsigned char)block.data[pos++] << 8;
      len |= (unsigned char)block.data[pos++];
      definition.reserve(len);
      
      uint def_pos = pos;
      bool fieldsStarted = false;
      
      for (uint a = 0; a < len; a++) {
        if (block.data[pos] == 0x0a) {
          definition += '\n';
          pos++;
        }
        else if (block.data[pos] == 0x14) {
          logd("..Fields Start, len: %d, (%d-%d), pos: %d", len-a, len, a, pos);
          fieldsStarted = true;
          pos++;
          break;
        }
        else if (Utils::isControlChar(block.data[pos])) {
          pos++;
        }
        else {
          definition += block.data[pos++];
        }
      }
      
      if (fieldsStarted) {
        string defPartOfSpeech = "";
        string defTitle = "";
        string defTitleTrans = "";
        string defTrans50 = "";
        string defTrans60 = "";
        logd("..[1]: %d, %d, %d", pos, len, block.length);
        
        while (pos - def_pos < len) {
          logd("pos: %d", pos);
          if (block.data[pos] == 0x02) {
            logd("..PartOfSpeech field");
            int pCode = (unsigned char) block.data[pos+1];
            if (partOfSpeech.find(pCode) != partOfSpeech.end()) defPartOfSpeech = partOfSpeech[pCode];
            pos += 2;
          }
          else if (block.data[pos] == 0x06) {
            logd("..0x06 field");
            pos += 2;
          }
          else if (block.data[pos] == 0x07) {
            logd("..0x07 field");
            pos += 3;
          }
          else if (block.data[pos] == 0x13) {
            logd("..0x13 field");
            int data_len = (unsigned char) block.data[pos+1];
            pos += 2;
            
            if (data_len == 0) continue;
            if (data_len < 0) {log("ERROR: length < 0"); break;}
            
            pos += data_len;
          }
          else if (block.data[pos] == 0x18) {
            logd("..Title field");
            int data_len = (unsigned char) block.data[pos+1];
            pos += 2;
            
            if (data_len == 0) continue;
            if (data_len < 0) {log("ERROR: length < 0"); break;}
            
            if (data_len > 0 && (pos - def_pos + data_len <= len)) {
              defTitle = string(block.data + pos, block.data + pos + data_len);
            }
            pos += data_len;
          }
          else if (block.data[pos] == 0x1a) {
            logd("..0x1a field");
            int data_len = (unsigned char) block.data[pos+1];
            pos += 2;
            
            if (data_len == 0) continue;
            if (data_len < 0) {log("ERROR: length < 0"); break;}
            
            pos += data_len;
          }
          else if (block.data[pos] == 0x28) {
            logd("..Title_Trans field\n");
            pos++;
            int data_len = ((unsigned char) block.data[pos] << 8) + (unsigned char) block.data[pos+1];
            pos += 2;
            
            if (data_len == 0) continue;
            if (data_len < 0) {log("ERROR: length < 0"); break;}
            
            if (data_len > 0 && pos - def_pos + data_len <= len) {
              defTitleTrans = string(block.data + pos, block.data + pos + data_len);
            }
            pos += data_len;
          }
          else if (block.data[pos] >= 0x40 && block.data[pos] <= 0x4f) {
            logd("..[0x40-0x4f] field: 0x%02x", block.data[pos]);
            int data_len = (unsigned char) block.data[pos] - 0x3f;
            pos += 2;
            pos += data_len;
          }
          else if (block.data[pos] == 0x50) {
            logd("..Trans50 field");
            int code = block.data[pos+1];
            int data_len = (unsigned char) block.data[pos+2];
            pos += 3;
            
            if (data_len == 0) continue;
            if (data_len < 0) {log("ERROR: length < 0"); break;}
            
            if (data_len > 0 && pos - def_pos + data_len <= len) {
              defTrans50 = string(block.data + pos, block.data + pos + data_len);
            }
            pos += data_len;
            
            logd("pos: %d, data_len: %d, trans: %s", pos, data_len, defTrans50.c_str());
          }
          else if (block.data[pos] == 0x60) {
            logd("..Trans60 field");
            int code = block.data[pos+1];
            pos += 2;
            int data_len = ((unsigned char) block.data[pos] << 8) + (unsigned char) block.data[pos+1];
            pos += 2;
            
            if (data_len == 0) continue;
            if (data_len < 0) {log("ERROR: length < 0"); break;}
            
            if (data_len > 0 && pos - def_pos + data_len <= len) {
              defTrans60 = string(block.data + pos, block.data + pos + data_len);
            }
            pos += data_len;
          }
          else if (Utils::isControlChar(block.data[pos])) {
            logd("..Remove_char field: 0x%02x", block.data[pos]);
            pos++;
          }
          else {
            logd("..ERROR: Unknown field: 0x%02x", block.data[pos]);
            entry.end = true;
            return entry;
          }
        }
        
        string definition_format = "";
        if (defPartOfSpeech.size() > 0) {
          logd("..defPartOfSpeech: %s", defPartOfSpeech.c_str());
          definition_format += defPartOfSpeech + "<br>";
        }
        if (defTitle.size() > 0) {
          logd("..defTitle: %s", defTitle.c_str());
          definition_format += defTitle + "<br>";
        }
        if (defTitleTrans.size() > 0) {
          logd("..defTitleTrans: %s", defTitleTrans.c_str());
          definition_format += defTitleTrans + "<br>";
        }
        if (defTrans50.size() > 0) {
          logd("..defTrans50: %s", defTrans50.c_str());
          definition_format += "[" + defTrans50 + "]" + "<br>";
        }
        if (defTrans60.size() > 0) {
          logd("..defTrans60: %s", defTrans60.c_str());
          definition_format += "[" + defTrans60 + "]" + "<br>";
        }
        
        definition = definition_format + definition;
      }
      
      // convertToUtf8(definition, TARGET_CHARSET);
      convertToUtf8(definition, DEFAULT_CHARSET);       // CP1252, converts some chars better than ISO-8859-1
      
      definition = Utils::convertHtmlEntities(definition);
      definition = Utils::decodeCharsetTags(definition);
      definition = Utils::html_to_dsl(definition);
      

      // == Alternate forms
      while (pos < block.length) {
        len = (unsigned char)block.data[pos++];
        alternate.reserve(len);
        for (uint a = 0; a < len; a++) {
          alternate += block.data[pos++];
        }
        alternate = Utils::stripDollar(alternate);
        alternate = Utils::stripSlash(alternate);
        
        convertToUtf8(alternate, SOURCE_CHARSET);
        if (alternate != headword) alternates.push_back(alternate);
        alternate.clear();
      }

      entry.headword = headword;
      entry.definition = definition;
      entry.alternates = alternates;
      return entry;
    }
  
    if (block.length) free(block.data);
  }
  
  entry.end = true;
  return entry;
}

void Babylon::convertToUtf8(string &s, uint type) {
  if (s.size() < 1) return;
  if (type > 2) return;

  string charset;
  switch(type) {
    case DEFAULT_CHARSET:
      if (m_defaultCharset.size() > 0 ) charset = m_defaultCharset;
      else charset = m_sourceCharset;
      break;
    case SOURCE_CHARSET:
      if (m_sourceCharset.size() > 0) charset = m_sourceCharset;
      else charset = m_defaultCharset;
      break;
    case TARGET_CHARSET:
      if (m_targetCharset.size() > 0) charset = m_targetCharset;
      else charset = m_defaultCharset;
      break;
    default:
      ;
  }
  
  iconv_t cd = iconv_open("UTF-8", charset.c_str());
  if (cd == (iconv_t)(-1)) {
    log("Error openning iconv library");
    exit(1);
  }

  char *inbuf, *outbuf, *defbuf;
  size_t inbufbytes, outbufbytes;

  inbufbytes = s.size();
  outbufbytes = s.size() * 6;
  inbuf = (char*)s.data();
  outbuf = (char*)malloc(outbufbytes + 1);
  memset(outbuf, '\0', outbufbytes + 1);
  defbuf = outbuf;
  
  while (inbufbytes) {
    if (iconv(cd, &inbuf, &inbufbytes, &outbuf, &outbufbytes) == -1) {
      log("%s", outbuf);
      log("Error in iconv conversion");
      exit(1);
    }
  }
  s = string(defbuf);

  free(defbuf);
  iconv_close(cd);
}
