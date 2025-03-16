/***************************************************************************
 *   Copyright (C) 2007 by Raul Fernandes                                  *
 *   rgbr@yahoo.com.br                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/

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


bool BabylonReader::convert() {
  // --- Read
  bool res = m_babylon->open();
  if(!res) {
    log("Error openning %s", m_babylon->filename().c_str());
    return false;
  }

  res = m_babylon->read();

  log("numEntries: %d", m_babylon->numEntries());
  log("title: %s", m_babylon->title().c_str());
  log("sourceLang: %s", m_babylon->sourceLang().c_str());
  log("targetLang: %s", m_babylon->targetLang().c_str());
  log("defaultCharset: %s", m_babylon->defaultCharset().c_str());
  log("sourceCharset: %s", m_babylon->sourceCharset().c_str());
  log("targetCharset: %s", m_babylon->targetCharset().c_str());


  // --- Process
  fstream f(m_outFile, ios::out);
  if (!f.is_open()) {
    log("File open ERROR");
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
