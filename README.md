# r-cmscu
An R source package implementing the Count-Min-Sketch with conservative updating.

# Source Distribution
    The tarball cmscu.tar.gz in this repository contains the latest packaged source. It can be installed via RStudio or `R CMD INSTALL cmscu.tar.gz` from the command line. Note: the package `Rcpp` is required before installation, e.g. `install.packages('Rcpp')`.

# Sample Usage:
    require(cmscu);
    unigrams <- unlist(strsplit("hello world we are training a dictionary hello world", " ", fixed=TRUE));
    num <- length(unigrams);
    
    dict <- new(FrequencyDictionary, 4, 10^4);
    dict$parallel_store(1, unigrams); # note: using more than 1 thread is subject to race conditions at the moment
    # alternatively: sapply(unigrams, dict$store)
    
    test <- unlist(strsplit("hello world goodbye training", " ", fixed=TRUE));
    
    # average unigram information with smoothing
    frequencies <- dict$parallel_query(4, test);
    normalization <- log2(num + 0.5*length(unique(unigrams)));
    aui <- -mean(log2(0.5+frequencies)) + normalization;
    # alternatively, -mean(log2(0.5+sapply(test, dict$query))) + normalization
    
