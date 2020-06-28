/*
** Copyright (C) 2020 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public License
**  as published by the Free Software Foundation; either version 2.1
**  of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free
**  Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
**  02110-1301, USA.
*/


#ifndef __XAPIAN_HH__
#define __XAPIAN_HH__

#include <xapian.h>
#include <query/mu-parser.hh>

namespace Mu {

/**
 * Transform a parse-tree into a Xapian query object
 *
 * @param tree a parse tree
 *
 * @return a Xapian query object
 */
Xapian::Query xapian_query (const Mu::Tree& tree);

} // namespace Mu

#endif /* __XAPIAN_H__ */
