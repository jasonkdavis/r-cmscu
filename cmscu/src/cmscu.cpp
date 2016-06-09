// This is a somewhat canonical implementation of a Count-Min-Sketch with Conservative Updating
// A hashing optimization from https://www.eecs.harvard.edu/~michaelm/postscripts/rsa2008.pdf is implemented
// along with the public domain MurmurHash3 code.

// [[Rcpp::plugins(openmp)]]
#ifdef _OPENMP
#include <omp.h>
#endif

#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include "MurmurHash3.h"

#include "cmscu.h"

namespace {
	// this is the canonical lookup procedure for the count min sketch
	uint_t query_helper(const std::string& str, unsigned int* hash, const std::vector< std::vector<uint_t> >& table, unsigned int w, unsigned int d) {
		// deconstruct the string
		const char* bytes = str.c_str();
		unsigned int len = str.length();

		// hash the string's bytes (we only use hash[0] and hash[1])
		MurmurHash3_x86_128(bytes, len, 0, hash);

		// do the lookup
		unsigned int index = hash[0];
		uint_t c = table[0][index % w];
		for (unsigned int i = 1; i < d; i++) {
			index += hash[1];
			c = std::min(c, table[i][index % w]);
		}

		// return the result
		return c;
	}
}




// The constructor: d is the number of hash functions, w is the number of entries
CountMinSketch::CountMinSketch(unsigned int d_, unsigned int w_) {
	d = d_;
	w = w_;
	count = 0;
	ucount = 0;

	table.resize(d);
	for (unsigned int i = 0; i < d; i++)
		table[i].resize(w, 0);
}




uint_t CountMinSketch::query(const std::string& str) const {
	unsigned int hash[] = { 0, 0, 0, 0 };
	return query_helper(str, hash, table, w, d);
}

std::vector<uint_t> CountMinSketch::query(const std::vector<std::string>& words) const {
	const size_t len = words.size();
	std::vector<uint_t> response(len);

	for (size_t i = 0; i < len; i++)
		response[i] = query(words[i]);

	return response;
}

std::vector<uint_t> CountMinSketch::query(const std::vector<std::string>& words, int n) const {
#ifdef _OPENMP
	int m = omp_get_num_threads();
	if (n > 1)
		omp_set_num_threads(n);

	const size_t len = words.size();
	std::vector<uint_t> response(len);

#pragma omp parallel for
	for (size_t i = 0; i < len; i++)
		response[i] = query(words[i]);

	omp_set_num_threads(m);

#else
	std::vector<uint_t> response = query(words);
#endif

	return response;
}


uint_t CountMinSketch::store(const std::string& str) {
	unsigned int hash[4] = { 0, 0, 0, 0 };
	uint_t c = query_helper(str, hash, table, w, d) + 1;
	unsigned int index = hash[0];
	for (unsigned int i = 0; i < d; i++) {
		uint_t* val = &(table[i][index % w]);
		index += hash[1];
		if (c > *val) {
			// the conservative update
			*val = c;
		}
	}

	++count;
	if (c == 1)
		++ucount;
	return c;
}

std::vector<uint_t> CountMinSketch::store(const std::vector<std::string>& words) {
	const size_t len = words.size();
	std::vector<uint_t> response(len);
	for (size_t i = 0; i < len; i++)
		response[i] = store(words[i]);
	return response;
}


uint_t CountMinSketch::entries() const {
	return count;
}
uint_t CountMinSketch::uniqueEntries() const {
	return ucount;
}
double CountMinSketch::uncertainty() const {
	return 2.718281828459 / (double) w * (double) count;
}
double CountMinSketch::confidence() const {
	return 1.0 - exp(-(double) d);
}

bool CountMinSketch::save(const std::string& file) const {
	std::ofstream output(file.c_str(), std::ofstream::binary);
	if (output.is_open()) {
		for (unsigned int i = 0; i < d; i++) {
			output.write((char*) &(table[i][0]), w * sizeof(uint_t));
		}
		output.write((char*) &count, sizeof(uint_t));
		output.write((char*) &ucount, sizeof(uint_t));
		output.close();
		return true;
	}
	return false;
}

bool CountMinSketch::read(const std::string& file) {
	std::ifstream input(file.c_str(), std::ifstream::binary);
	if (input.is_open()) {
		for (unsigned int i = 0; i < d; i++) {
			input.read((char*) &(table[i][0]), w * sizeof(uint_t));
		}
		input.read((char*) &count, sizeof(uint_t));
		input.read((char*) &ucount, sizeof(uint_t));
		input.close();
		return true;
	}
	return false;
}


double CountMinSketch::density() const {
	size_t zeros = 0;
	for (size_t i = 0; i < w; i++) {
		bool zero = true;
		for (size_t j = 0; j < d; j++) {
			if (table[j][i] != 0) {
				zero = false;
				break;
			}
		}
		if (zero)
			++zeros;
	}
	return 1.0 - (double) zeros / (double) w;
}


std::vector< std::vector<uint_t> > CountMinSketch::histogram(uint_t n) const {
	std::vector< std::vector<uint_t> > response(d);
	uint_t val;
	for (unsigned int i = 0; i < d; i++) {
		response[i].resize(n+1, 0);
		for (unsigned int j = 0; j < w; j++) {
			val = std::min(table[i][j], n+1);
			if (val > 0)
				++response[i][val-1];
		}
	}
	return response;
}


