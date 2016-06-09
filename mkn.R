require(cmscu);

# helper function
ngrams_h <- function(lst, n) {
    lst2 <- c(rep('@', n), lst, '#'); # this gets the BOS and EOS tokens inserted
    len <- length(lst2);
    sapply(1:(len-n+1), function(i) do.call(paste, as.list(lst2[i:(i+n-1)])))
}


# class description of modified Kneser-Ney:
# We store the frequencies of each n-ngram, and separately keep
# track of instances where completions or prefixing occurs.
mkn <- function(n, d, w)
    structure(list(
        # the n in the n-gram, i.e. if we're studying up to bigrams this is 2
        order = n,

        # this is to count the number of times a particular ngram is encountered
        frequencies = replicate(n, new(FrequencyDictionary, d, w)),

        # this is for the unigram estimation only
        completions = new(FrequencyDictionary, d, w),

        # this is for the magic of modified Kneser-Ney:
        # prefixes[[i]][[j]]$query(word) >= prefixes[[i+1]][[j]]$query(word)
        # for each (i,j)
        prefixes = lapply(1:3, function(i) lapply(2:n, function(j) new(FrequencyDictionary, d, w)))), class="mkn.training");


# We define a function `train` that takes an ordered list of unigram tokens, then
# maps them to the ngrams and fills in the data structures accordingly
train <- function(x, ...) UseMethod("train");
train.mkn.training <- function(obj, data) {
    # Parse and store all the ngrams
    grams <- lapply(1:obj$order, function(i) {
        tokens <- ngrams_h(data, i);
        counts <- obj$frequencies[[i]]$store(tokens);
        return(list(tokens = tokens, indices = lapply(1:3, function(j) counts == j)));
    });

    # for unigram, we figure out bigram completions
    obj$completions$store(grams[[1]]$tokens[grams[[2]]$indices[[1]]]);

    # now we figure out the prefixes for the N quantities from chen
    for (i in 2:obj$order) {
        # if the kth time we see the n-gram, increment the prefixing n-1 gram
        for (j in 1:3) {
            prefixes <- grams[[i - 1]]$tokens[c(tail(grams[[i]]$indices[[j]], -1), FALSE)];
            if (length(prefixes) > 0) {
                obj$prefixes[[j]][[i - 1]]$store(prefixes);
            }
        }
    }
};


finalize <- function(x, ...) UseMethod("finalize");
finalize.mkn.training <- function(obj) {
    # chen paper has n1 through n4, which is different for each n (as in n-gram)
    n <- sapply(1:obj$order, function(i) {
        # the count min sketch has a histogram function which returns the number of items
        # with count 1, count 2, ..., count n, and then the number with a count exceeding n.
        # we grab this, but note that each "row" might have a slightly different prediction for each count
        distribution <- matrix(unlist(obj$frequencies[[i]]$histogram(4), use.names = FALSE), ncol = 5, byrow = TRUE);

        # thus we ignore this uncertainty by taking a consensus, e.g. the median of each column
        result <- apply(distribution, 2, median);
        # finally, we fix it up for the BOS nonsense
        bos <- do.call(paste, as.list(rep('@', i)));
        k <- min(5, obj$frequencies[[i]]$query(bos));
        result[k] <- result[k] - 1;
        return(result);
        });
    
    # from the chen paper exactly (noting that Y, D1, ..., D3 are actually vectors corresponding to their values for each level of n in n-gram)
    Y <- n[1,] / (n[1,] + 2 * n[2,]);

    D <- matrix(0, nrow=obj$order, ncol=4);
    D[,2] <- 1 - 2 * Y * n[2,] / n[1,];
    D[,3] <- 2 - 3 * Y * n[3,] / n[2,];
    D[,4] <- 3 - 4 * Y * n[4,] / n[3,];
    D[is.na(D)] <- 0;

    # The big-N are the "number of words that have exactly k counts" (except, for k=3, at least 3 counts, rather than exactly 3 counts)
    # Since the prefixes are at least 1, at least 2, and at least 3, we take consecutive differences to fix
    N1 <- obj$prefixes[[1]];
    N2 <- obj$prefixes[[2]];
    N3 <- obj$prefixes[[3]];

    p0 <- 1 / (obj$frequencies[[1]]$unique_entries - 1);
    p1 <- 1 / (obj$frequencies[[2]]$unique_entries - 1);

    # this is it: the big function
    p <- function(tokens) {
        pmkn <- vector("list", obj$order);

        # unigrams is a special case
        grams     <- ngrams_h(tokens, 1);
        counts    <- obj$frequencies[[1]]$query(grams);
        pmkn[[1]] <- ifelse(counts == 0, p0, p1 * obj$completions$query(grams))[-1];

        # for each higher order n-gram
        for (n in 2:obj$order) {

            # here we compute back-off probabilities
            a <- N1[[n - 1]]$query(grams);
            b <- N2[[n - 1]]$query(grams);
            c <- N3[[n - 1]]$query(grams);
            gamma <- head((D[n, 2] * pmax(a - b, 0) + D[n, 3] * pmax(b - c, 0) + D[n, 4] * c) / counts,-1);

            # now we update
            oldcounts <- head(counts,-1);
            grams <- ngrams_h(tokens, n);
            counts <- obj$frequencies[[n]]$query(grams);

            # and that's it!
            pmkn[[n]] <- tail(counts - D[n, 1 + pmin(3, counts)], -1) / oldcounts + gamma * pmkn[[n - 1]];

            backoff <- is.na(pmkn[[n]]) | pmkn[[n]] == 0;
            pmkn[[n]][backoff] <- pmkn[[n-1]][backoff];
        }

        return(pmkn);
    }

    return(p);
}
