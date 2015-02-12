# A Small Tutorial

  This document provides a small tutorial for using Harry and its options.
  We will use the following text file as input data: [`data.txt`](data.txt).
  The file contains only four lines and thus we are looking at only four
  different strings in this tutorial.

## Computing a Similarity Matrix

  Let's start by simply computing a similarity matrix for the four strings.
  The default similarity measure implemented in Harry is the Levenshtein
  distance (edit distance).  We thus get a 4x4 matrix of distance values, if
  we run:

      harry data.txt -

  Harry will print the matrix to standard out (stdout), since no output file
  is selected.  Alternatively, we can write the matrix to a file by running:

      harry data.txt matrix.txt

## Different Similarity Measures

  The Levenshtein distance is only one of many similarity measures for
  strings.  To see a list of similarity measures supported by Harry, run
  the following command.

      harry -M

  Note that distances start with `dist`, while kernel functions and
  similarity coefficients are prefixed by `kern` and `sim`, respectively.
  The latter two compute the similarity of two strings, that is, the
  returned value increases with the similarity of the strings.  By contrast,
  the distances compute the dissimilarity of two strings and thus the
  returned value decreases with the similarity of the strings.  Let's have
  some fun and compute a couple of different similarity measures:

      harry -m dist_hamming data.txt -
      harry -m dist_jaro data.txt -
      harry -m kern_spectrum data.txt -
      harry -m sim_jaccard data.txt -

  Each of the similarity measures emphasizes different aspects of the
  strings. Just have a look at Wikipedia to learn a little bit about how
  these are computed.

## Characters and Words

  By default Harry considers a string as a sequence of bytes. The
  similarity of two strings is thus determined by comparing the order and
  distribution of bytes in the strings.  However, in many settings it
  is not the actual bytes of a string that are relevant but the tokens
  (or words) contained in the string.

  Harry supports computing the similarity measures based on tokens by
  partitioning strings using a set of delimiter bytes.  In this setting
  the different distances, kernel functions and similarity coefficients
  simply operate on tokens instead of bytes.  Try this command:

      harry -g tokens data.txt -

  Note how the distances differ from the first example, where you compute
  the Levenshtein distance for the bytes and not the tokens.  You can
  adapt the partitioning of strings to different data by defining a good
  delimiter set.  For example, you can use `-d ' %09%0a%0d'` to split
  strings using whitespace bytes.  The analysis of tokens comes also
  handy if you have structured strings, such as log entries, that can be
  easily partitioned into different tokens.

  Here is another example: the Jaccard coefficient has been originally
  developed for measuring the similarity of sets.  If it is used with the
  delimiter option of Harry, it computes the similarity of the sets of tokens
  contained in the strings.

      harry -m sim_jaccard -g tokens data.txt -

## Endless Options

  Many similarity measures have parameters that can be  adapted to a
  particular setting.  For example, the Levenshtein distance allows for
  individually changing the costs of an insertion, deletion and
  substitution.  The different parameter along with several other options
  can be defined in a configuration file.  This file spares you from
  specifying all parameters on the command line.  You can generate the
  default configuration of Harry by running:

      harry -C > harry.cfg

  You can then edit the configuration file, adapt it to your needs and use
  it later for running Harry as follows:

      harry -c harry.cfg data.txt -

  Note that you can always override parameters on the command line and thus
  the configuration file can be used as a base setup for running Harry in
  different experiments.  There is another cool feature: you can also
  convert a given set of command-line options to a configuration file.  In
  this example, the given delimiter set and the selected similarity measure
  are output to the configuration file.

      harry -g tokens -d ' .,:;' -m dist_jaro -D > harry.cfg

  There are many parameters and options. Check out the manual page of Harry
  to get an overview of what can be set and how.  There is also a list of
  references at the end of the manual page that provide detailed
  descriptions of the implemented similarity measures.

      man harry

## Multi-Core Computing

  If you are running a multi-core system, Harry automatically utilizes all
  cores for computing the similarity measure.  Obviously with only four
  strings in our example data, this feature is not necessary.  To
  demonstrate this feature we just replicate the content of the example
  file, as follows:

      for i in `seq 1 1000` ; do cat data.txt ; done > large-data.txt

  The resulting file contains 4000 strings and if we run Harry on it
  the computation takes significantly longer. We use the option `-v`
  to display a progress bar.

      harry -v large-data.txt matrix.txt

  If you monitor the CPU usage while running this command, you can
  (hopefully) see how all cores are used.  You can use the option `-n` to
  set the number of computation threads, if you don't want to use as many
  threads as available cores.

  There is another cool feature: Harry can use a global cache during the
  computation of a similarity measure.  If the same strings are compared
  twice, Harry skips the computation and returns the cached value.  Usually
  this will not happen very often, but in our example we have 3996 duplicate
  strings and thus this option boosts the computation time.

      harry -v -g large-data.txt matrix.txt

## Ranges and Splits

  So far we have only computed full square matrices. Often however, one is
  only interested in comparing one set of strings with another set of
  strings.  Harry supports this setting using ranges that can be defined on
  the x-axis and y-axis of the matrix.  For example, we can compare the
  first two strings in our example file, with the last two by running:

      harry -x 0:2 -y 2:4 data.txt -

  The ranges are defined similar to Python array indices,  where the first
  value defines the index of the first string and the second value defines
  the index after the last string.

  If the start or end index is omitted, the minimum or maximum value is
  substituted, respectively.  For example, ":4" selects strings starting
  from the index 0 and ":" chooses all strings.  If the end index is
  negative, it is subtracted from the maximum index, that is, ":-1" selects
  all strings except for the last one.  We can write the above command hence
  as follows

      harry -x :-2 -y 2: data.txt -

  For convenience, Harry supports another option that can be used to split
  the computation of a matrix into n pieces.  This open comes handy if you
  want to distribute the computation of a large similarity matrix over
  different hosts.  The following four commands each compute one split out
  of four splits.

      harry -s 4:0 data.txt split0.txt
      harry -s 4:1 data.txt split1.txt
      harry -s 4:2 data.txt split2.txt
      harry -s 4:3 data.txt split3.txt

  The matrices are split row-wise. That is, the resulting output can be
  simply concatenated to yield the original similarity matrix

       cat split?.txt > matrix.txt</pre>

## Output Formats

  Harry supports different output formats. In the previous examples you have
  already seen the simple text format that can be used with many analysis
  tools.  We now have a look at the Matlab output format:

       harry -o matlab data.txt matrix.mat

  You can easily access the computed similarity values from the Matlab
  environment by loading the file `matrix.mat`.

       > data = load('matrix.mat')
       data =
         scalar structure containing the fields:
           matrix =
               0    8   29   28
               8    0   29   32
              29   29    0   25
              28   32   25    0

  Similarly, you can use Harry to output the similarity values as a JSON
  object using the following command:

       harry -o json data.txt matrix.json

  JSON has been designed for use in JavaScript; however, it is also a handy
  format for programming in Python.  Here is some example in Python
  code.

       import json
       data = json.load(open('matrix.json'))
       for row in data['matrix']:
             print row</pre>

## Conclusions

   Have fun with Harry!
   Konrad & Christian
