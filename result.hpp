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

#ifndef SQLITE3XX_RESULT_H
#define SQLITE3XX_RESULT_H

#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>
namespace sqlite3xx {
	using std::string;
	using std::vector;
	using std::pair;
	using std::map;
	using std::ostream;
	using std::ostringstream;
	using std::invalid_argument;
}

#include "port/iterator.hpp"
#include "port/dllexport.h"

namespace sqlite3xx {

class SQLITEXX_LIBEXPORT result {
	public:
		class field;
		class const_iterator;

		typedef unsigned long size_type;
		typedef signed long difference_type;

	class SQLITEXX_LIBEXPORT tuple {
		public:
			typedef unsigned int size_type;

		private:
			const result *_r;
			result::size_type _i;

		public:
			tuple( const result *r, size_type i ) :
				_r( r ), _i( i ) { }
			~tuple( ) { }

			size_type size( ) const {
				return _r->columns( );
			}

			field operator[]( size_type i ) const {
				return field( *this, i );
			}
			field operator[]( int i ) const {
				return operator[]( (size_type)i );
			}
			field operator[]( string s ) const {
				return field( *this, _r->column_number( s ) );
			}

			int column_type( size_type i ) const {
				return _r->column_type( i );
			}

			int GetValueInt( size_type i ) const {
				return _r->GetValueInt( i );
			}

			const unsigned char* GetValueText( size_type i ) const {
				return _r->GetValueText( i );
			}

			const int GetValueBytes(size_type i) const {
				return _r->GetValueBytes(i);
			}

			double GetValueDouble( size_type i ) const {
				return _r->GetValueDouble( i );
			}

			size_type rownumber( ) const throw( ) {
				return _i;
			}

		protected:
			tuple( );
	};

	class SQLITEXX_LIBEXPORT field {
		private:
			tuple _t;
			tuple::size_type _c;

			SQLITEXX_PRIVATE string type_to_str(const int type) const;

		public:
			field( const tuple& t, tuple::size_type c ) :
				_t( t ), _c( c ) { }

			SQLITEXX_LIBEXPORT friend ostream& operator<<( ostream& o, const field& f );

			void GetValueOfType(int type, int& obj) const;

			void GetValueOfType(int type, std::basic_string<unsigned char>& obj) const;

			void GetValueOfType(int type, std::string& obj) const;

			void GetValueOfType(int type, double& obj) const;

			template<typename T> bool to( T& obj ) const {
				int type = _t.column_type( _c );
				GetValueOfType( type, obj );
				return true;
			}
		};

	typedef sqlite3xxstd::iterator< std::random_access_iterator_tag,
		const tuple,
		result::difference_type,
		const_iterator,
		tuple>
	const_iterator_base;

	class SQLITEXX_LIBEXPORT const_iterator :
		public const_iterator_base,
		public tuple {
		
		public:
			typedef const tuple *pointer;

			const_iterator( ) throw( ) : tuple( 0, 0 ) { }
			const_iterator( const tuple& t ) throw( )
				: tuple( t ) { }

			pointer operator->( ) const { return this; }
			const_iterator operator++( int incr );
			bool operator<( const const_iterator& i ) const;

		private:
			int dummy;
			friend class result;
			SQLITEXX_PRIVATE const_iterator( const result* r, size_type i ) throw( ) :
				tuple( r, i ) {
			};
	};

	private:
		struct sqlite3_stmt* _stmt;
		size_type mutable _row;		/* the currently visible row from the cache */
		size_type mutable _crow;	/* the row of the sqlite step cursor */
		typedef map<string, size_type> ColMap;
		ColMap _colmap;
		typedef vector<int> ColType;
		ColType _coltype;

		enum Status {
			st_nascent,	/* created, we must call step */
			st_nodata,	/* a command without result data */
			st_lastrow,	/* a query, on the last row */
                        st_hasdata,	/* a query, we must clean up in the end */
			st_nomoredata   /* a query, with no data left */
		};
		
		class CachedValue {
			public:
				int type;
				union {
					int i;
					double d;
					unsigned char *s;
				} value;
				int bycount;
			public:
				CachedValue() { type = 0; value.s = NULL; bycount = 0; };
				CachedValue( const CachedValue& v );
				~CachedValue( );
				CachedValue& operator= ( const CachedValue& v );
		};
		
		typedef vector<CachedValue> ValueCache;
		ValueCache mutable _cache;

		Status mutable _status;
		
	public:
		result( sqlite3_stmt* stmt );
		result( const result& r );
		~result( );
		result& operator= ( const result& r );

		size_type affected_rows( ) const;
		size_type size( ) const;
		size_type columns( ) const;

		const tuple operator[]( size_type i ) const;

		size_type column_number( string name ) const;
		int column_type( size_type i ) const;
		int column_type( string name ) const {
			return column_type( column_number( name ) );
		}

		int GetValueInt( size_type i ) const {
			return _cache[i].value.i;
		}

		const unsigned char* GetValueText( size_type i ) const {
			return _cache[i].value.s;
		}

		const int GetValueBytes(size_type i) const {
			return _cache[i].bycount;
		}

		double GetValueDouble( size_type i ) const {
			return _cache[i].value.d;
		}

		const_iterator begin( ) const throw( ) {
			return const_iterator( this, 0 );
		}

		/* as size( ) is not reliable, we can't create the correct
		 * tuple with the correct index here
		 */
		const_iterator end( ) const throw( ) {
			return const_iterator( this, size( ) );
		}

	private:
		SQLITEXX_PRIVATE void Step( ) const;
		SQLITEXX_PRIVATE void BufferData( ) const;
		SQLITEXX_PRIVATE void FillColNameMap( );
		SQLITEXX_PRIVATE void FillColTypeMap( );
};

}

#endif /* SQLITE3XX_RESULT_H */
