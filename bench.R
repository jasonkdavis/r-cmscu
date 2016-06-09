require(rbenchmark);
require(rjson);

require(tm);
require(cmscu);

unigrams <- function(line) {
  # upper-case everything
  str <- toupper(line);
  # strip-out small html tags
  str <- gsub('<[^>]{1,2}>', '', str);
  # replace all terminal punctuation with a period
  str <- gsub('[[:space:]]*[.?!:;]+[[:space:]]*', ' ', str);
  # get rid of anything not A-Z, ', or whitespace
  str <- gsub('[^A-Z\'[:space:]]', ' ', str);
  # make sure contraction's are "tight"
  str <- gsub("[[:space:]]?'[[:space:]]?", "'", str);
  # split on whitespace
  return(unlist(strsplit(str, "[[:space:]]+", " ")));
}

test <- function(size, repls) {


srcFile <- file('yelp_academic_dataset_review.json', 'r', FALSE);
srcData <- sapply(readLines(con=srcFile, n=size), function(line) unigrams(fromJSON(line)$text));
close(srcFile);
names(srcData) <- NULL;
tmData <- sapply(srcData, function(doc) paste(doc, collapse=' '), USE.NAMES=FALSE);


afdRun <- function() {
  # allocate a fixed 1GB for the dictionary
  dict <- new(FrequencyDictionary, 4, 2^24);
  invisible(
    lapply(srcData, function(lst) dict$store(lst))
  );

  return(sapply(srcData, function(lst) -mean(log2(dict$query(lst))), USE.NAMES=FALSE));
}


# tm's time to shine
tmRun <- function() {
  corpus <- Corpus(VectorSource(tmData));
  dtm <- as.matrix(DocumentTermMatrix(corpus, control=list(wordLengths=c(1,Inf))));
  mults <- log2(colSums(dtm));
  return(tmOutput <- apply(dtm, 1, function(row) {
    return(-sum(row*mults)/sum(row));
  }));
}


adata <- afdRun();
tdata <- tmRun();
print(sqrt(mean((adata-tdata)^2)));

return(benchmark(afdRun(), tmRun(), replications=repls));
}



