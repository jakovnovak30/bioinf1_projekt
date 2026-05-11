#pragma once

#include "logging.hpp"

/**
 * Implementation of CuckooHashTable
 *
 * @author Jakov Novak
 */
template <typename T> class CuckooHashTable {
public:
	CuckooHashTable() = default;
	~CuckooHashTable() = default;

  /**
   * This function inserts an item into the hash table.
   *
   * @param entry any hashable type that is to be stored in the table
   * @return nothing
   *
   * @author Jakov Novak
   */
	void insert(T entry) {
	  WARN("TODO: should insert something!");
	}
  /**
   * This function checks if an item is already in the hash table.
   *
   * @param entry value we want to look up
   * @return boolean, true if entry is contained in table
   *
   * @author Jakov Novak
   */
	bool lookup(T entry) {
    WARN("TODO: should lookup something!");
    return true;
  }
  /**
   * Function that deletes an entry from table
   *
   * @param entry value that needs to be deleted from table
   * @return nothing
   *
   * @author Jakov Novak
   */
	void del(T entry) {
	  WARN("TODO: should delete something!");
  }
};
