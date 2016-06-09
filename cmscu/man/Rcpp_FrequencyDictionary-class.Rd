\name{Rcpp_FrequencyDictionary-class}
\alias{Rcpp_FrequencyDictionary-class}
\alias{FrequencyDictionary-class}
\docType{class}

\title{Class \code{"FrequencyDictionary"}}
\description{
The count-min-sketch is a probabilistic algorithm for computing the approximate frequency of strings in a space- and computationally-efficient manner. (See Cormode and Muthukrishnan, "An improved data stream summary: the count-min sketch and its applications.") Note: hashing is provided by the public domain MurmurHash3 implementation (https://github.com/aappleby/smhasher).
}
\usage{
cms <- new(FrequencyDictionary, hashes, entries);
}
\arguments{
  \item{hashes}{The number of hash functions to use. The more hash functions, the more confidence we have in the estimates. 4 is a good baseline number.}
  \item{entries}{The number of entries per hash function. The more entries, the smaller our uncertainty in the estimates.}
}

\references{
https://github.com/jasonkdavis/r-cmscu
}
\author{
Jason K Davis <jkd@jasonkarldavis.com> [aut,cre],
David W Vinson <dave@davevinson.com> [aut]
Maintainer: Jason K Davis
}

\examples{
require(cmscu);

# use 4 hash functions and 1024 entries per hash
cms <- new(FrequencyDictionary, 4, 2^10);

# store some strings
cms$store(c("hello", "hello", "world"));

# query the frequencies
freqs <- cms$query(c("hello", "goodbye"));

# how certain are we in our confidence interval?
100*cms$confidence

# how much might we be over-estimating the frequency by?
cms$uncertainty
}

\section{Fields}{
  \describe{
    \item{\code{confidence}:}{The probability that we are not over-estimating the frequency by more than \code{uncertainty}. Ideally close to 1.}
    \item{\code{uncertainty}:}{The uncertainty in our frequency estimates, ideally less than 1.}
    \item{\code{density}:}{How full the count-min-sketch tables are. The closer to 1 this is, the more likely collisions are occurring.}
    \item{\code{entries}:}{The number of items that have been stored in the count-min-sketch.}
    \item{\code{unique_entries}:}{The number of unique items that have been stored in the count-min-sketch.}
  }
}

\section{Methods}{
  \describe{
    \item{\code{store(str)}:}{\code{str} is either a string or a vector of strings. This method increments the corresponding counts for each string and returns a vector of the new counts.}
    \item{\code{save(str)}:}{\code{str} is a string representing a filename. This method writes the binary representation of the count-min-sketch to a file, for later reuse.}
    \item{\code{read(str)}:}{\code{str} is a string representing a filename. This method reads the binary representation of the count-min-sketch from a file.}
    \item{\code{query(str, n=1)}:}{\code{str} is either a string or a vector of strings. This method queries the frequencies for each string and returns the vector of counts. If \code{n}>1 and the package was built in an environment supporting OpenMP, this method will use \code{n} threads to perform the query.}
    \item{\code{histogram(n)}:}{\code{n} is a positive integer. This method returns a list of vectors of length \code{n+1}; each item of the list corresponds to a different hash function, and its entries the number of 1-counts, 2-counts, ..., \code{n}-counts, and the number of all entries exceeding \code{n}.}
  }
}
