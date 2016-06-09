#ifndef CMSCU_H
#define CMSCU_H

#include <string>
#include <vector>


typedef unsigned int uint_t;

class CountMinSketch {
	private:
		unsigned int d; // the number of hash functions
		unsigned int w; // the width of each row
		uint_t count, ucount; // the total and unique counts of items in this data structure

		std::vector< std::vector<uint_t> > table;


	public:
		CountMinSketch(unsigned int, unsigned int);
		uint_t query(const std::string&) const;
		std::vector<uint_t> query(const std::vector<std::string>&) const;
		std::vector<uint_t> query(const std::vector<std::string>&, int) const;

		uint_t store(const std::string&);
		std::vector<uint_t> store(const std::vector<std::string>&);

		uint_t entries() const;
		uint_t uniqueEntries() const;
		double uncertainty() const;
		double confidence() const;

		bool save(const std::string&) const;
		bool read(const std::string&);
		double density() const;

		std::vector< std::vector<uint_t> > histogram(uint_t) const;
};

#endif

