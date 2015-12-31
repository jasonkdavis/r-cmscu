# r-cmscu
An R source package implementing the Count-Min-Sketch with conservative updating.

# Sample Usage:
    require(cmscu);
    unigrams <- unlist(strsplit("hello world we are training a dictionary hello world", " ", fixed=TRUE));
    num <- length(unigrams);
    
    dict <- new(FrequencyDictionary, 4, 10^4);
    dict$parallel_store(1, unigrams); # note: using more than 1 thread is subject to race conditions at the moment
    # alternatively: sapply(unigrams, dict$store)
    
    test <- unlist(strsplit("hello world goodbye training", " ", fixed=TRUE));
    
    # average unigram information
    aui <- -mean(log2(dict$parallel_query(4, test) / num));
    # alternatively, -mean(log2(sapply(test, dict$query) / num))
    
