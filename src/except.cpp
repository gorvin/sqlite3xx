/*
    sqlite3xx - sqlite3 C++ layer, following ideas of libpqxx
    Copyright (C) 2009  Andreas Baumann

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sqlite3xx/except.hpp"

namespace sqlite3xx {

sqlitexx_exception::~sqlitexx_exception( ) throw( )
{
}

failure::failure( const string &_what )
	: sqlitexx_exception( ), runtime_error( _what )
{
}

sql_error::sql_error( )
	: failure( "failed query" ), m_q( )
{
}

sql_error::sql_error( const string &_what )
	: failure( _what ), m_q( )
{
}

sql_error::sql_error( const string &_what, const string &_q )
	: failure( _what ), m_q( _q )
{
}

sql_error::~sql_error(  ) throw( )
{
}

const string& sql_error::query( ) const throw( )
{
	return m_q;
}

database_locked::database_locked( )
	: failure( "database locked" )
{
}

database_locked::database_locked( const string &_what )
	: failure( _what )
{
}

} /* namespace sqlite3xx */
