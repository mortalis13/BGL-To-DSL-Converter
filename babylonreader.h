/***************************************************************************
 *   Copyright (C) 2007 by Raul Fernandes                                  *
 *   rgfbr@yahoo.com.br                                                    *
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

#ifndef BABYLONREADER_H
#define BABYLONREADER_H

#include "babylon.h"

#include <string>
#include <map>

using namespace std;

class BabylonReader {

  public:
    BabylonReader(string filename, string outFile);
    bool convert();

    inline string filename() const {return m_babylon->filename();};

  protected:
    struct entry {
      unsigned long position;
      unsigned long size;
    };

    Babylon *m_babylon;

    string m_outFile;

    unsigned int m_entriescount;
    string m_definition;

};

#endif // BABYLONREADER_H
