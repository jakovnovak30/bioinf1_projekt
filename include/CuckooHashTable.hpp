#pragma once

#include "logging.hpp"

template <typename T> class CuckooHashTable {
public:
	CuckooHashTable() = default;
	~CuckooHashTable() = default;

	void insert(T entry) {
	  WARN("TODO: should insert something!");
	}
	bool lookup(T entry) {
    WARN("TODO: should lookup something!");
    return true;
  }
	void del(T entry) {
	  WARN("TODO: should delete something!");
  }
};
