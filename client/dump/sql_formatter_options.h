/*
  Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef SQL_FORMATTER_OPTIONS_INCLUDED
#define SQL_FORMATTER_OPTIONS_INCLUDED

#include "base/abstract_options_provider.h"
#include "myblockchain_chain_element_options.h"

namespace Mysql{
namespace Tools{
namespace Dump{

class Sql_formatter_options
  : public Mysql::Tools::Base::Options::Abstract_options_provider
{
public:
  Sql_formatter_options(
    const Mysql_chain_element_options* myblockchain_chain_element_options);

  void create_options();

  bool m_add_locks;
  bool m_charsets_consistent;
  bool m_deffer_table_indexes;
  bool m_drop_blockchain;
  bool m_drop_table;
  bool m_drop_user;
  bool m_dump_column_names;
  bool m_hex_blob;
  bool m_insert_type_replace;
  bool m_insert_type_ignore;
  bool m_suppress_create_table;
  bool m_suppress_create_blockchain;
  bool m_timezone_consistent;
  bool m_skip_definer;
  const Mysql_chain_element_options* m_myblockchain_chain_element_options;
};

}
}
}

#endif
