// My humble apologies: in a rush to get this package distributed, I
// was having trouble with Rcpp trying to separately compile all of
// the source files... the "easy" (and awful) fix was to combine them
// all into 1. The actual CMS-CU code starts on line 386.
// Fixing this should be one of my next commits.


//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER) && (_MSC_VER < 1600)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;

// Other compilers

#else	// defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32  ( const void * key, int len, uint32_t seed, void * out );

void MurmurHash3_x86_128 ( const void * key, int len, uint32_t seed, void * out );

void MurmurHash3_x64_128 ( const void * key, int len, uint32_t seed, void * out );

//-----------------------------------------------------------------------------

#endif // _MURMURHASH3_H_
//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

//#include "MurmurHash3.h"

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)

#define FORCE_INLINE	__forceinline

#include <stdlib.h>

#define ROTL32(x,y)	_rotl(x,y)
#define ROTL64(x,y)	_rotl64(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else	// defined(_MSC_VER)

#define	FORCE_INLINE inline __attribute__((always_inline))

inline uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

FORCE_INLINE uint32_t getblock32 ( const uint32_t * p, int i )
{
  return p[i];
}

FORCE_INLINE uint64_t getblock64 ( const uint64_t * p, int i )
{
  return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

FORCE_INLINE uint32_t fmix32 ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//----------

FORCE_INLINE uint64_t fmix64 ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32 ( const void * key, int len,
                          uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 4;

  uint32_t h1 = seed;

  const uint32_t c1 = 0xcc9e2d51;
  const uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

  for(int i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock32(blocks,i);

    k1 *= c1;
    k1 = ROTL32(k1,15);
    k1 *= c2;
    
    h1 ^= k1;
    h1 = ROTL32(h1,13); 
    h1 = h1*5+0xe6546b64;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

  uint32_t k1 = 0;

  switch(len & 3)
  {
  case 3: k1 ^= tail[2] << 16;
  case 2: k1 ^= tail[1] << 8;
  case 1: k1 ^= tail[0];
          k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len;

  h1 = fmix32(h1);

  *(uint32_t*)out = h1;
} 

//-----------------------------------------------------------------------------

void MurmurHash3_x86_128 ( const void * key, const int len,
                           uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;

  uint32_t h1 = seed;
  uint32_t h2 = seed;
  uint32_t h3 = seed;
  uint32_t h4 = seed;

  const uint32_t c1 = 0x239b961b; 
  const uint32_t c2 = 0xab0e9789;
  const uint32_t c3 = 0x38b34ae5; 
  const uint32_t c4 = 0xa1e38b93;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);

  for(int i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock32(blocks,i*4+0);
    uint32_t k2 = getblock32(blocks,i*4+1);
    uint32_t k3 = getblock32(blocks,i*4+2);
    uint32_t k4 = getblock32(blocks,i*4+3);

    k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;

    h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;

    k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

    h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;

    k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

    h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;

    k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

    h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint32_t k1 = 0;
  uint32_t k2 = 0;
  uint32_t k3 = 0;
  uint32_t k4 = 0;

  switch(len & 15)
  {
  case 15: k4 ^= tail[14] << 16;
  case 14: k4 ^= tail[13] << 8;
  case 13: k4 ^= tail[12] << 0;
           k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

  case 12: k3 ^= tail[11] << 24;
  case 11: k3 ^= tail[10] << 16;
  case 10: k3 ^= tail[ 9] << 8;
  case  9: k3 ^= tail[ 8] << 0;
           k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

  case  8: k2 ^= tail[ 7] << 24;
  case  7: k2 ^= tail[ 6] << 16;
  case  6: k2 ^= tail[ 5] << 8;
  case  5: k2 ^= tail[ 4] << 0;
           k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

  case  4: k1 ^= tail[ 3] << 24;
  case  3: k1 ^= tail[ 2] << 16;
  case  2: k1 ^= tail[ 1] << 8;
  case  1: k1 ^= tail[ 0] << 0;
           k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  h1 = fmix32(h1);
  h2 = fmix32(h2);
  h3 = fmix32(h3);
  h4 = fmix32(h4);

  h1 += h2; h1 += h3; h1 += h4;
  h2 += h1; h3 += h1; h4 += h1;

  ((uint32_t*)out)[0] = h1;
  ((uint32_t*)out)[1] = h2;
  ((uint32_t*)out)[2] = h3;
  ((uint32_t*)out)[3] = h4;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x64_128 ( const void * key, const int len,
                           const uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;

  uint64_t h1 = seed;
  uint64_t h2 = seed;

  const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
  const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

  //----------
  // body

  const uint64_t * blocks = (const uint64_t *)(data);

  for(int i = 0; i < nblocks; i++)
  {
    uint64_t k1 = getblock64(blocks,i*2+0);
    uint64_t k2 = getblock64(blocks,i*2+1);

    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch(len & 15)
  {
  case 15: k2 ^= ((uint64_t)tail[14]) << 48;
  case 14: k2 ^= ((uint64_t)tail[13]) << 40;
  case 13: k2 ^= ((uint64_t)tail[12]) << 32;
  case 12: k2 ^= ((uint64_t)tail[11]) << 24;
  case 11: k2 ^= ((uint64_t)tail[10]) << 16;
  case 10: k2 ^= ((uint64_t)tail[ 9]) << 8;
  case  9: k2 ^= ((uint64_t)tail[ 8]) << 0;
           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

  case  8: k1 ^= ((uint64_t)tail[ 7]) << 56;
  case  7: k1 ^= ((uint64_t)tail[ 6]) << 48;
  case  6: k1 ^= ((uint64_t)tail[ 5]) << 40;
  case  5: k1 ^= ((uint64_t)tail[ 4]) << 32;
  case  4: k1 ^= ((uint64_t)tail[ 3]) << 24;
  case  3: k1 ^= ((uint64_t)tail[ 2]) << 16;
  case  2: k1 ^= ((uint64_t)tail[ 1]) << 8;
  case  1: k1 ^= ((uint64_t)tail[ 0]) << 0;
           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  ((uint64_t*)out)[0] = h1;
  ((uint64_t*)out)[1] = h2;
}

//-----------------------------------------------------------------------------






#ifdef _OPENMP
#include <omp.h>
#endif

#include <Rcpp.h>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
//#include "MurmurHash3.cpp"

using namespace std;

template <class uint>
class CountMinSketch {
	private:
		size_t d; // the number of hash functions
		size_t w; // the width of each row

		uint l1;

		vector< vector<uint> > table;
	public:
		CountMinSketch(size_t dd, size_t ww) : d(dd), w(ww), l1(0) {
			table.resize(d);
			for (size_t i = 0; i < d; i++)
				table[i].resize(w, 0);
		}

		uint store(const string& str) {
			++l1;
			const char* bytes = str.c_str();
			unsigned int len = (unsigned int) str.length();

#if UINTPTR_MAX == 0xffffffffffffffff
			size_t hash[] = { 0, 0 };
			MurmurHash3_x64_128(bytes, len, 0, hash);
#elif UINTPTR_MAX == 0xffffffff
			size_t hash[] = { 0, 0, 0, 0 };
			MurmurHash3_x86_128(bytes, len, 0, hash);
#else
			size_t hash[] = { 0, 0 };
			throw "Unknown pointer size";
#endif
			size_t index = hash[0];
			uint c = table[0][index % w];
			for (size_t i = 1; i < d; i++) {
				index += hash[1];
				c = min(c, table[i][index % w]);
			}
			++c;
			index = hash[0];
			for (size_t i = 0; i < d; i++) {
				uint* val = &(table[i][index % w]);
				index += hash[1];
				if (c > *val)
					*val = c;
			}
			return c;
		}
		uint query(const string& str) const {
			const char* bytes = str.c_str();
			unsigned int len = (unsigned int) str.length();

#if UINTPTR_MAX == 0xffffffffffffffff
			size_t hash[] = { 0, 0 };
			MurmurHash3_x64_128(bytes, len, 0, hash);
#elif UINTPTR_MAX == 0xffffffff
			size_t hash[] = { 0, 0, 0, 0 };
			MurmurHash3_x86_128(bytes, len, 0, hash);
#else
			size_t hash[] = { 0, 0 };
			throw "Unknown pointer size";
#endif
			size_t index = hash[0];
			uint c = table[0][index % w];
			for (size_t i = 1; i < d; i++) {
				index += hash[1];
				c = min(c, table[i][(hash[0] + i*hash[1]) % w]);
			}

			return c;
		}
		uint numEntries() const {
			return l1;
		}
		double uncertainty() const {
			return 2.718281828459 / (double) w * (double) l1;
		}
		double confidence() const {
			return 1.0 - exp(-(double) d);
		}

		bool save(const string& file) {
			ofstream output(file.c_str(), ofstream::binary);
			if (output.is_open()) {
				for (size_t i = 0; i < d; i++) {
					output.write((char*) &(table[i][0]), w * sizeof(uint));
				}
				output.write((char*) &l1, sizeof(uint));
				output.close();
				return true;
			}
			return false;
		}
		bool read(const string& file) {
			ifstream input(file.c_str(), ifstream::binary);
			if (input.is_open()) {
				for (size_t i = 0; i < d; i++) {
					input.read((char*) &(table[i][0]), w*sizeof(uint));
				}
				input.read((char*) &l1, sizeof(uint));
				input.close();
				return true;
			}
			return false;
		}
		double density() const {
			size_t zeros = 0;
			for (size_t i = 0; i < w; i++) {
				size_t j = 0;
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
			return (double) zeros / (double) w;
		}

		vector<uint> parallel_query(int cores, const vector<string>& words) const {
#ifdef _OPENMP
			omp_set_num_threads(cores);
#endif
			const size_t len = words.size();
			vector<uint> response(len);
			#pragma omp parallel for
			for (size_t i = 0; i < len; i++)
				response[i] = query(words[i]);
			return response;
		}
		void parallel_store(int cores, const vector<string>& words) {
#ifdef _OPENMP
			omp_set_num_threads(cores);
#endif
			const size_t len = words.size();
			#pragma omp parallel for
			for (size_t i = 0; i < len; i++)
				store(words[i]);
		}
};

RCPP_MODULE(cmscu) {
        Rcpp::class_< CountMinSketch<unsigned int> >("FrequencyDictionary")
                .constructor<size_t,size_t>()
                .method("query", &CountMinSketch<unsigned int>::query, "Compute an approximate empirical frequency.")
                .method("store", &CountMinSketch<unsigned int>::store, "Store a single string.")
		.method("uncertainty", &CountMinSketch<unsigned int>::uncertainty, "The amount that a frequency may be over-estimated by with some confidence.")
		.method("confidence",  &CountMinSketch<unsigned int>::confidence, "The confidence that we have in our uncertainty.")
		.method("save", &CountMinSketch<unsigned int>::save, "Write the dictionary to a file.")
		.method("read", &CountMinSketch<unsigned int>::read, "Read the dictionary from a file.")
		.method("density", &CountMinSketch<unsigned int>::density, "An approximation to how \"dense\" the storage is used. This should be small.")
		.method("parallel_query", &CountMinSketch<unsigned int>::parallel_query, "Query a list of strings simultaneously.")
		.method("parallel_store", &CountMinSketch<unsigned int>::parallel_store, "Store a list of strings simultaneously.")
		.method("numEntries", &CountMinSketch<unsigned int>::numEntries, "Query the number of entries that have been stored in the dictionary.");
};
        

