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
#include "babylon.h"
#include "dictbuilder.h"

#include <stdio.h>

BabylonReader::BabylonReader( std::string filename, DictBuilder *builder )
{
  m_babylon = new Babylon( filename );
  m_builder = builder;
}


bool BabylonReader::convert()
{
  if( !m_babylon->open() )
  {
    printf( "Error openning %s\n", m_babylon->filename().c_str() );
    return false;
  }

  m_babylon->read();
  if( !m_babylon->read() )
  {
    printf( "Error reading %s\n", m_babylon->filename().c_str() );
    return false;
  }

  bgl_entry entry;
  entry = m_babylon->readEntry();

  while( entry.headword != "" )
  {
    m_builder->addHeadword( entry.headword.c_str(), entry.definition.c_str(), entry.alternates );
    entry = m_babylon->readEntry();
    printf( "." );
  }
  printf( "\n" );

  m_babylon->close();

  m_builder->setTitle( m_babylon->title() );
  m_builder->setAuthor( m_babylon->author() );
  m_builder->setEmail( m_babylon->email() );
  m_builder->setLicense( m_babylon->copyright() );
  m_builder->setOrigLang( m_babylon->sourceLang() );
  m_builder->setDestLang( m_babylon->targetLang() );
  m_builder->setDescription( m_babylon->description() );

  return true;
}
