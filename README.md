# r-cmscu
An R source package implementing the Count-Min-Sketch with conservative updating.

# Source Distribution
The tarball cmscu.tar.gz in this repository contains the latest packaged source. It can be installed via RStudio or `R CMD INSTALL cmscu.tar.gz` from the command line. Note: the package `Rcpp` is required before installation, e.g. `install.packages('Rcpp')`.

Further note: the parallel features require OpenMP support in your build environment. Typically this means that the parallel functions won't actually run in parallel unless installed in a Linux environment. Nevertheless, it is still considerably faster to run `dict$query(list_of_strings)` than `sapply(list_of_strings, dict$query)` for large lists.

# Sample Usage:
	require(cmscu);
	require(tm);
	require(rjson);

	# read in the official Yelp dataset review file
	yelp <- file("yelp_academic_dataset_review.json", "r", FALSE);
	training <- sapply(readLines(con = yelp, n = 1000), function(review) fromJSON(review)$text, USE.NAMES = FALSE);
	testing  <- sapply(readLines(con = yelp, n = 1000), function(review) fromJSON(review)$text, USE.NAMES = FALSE);

	# use the tm package to clean up some text from the Yelp review dataset
	clean_data <- function(data) {
	    tcorpus <- VCorpus(VectorSource(data));
	    tcorpus <- tm_map(tcorpus, stripWhitespace);
	    tcorpus <- tm_map(tcorpus, content_transformer(tolower));
	    tcorpus <- tm_map(tcorpus, removePunctuation);
	    tcorpus <- tm_map(tcorpus, stemDocument);
	    return(tcorpus);
	}

	# instead of using a DocumentTermMatrix from tm, we use the cmscu package
	dict <- new(FrequencyDictionary, 4, 2 ^ 20); # this uses 16mb of RAM
	invisible(sapply(clean_data(training), function(review) {
	    words <- unlist(strsplit(as.character(review), " ", fixed = TRUE), use.names = FALSE);
	    bigrams <- do.call(paste, list(head(words, -1), tail(words, -1)));
	    dict$store(bigrams);
	}));

	# some diagnostics:
	#   we hope dict$uncertainty is small (less than 1, or whatever your application requires)
	#   this is an upper bound on our over-estimation, dict$confidence% of the time.
	if (dict$uncertainty >= 1) {
	    print("Consider increasing the number of entries per hash table in the dictionary");
	}
	print(c(dict$uncertainty, dict$confidence));

	# finally, let's look at the distribution of mean information density across 1000 reviews
	normalization <- log2(dict$entries + 0.5 * dict$unique_entries);
	info <- sapply(clean_data(testing), function(review) {
	    words <- unlist(strsplit(as.character(review), " ", fixed = TRUE), use.names = FALSE);
	    bigrams <- do.call(paste, list(head(words, -1), tail(words, -1)));

	    # Use Dirichlet smoothing to avoid divide-by-zero problems
	    return(-mean(log2(dict$query(bigrams) + 0.5) - normalization));
	}, USE.NAMES = FALSE);

	# plot the histogram
	hist(info);

# Extras
The file mkn.R contains an implementation of the modified Kneser-Ney smoothing algorithm powered by the cmscu library. It is still undergoing development and documentation, so use at your own risk! With that said,

	# initialize the smoothing for ngrams of up to order 3
	# (pass the other arguments to each instance of cmscu objects,
	#  of which there are 2*n, where n is the order of the ngram)
	smoother <- mkn(3, 4, 2^20);
	# sentence is a character vector of words
	train(smoother, a_sentence);
	# train can be called many, many times
	train(smoother, another_sentence);
	# finalize when there is no more data
	smoother <- finalize(smoother);
	# smoother is now a function which takes a character vector
	# of trigram tokens, and returns their probabilities
	# (note: this includes all suffixing-ngram probabilities)

