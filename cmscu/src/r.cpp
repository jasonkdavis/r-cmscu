#include <Rcpp.h>
// [[Rcpp::plugins(openmp)]

#include "cmscu.h"

using namespace std;

bool get_str_valid(SEXP* args, int nargs ) {
	return (nargs == 1) && (TYPEOF(args[0]) == STRSXP) && LENGTH(args[0]) == 1;
}

RCPP_EXPOSED_CLASS(CountMinSketch)

RCPP_MODULE(cmscu) {
	Rcpp::class_< CountMinSketch >("FrequencyDictionary")
		.constructor<unsigned int,unsigned int>()
		.method("query", (uint_t         (CountMinSketch::*)(const string&)              const)(&CountMinSketch::query), "Query a single string.", &get_str_valid)
		.method("query", (vector<uint_t> (CountMinSketch::*)(const vector<string>&)      const)(&CountMinSketch::query), "Query a list of strings.")
		.method("query", (vector<uint_t> (CountMinSketch::*)(const vector<string>&, int) const)(&CountMinSketch::query), "Query a list of strings in parallel.")

		.method("store", (uint_t         (CountMinSketch::*)(const string&)        )(&CountMinSketch::store), "Store a single string.", &get_str_valid)
		.method("store", (vector<uint_t> (CountMinSketch::*)(const vector<string>&))(&CountMinSketch::store), "Store a list of strings.")

		.method("save", &CountMinSketch::save,       "Write the dictionary to a file.")
		.method("read", &CountMinSketch::read,       "Read the dictionary from a file.")

		.method("histogram", &CountMinSketch::histogram, "Returns the number of words that appears 1, 2, ..., n, and >n  times.")

		.property("entries",        &CountMinSketch::entries,       "The number of entries that have been stored in the dictionary.")
		.property("unique_entries", &CountMinSketch::uniqueEntries, "The number of unique entries that have been stored in the dictionary.")
		.property("uncertainty",    &CountMinSketch::uncertainty,   "The amount that a frequency may be over-estimated by with some confidence.")
		.property("confidence",     &CountMinSketch::confidence,    "The confidence that we have in our uncertainty.")
		.property("density",        &CountMinSketch::density,       "An approximation to how \"dense\" the storage is used. This should be closer to 0 than 1.");
};

