/*
 *  sqlite3xx - sqlite3 C++ layer, following the ideas of libpqxx
 *  Copyright (C) 2009  Andreas Baumann
 *
 *  This copyrighted material is made available to anyone wishing to use,
 *  modify, copy, or redistribute it subject to the terms and conditions of
 *  the GNU Lesser General Public License, as published by the Free Software
 *  Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this distribution; if not, write to:
 *  Free Software Foundation, Inc.
 *  51 Franklin Street, Fifth Floor
 *  Boston, MA  02110-1301  USA
 *
 */

#ifndef SQLITE3XX_EXCEPT_H
#define SQLITE3XX_EXCEPT_H

#include <stdexcept>
namespace sqlite3xx {
	using std::string;
	using std::exception;
	using std::runtime_error;
}

#include "port/dllexport.h"

namespace sqlite3xx {

class SQLITEXX_LIBEXPORT sqlitexx_exception {
	public:
		virtual ~sqlitexx_exception( ) throw( ) = 0;

		virtual exception const &base() const throw( ) = 0;
};

class SQLITEXX_LIBEXPORT failure : public sqlitexx_exception, public runtime_error
{
	virtual exception const &base( ) const throw( ) { return *this; }

	public:
		explicit failure( const string &m );
};

class SQLITEXX_LIBEXPORT sql_error : public failure
{
	string m_q;

	public:
		sql_error( );
		explicit sql_error( const string& _q );
		explicit sql_error( const string& _what, const string& _q );
		virtual ~sql_error( ) throw( );

		const string& msg( ) const throw( );
		const string& query( ) const throw( );
};

class SQLITEXX_LIBEXPORT database_locked : public failure 
{	
	public:
		database_locked( );
		explicit database_locked( const string &_what );
};

} // namespace sqlite3xx

#endif /* SQLITE3XX_EXCEPT_H */
