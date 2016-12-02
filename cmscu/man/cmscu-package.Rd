\name{cmscu-package}
\alias{cmscu-package}
\alias{cmscu}
\alias{FrequencyDictionary}

\docType{package}

\title{A Count-Min-Sketch with Conservative Update implementation for R}
\description{The count-min-sketch is a probabilistic algorithm for computing the approximate frequency of strings in a space-efficient and computationally-efficient manner. (See Cormode and Muthukrishnan, "An improved data stream summary: the count-min sketch and its applications.") Note: hashing is provided by the public domain MurmurHash3 implementation (https://github.com/aappleby/smhasher). See the class documentation for methods, usage, etc.}


\details{
\tabular{ll}{
Package: \tab cmscu\cr
Type: \tab Package\cr
Version: \tab 1.0\cr
Date: \tab 2016-05-23\cr
License: \tab BSD_3_clause\cr
}}

\seealso{
  \code{\link{Rcpp_FrequencyDictionary-class}}.
}

\author{
Jason K Davis <jkd@jasonkarldavis.com> [aut,cre],
David W Vinson <dave@davevinson.com> [aut],
Maintainer: Jason K Davis
}
\references{
http://link.springer.com/article/10.3758/s13428-016-0766-5,
https://github.com/jasonkdavis/r-cmscu
}

