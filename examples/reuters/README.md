# Example 2: Text Categorization

  This example deals with categorizing texts of different topics. We make
  use of Harry for computing _string kernels_, which enable us to apply
  [Support Vector
  Machines](http://en.wikipedia.org/wiki/Support_vector_machine) (SVM) for
  the categorization of the text documents.

## Requirements and Files

  To run this example, you first need to download and install
  [LibSVM](http://www.csie.ntu.edu.tw/~cjlin/libsvm/), a simple yet
  efficient implementation of SVMs.

  All files needed for the example are provided in this directory The file
  [`reuters.zip`](reuters.zip) contains 400 news articles from 4 different
  topics, where the filename of each article indicates the topic.  The
  articles are part of the well-known Reuters corpus.  The configuration for
  Harry is given in the file [`harry.cfg`](harry.cfg).

  Make sure that Harry supports reading archives by providing the switch
  `--enable-libarchive` to `configure` before compiling the source code of
  Harry.  If you cannot enable support for reading archives, you need to
  unpack the dataset and use the input format `dir` instead of `arc` for
  this example.

## Configuring Harry

  In contrast to the first example, we make use of the configuration file
  [`harry.cfg`](harry.cfg) for telling Harry what to do.  This enables us to
  repeatedly use the same configuration and just change some parameters on
  the command-line.

  First, we set the input format in the `input` section of the configuration
  file to `arc`.  This instructs Harry to read from an archive, where each
  file contained in the archive is treated as an individual string.

      input_format = "arc";

  Second, we tell Harry to consider tokens instead of bytes for comparing
  the strings.  We do this by setting the parameter `granularity` and
  `token_delim` in the `measures` section.  In particular, we choose a set
  of delimiter bytes that are used to partition a string into tokens.

      granularity = "tokens";
      token_delim = "%09%0a%0d%20,.;:?!";

  Finally, we configure three kernel functions for strings. Simply put, a
  kernel function is a special form of a similarity measure that does not
  only assess the similarity of two objects but also satisfies certain
  mathematical properties.

  1. We configure the _spectrum kernel_ (or _n_-gram kernel)
     first.  This kernel extracts groups of _n_ consecutive tokens from
     the strings and uses these for comparison.  We set _n = 2_ and
     enable a normalization of the kernel.

          kern_spectrum = {
                  length = 2;
                  norm = "l2";
          }

  2. We then configure the _subsequence kernel_ that also considers
     groups of _n_ tokens but allows gaps between them.  We again set _n =
     2_, choose a penalty weight for gaps (default is 0.1) and enable a
     normalization of the kernel.

          kern_subsequence = {
                  length = 2;
                  lambda = 0.1;
                  norm = "l2";
          }

  3. We finally configure a distance substitution kernel &ndash; a kernel
     function that is computed from a distance and thus allows plugging in
     all sorts of string distances.  In our example, we choose the
     Levenshtein distance and use a linear mapping to a kernel as follows.

          kern_distance = {
                  dist = "dist_levenshtein";
                  type = "linear";
                  norm = "l2";
          }

  4. Last but not least, we set the output format to `libsvm` in the
     `output` section.  This ensures that LibSVM can directly read the files
     created by Harry.

          output_format = "libsvm";

## Computing Kernel Matrices

  With the specified configuration, we are finally ready to apply Harry to
  the new articles in the archive [`reuters.zip`](reuters.zip).  To this
  end, we compute kernel matrices that contain the pairwise kernel values
  between all 400 new articles for the three different kernels.  On the
  command-line we can do this as follows:

      harry -c harry.cfg -v -m kern_spectrum reuters.zip kern1.txt
      harry -c harry.cfg -v -m kern_subsequence reuters.zip kern2.txt
      harry -c harry.cfg -v -m kern_distance reuters.zip kern3.txt

  Note that the run-time time of Harry scales quadratically with the number
  of strings and some kernels take longer to compute than others.  Although
  Harry makes use of OpenMP and efficient code to speed up the computation,
  calculating kernel matrices is only possible for small and medium-sized
  datasets.  For large-scale data, you may have a look at the analysis tool
  [Sally](http://www.mlsec.org/sally) that takes a different approach to
  analyzing strings.

## Training a Support Vector Machine

  Given the three kernel matrices, we can finally apply LibSVM to learn a
  classification of the four different topics.  For simplicity we conduct a
  5-fold cross-validation, where the SVM is learned on 4/5 of the data and
  tested on the remaining 1/5.  This procedure is repeated 5 times with
  different folds and the accuracy is averaged.

      svm-train -q -v 5 -t 4 kern1.txt
      svm-train -q -v 5 -t 4 kern2.txt
      svm-train -q -v 5 -t 4 kern3.txt

  The switch `-v 5` enables the cross-validation and the option `-t 4`
  instructs LibSVM to use the kernel matrices for training and testing.  The
  output should look like this.

     Cross Validation Accuracy = 89.5%
     Cross Validation Accuracy = 90%
     Cross Validation Accuracy = 88.25%

  All three kernels enable a good discrimination of the four categories with
  an accuracy between 88% and 90%.  Note that a random classification would
  yield only 25% accuracy since four classes need to be discriminated.

## More Kernels

  To get a feeling for the different kernels and their parameters, simply
  run the example again and change the configuration.  For example, you can
  change the number of tokens considered by the spectrum and subsequence
  kernel.  Similarly, you can plug other distances into the distance
  substitution kernel and check the results.
