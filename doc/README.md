# NAME

**harry** - A tool for measuring string similarity

# SYNOPSIS

**harry** \[**options**\] \[**-c** _config_\] _input_ \[_input_\] _output_

# DESCRIPTION

**harry** is a small tool for measuring the similarity of strings. The tool
supports common distance and kernel functions for strings as well as some
exotic similarity measures.  The focus of **harry** lies on implicit
similarity measures, that is, comparison functions that do not give rise to
an explicit vector space.  Examples of such similarity measures are the
Levenshtein distance, the Jaro-Winkler distance or the normalized
compression distance.

During operation **harry** loads a set of strings from _input_, computes the
specified similarity measure and writes a matrix of similarity values to
_output_.  If one _input_ is given, **harry** computes the similarities
between all strings in _input_.  If two _input_ sources are provided,
**harry** computes only the similarities between the two sources.  The
similarity measure can be computed based on the granularity of bits, bytes
as well as tokens contained in the strings.  The configuration of this
process, such as the input format, the similarity measure and the output
format, are specified in the file _config_ and can be additionally refined
using command-line options.

    .---------.                        .----------.
    |  lines  |                        |   text   |
    |   dir   |   \   .---------.   /  |  libsvm  |
    |  stdin  |   --  |  Harry  |  --  |  matlab  |
    |   ...   |   /   '---------'   \  |   ...    |
    '---------'                        '----------'
       Input      Similarity Measures     Output

**harry** is implemented using OpenMP, such that the computation time for a
set of strings scales linear with the number of available CPU cores.
Moreover, efficient implementations of several similarity measures,
effective caching of similarity values and low-overhead locking further
speedup the computation.

**harry** complements the tool **sally**(1) that embeds strings in a vector
space and allows computing vectorial similarity measures, such as the cosine
distance and the bag-of-words kernel.

# CONFIGURATION

The configuration of **harry** is provided by a configuration file.  This
file is structured into the three sections **input**, **measures** and
**output**, which define the parameters of the input format, the similarity
measures and the output format, respectively.  If no configuration file is
provided, **harry** resorts to a default configuration.  This default
configuration can be printed using the command-line option **-D** (see
**OPTIONS**).

## Input formats

**harry** supports different formats for reading sets of strings, which may
range from plain files to directories and other structured resources.  The
input format is specified in the configuration file of **harry**, but can
also be defined on the command line using the option **-i** (see **OPTIONS**).
Following is a list of supported input formats:

- **input = {**
    - **input\_format = "lines";**

        This parameter specifies the input format.

        - _"lines"_

            The input strings are available as lines in a text file. The name of the
            file is given as _input_ to **harry**.  The lines need to be separated by
            newline and may not contain the NUL character.  Labels can be extracted from
            each line using a regular expression (see **lines\_regex**).

        - _"stdin"_

            The input strings are provided on standard input (stdin) as text lines.  The
            lines need to be separated by newline and may not contain the NUL character. 
            Labels can be extracted from each line using a regular expression (see
            **lines\_regex**).  This input format is also enabled when _input_ is set to
            _-_, otherwise _input_ is ignored.

        - _"dir"_

            The input strings are available as binary files in a directory and the
            name of the directory is given as _input_ to **harry**. The suffixes
            of the files are used as labels for the strings.

        - _"arc"_

            The input strings are available as binary files in a compressed archive,
            such as a zip or tgz archive.  The name of the archive is given as _input_
            to **harry**.  The suffixes of the files are used as labels for the strings.

        - _"fasta"_

            The input strings are available in FASTA format. The name of the file is
            given as _input_ to **harry**.  Labels can be extracted from the description
            of each sequence using a regular expression (see **fasta\_regex**).  Comments
            are allowed if they are preceded by either ';' or '>'.

        - _"raw"_

            The input strings are provided on standard input (stdin) in "raw" format.
            This input module is designed to efficiently interface with other
            environments.  The binary format for strings has the form

                | len (uint32) | array (uint8) ... |
                | len (uint32) | array (uint8) ... |
                | ...

            where _len_ is a 32-bit unsigned integer in host byte order indicating the
            length of the following _array_ containing the string data in bytes. 
            Labels cannot be extracted from this representation.  This input format is
            also enabled when _input_ is set to _=_, otherwise _input_ is ignored.

    - **chunk\_size = 256;**

        To enable an efficient processing of large data sets, **harry** loads strings
        in chunks.  This parameter defines the number of strings in one of these
        chunks.  Depending on the lengths and type of the strings, this parameter
        can be adjusted to improve loading times.

    - **decode\_str = false;**

        If this parameter is set to _true_, **harry** automatically decodes strings
        that contain URI-encoded characters.  That is, substrings of the form %XX
        are replaced with the byte corresponding to the hexadecimal number XX.

    - **fasta\_regex = " (\\\\+|-)?\[0-9\]+";**

        The FASTA format allows to equip each string with a short description.  In
        several data sets this description contains a numerical label which can be
        used for supervised learning tasks.  The parameter defines a regular
        expression that matches these numerical labels, such as +1 and -1.

    - **lines\_regex = "^(\\\\+|-)?\[0-9\]+";**

        If the strings are available as text lines, the parameter can be used to
        extract a numerical label from the strings.  The parameter is a regular
        expression matching labels, such as +1 and -1.

    - **reverse\_str = false;**

        If this parameter is set to _true_, the characters of all input strings
        will be reversed.  Such reversing might help in situations where the reading
        direction of the input strings is unspecified.

    - **stoptoken\_file = "";**

        Stop tokens (irrelevant tokens) can be filtered from the strings by providing
        a file containing these tokens; one per line.  Non-printable characters can
        be escaped using URI encoding (%XX).  Stop tokens can only be filtered, if
        the **granularity** is set to _tokens_.

    - **soundex = false;**

        All tokens in the strings are mapped to the soundex index. For example,
        "Pfister" is mapped to "P236" and "Jackson" to "J250".  The soundex index
        has been originally designed for comparing names, however, in **harry** it
        can be applied to all sorts of tokens, if they are composed of alphabetic
        letters.  Punctation characters are ignored and thus the string "Hey, I am
        here with Harry!", gets mapped to "H000 I000 A500 H600 W300 H600".
- **};**

## Similarity Measures

**harry** supports different similarity measures for comparing string,
including common distance and kernel functions.  The similarity measure can
be specified in the configuration file as well as on the command line using
the option **-m** (see **OPTIONS**).  The name of each similarity measure is
prefixed by its type (_dist\__ for distances, _kern\__ for kernels and
_sim\__ for similarity measures).  For convenience, this prefix can be
omitted.  Moreover, the names of some similarity measures are aliased,
for example, the normalized compression distance is available as
_dist\_compression_ and _dist\_ncd_.

Parameters of the similarity measures are organized in individual
configuration groups.  For instance, parameters of the Levenshtein distance
are defined in the group **dist\_levenshtein**, while parameters for the Jaro
and Jaro-Winkler distance are given in **dist\_jarowinkler**.

- **measures = {**
    - **type = "dist\_levenshtein"**

        The parameter **type** specifies the similarity measure that is used for
        comparing the strings. Supported similarity measures are:

        - _"dist\_hamming"_

            Hamming distance. See configuration group **dist\_hamming**.

        - _"dist\_levenshtein"_, _"dist\_edit"_

            Levenshtein distance. See configuration group **dist\_levenshtein**.

        - _"dist\_damerau"_

            Damerau-Levenshtein distance. See configuration group **dist\_damerau**.

        - _"dist\_osa"_

            Optimal string alignment (OSA) distance. See configuration group **dist\_osa**.

        - _"dist\_jaro"_

            Jaro distance. See configuration group **dist\_jarowinkler**.

        - _"dist\_jarowinkler"_

            Jaro-Winkler distance. See configuration group **dist\_jarowinkler**.

        - _"dist\_lee"_

            Lee distance. See configuration group **dist\_lee**

        - _"dist\_compression"_, _"dist\_ncd"_

            Normalized compression distance (NCD). See configuration group **dist\_compression**.

        - _"dist\_bag"_

            Bag distance. See configuration group **dist\_bag**.

        - _"dist\_kernel"_

            Kernel substitution distance. See configuration group **dist\_kernel**.

        - _"kern\_subsequence"_, _"kern\_ssk"_

            Subsequence kernel (SSK). See configuration group **kern\_subsequence**.

        - _"kern\_spectrum"_, _"kern\_ngram"_

            Spectrum kernel (also n-gram kernel). See configuration group **kern\_spectrum**.

        - _"kern\_wdegree"_, _"kern\_wdk"_

            Weighted-degree kernel (WDK) with shifts. See configuration group **kern\_wdegree**.

        - _"kern\_distance"_, _"kern\_dsk"_

            Distance substitution kernel (DSK). See configuration group **kern\_distance**.

        - _"sim\_simpson"_

            Simpson coefficient. See configuration group **sim\_coefficient**.

        - _"sim\_jaccard"_

            Jaccard coefficient.  See configuration group **sim\_coefficient**.

        - _"sim\_braun"_

            Braun-Blanquet coefficient.  See configuration group **sim\_coefficient**.

        - _"sim\_dice"_, _"sim\_czekanowski"_

            Dice-coefficient (Czekanowsi coefficient) See configuration group
            **sim\_coefficient**.

        - _"sim\_sokal"_, _"sim\_anderberg"_

            Sokal-Sneath coefficient (Anderberg coefficient).  See configuration group
            **sim\_coefficient**.

        - _"sim\_kulczynski"_

            Second Kulczynski coefficient.  See configuration group **sim\_coefficient**.

        - _"sim\_otsuka"_, _"sim\_ochiai"_

            Otsuka coefficient (Ochiai coefficient).  See configuration group
            **sim\_coefficient**.

    - **granularity = "bytes";**

        This parameter controls the granularity of strings. It can be set to either
        _bits_, _bytes_ or _tokens_. Depending in the granularity a string is 
        considered as a sequence of bits, bytes or tokens, which results in different
        similarity values during comparison. 

    - **token\_delim = "";**

        The parameter **token\_delim** defines characters for delimiting tokens in
        strings, for example " %0a%0d".  It is only considered, if the granularity
        is set to _tokens_, otherwise it is ignored.

    - **num\_threads = 0;**

        The parameter **num\_threads** sets the number of threads for the calculation
        of the similarity measures.  If set 0, **harry** determines the number of
        available CPU cores using OpenMP and sets the number of threads accordingly.

    - **cache\_size = 256;**

        The parameter **cache\_size** specifies the maximum size of the internal cache
        in megabytes (Mb).  The general-purpose cache is used to speed up
        computations of **harry** for some similarity measures.

    - **global\_cache = false;**

        By default **harry** caches only internal computations. If this parameter is
        set to _true_, all similarity values are stored in the cache.  This feature
        should only be enabled if many of the compared strings are identical and
        thus caching similarity values can provide benefits.

    - **col\_range = "";**
    - **row\_range = "";**

        These two parameters control which strings are used for computing the matrix
        of similarity values.  **col\_range** defines a range of indices on the
        columns and **row\_range** on the rows of the matrix.  The format of the
        ranges is similar to indexing of Python arrays:  A range is given by
        "_start_:_end_", where _start_ defines the index of the first string and
        _end_ defines the index after the last string.  For example, _"0:4"_
        selects the strings at index 0, 1, 2, and 3.  If the start or end index is
        omitted, the minimum or maximum value is substituted, respectively.  For
        example, _":4"_ selects strings starting from the index _0_ and _":"_
        chooses all strings.  If the end index is negative, it is substracted from
        the maximum index, that is, _":-1"_ selects all strings except for the last
        one.

        The parameters **col\_range** and **row\_range** are ignore if two input sources
        are given on the command line.

    - **split = "";**

        To ease the computation of large similarity matrices, **harry** supports
        automatically splitting a matrix into blocks.  This splitting is defined by
        a string of the form "_blocks_:_idx_", where _blocks_ defines the number
        of blocks and _idx_ the index of the block to compute.  The matrix is
        splitted across the y-axis.  For many output formats the blocks can be
        simply concatenated to get the original matrix.

        The parameter **split** is ignore if two input sources are given on the
        command line.

    - **dist\_hamming = {**

        This module implements the Hamming distance (see Hamming, 1950).  The
        runtime complexity of a comparison is linear in the length of the strings.
        If the compared strings have unequal length, the length difference is added
        to the distance.  The following parameters are supported:

        - **norm = "none";**

            This parameter specifies the normalization of the distance. Supported values
            are _"none"_ for no normalization, _"min"_ for normalization on the
            minimum length, _"max"_ for normalization on the maximum length, _"avg"_
            for normalization on the average length of the compared strings.

    - **};**
    - **dist\_levenshtein = {**

        This module implements the Levenshtein distance (see Levenshtein, 1966). The
        runtime complexity of a comparison is quadratic in the length of the
        strings.  The following parameters are supported:

        - **norm = "none";**

            This parameter specifies the normalization of the distance. Supported values
            are _"none"_ for no normalization, _"min"_ for normalization on the
            minimum length, _"max"_ for normalization on the maximum length, _"avg"_
            for normalization on the average length of the compared

        - **cost\_ins = 1.0;**
        - **cost\_del = 1.0;**
        - **cost\_sub = 1.0;**

            The computation of the distance can be adapted using three parameters
            defining the cost for an insertion, deletion and substitution,
            respectively.  The default costs are _1.0_ for each operation.

    - **};**
    - **dist\_damerau = {**

        This module implements the Damerau-Levenshtein distance (see Damerau, 1964).
        The runtime and space complexity of a comparison is quadratic in the length
        of the strings.  The following parameters are supported:

        - **norm = "none";**

            This parameter specifies the normalization of the distance. Supported values
            are _"none"_ for no normalization, _"min"_ for normalization on the
            minimum length, _"max"_ for normalization on the maximum length, _"avg"_
            for normalization on the average length of the compared strings.

        - **cost\_ins = 1.0;**
        - **cost\_del = 1.0;**
        - **cost\_sub = 1.0;**
        - **cost\_tra = 1.0;**

            The computation of the distance can be adapted using four parameters
            defining the cost for an insertion, deletion, substitution and
            transposition, respectively.  The default costs are _1.0_ for each
            operation.

    - **};**
    - **dist\_osa = {**

        This module implements the optimal string alignment (OSA) distance, which is
        often confused with the Damerau-Levenshtein distance.  The difference
        between the two is that the OSA distance computes the number of edit
        operations needed to make the strings equal under the condition that no
        substring is edited more than once.  (see the Wikipedia article on the
        Damerau-Levenshtein distance).  The runtime and space complexity of a
        comparison is quadratic in the length of the strings.  The following
        parameters are supported:

        - **norm = "none";**

            This parameter specifies the normalization of the distance. Supported values
            are _"none"_ for no normalization, _"min"_ for normalization on the
            minimum length, _"max"_ for normalization on the maximum length, _"avg"_
            for normalization on the average length of the compared strings.

        - **cost\_ins = 1.0;**
        - **cost\_del = 1.0;**
        - **cost\_sub = 1.0;**
        - **cost\_tra = 1.0;**

            The computation of the distance can be adapted using four parameters
            defining the cost for an insertion, deletion, substitution and
            transposition, respectively.  The default costs are _1.0_ for each
            operation.

    - **};**
    - **dist\_jarowinkler = {**

        This module implements the Jaro distance (Jaro, 1989) and the Jaro-Winkler
        distance (Winkler, 1990).  In contrast to the original formulation, a valid
        distance function is implemented, where similar strings yield a low value
        and dissimilar strings a high value.  The runtime complexity of a comparison
        is quadratic in the length of the strings.  The following parameters are
        supported:

        - **scaling = 0.1;**

            If this parameter is set to _0_, the original Jaro distance is returned,
            otherwise the Jaro-Winkler distance is calculated. This distance uses a
            **scaling** which gives more favorable ratings to strings that match from
            the beginning up to 4 symbols. The default value is _0.1_.

    - **};**
    - **dist\_lee = {**

        This module implements the Lee distance (Lee, 1958) for strings. The runtime
        complexity of a comparison is linear in the length of the strings. If the
        compared strings have unequal length, the remaining symbols of the longer
        string are added to the distance. The following parameters are supported:

        - **min\_sym = 0;**
        =item **max\_sym = 255;**

            These parameters specify the range of symbols, that is, the minimum and
            maximum value of a symbol in all strings.  If the strings consist of bytes,
            **min\_sym** is typically set to _0_ and **max\_sym** to _255_.  For printable
            characters the range can be further narrowed to _32_ and _126_.  If tokens
            are analyzed using the parameter **token\_delim**, **min\_sym** must be set to 0
            and **max\_sym** to _65535_, as the tokens are mapped to integers in this
            range.

    - **};**
    - **dist\_compression = {**

        This module implements the normalized compression distance for strings
        (Cilibrasi and Vitanyi, 2005).  The distance is "symmetrized".  The
        compression is implemented using **zlib**.  Note that the comparison of
        strings highly depends on the characteristics of the compressor (Cebrian et
        al., 2005).  The strings should not be longer than 16 kilobytes, such that
        two strings fit into the window of **zlib**.  The runtime complexity of a
        comparison is linear in the length of the strings, though with a large
        constant factor.  The following parameters are supported:

        - **level = 9;**

            This parameter defines the compression level used by **zlib** and must be
            between _1_ and _9_, where _1_ gives the best speed and _9_ the best
            compression.  See **zlib(3)**

    - **};**
    - **dist\_bag = {**

        This module implements the bag distance (see Bartolini et al., 2002). The
        distance approximates and lower bounds the Levenshtein distance.  The
        runtime complexity of a comparison is linear in the length of the strings.
        The following parameters are supported:

        - **norm = "none";**

            This parameter specifies the normalization of the distance. Supported values
            are _"none"_ for no normalization, _"min"_ for normalization on the
            minimum length, _"max"_ for normalization on the maximum length, _"avg"_
            for normalization on the average length of the compared strings.

    - **};**
    - **dist\_kernel = {**

        This module implements a kernel-based distance, that is, a distance  is
        computed given a kernel function for strings.  The specified kernel function
        is mapped to a Euclidean distance using simple geometry.  The runtime
        complexity depends on the kernel function.  The following parameters are
        supported:

        - **kern = "kern\_wdegree";**

            This parameter selects the kernel function to use for the distance. The
            kernel is mapped to a Euclidean distance using simple geometry.

        - **norm = "none";**

            This parameter specifies the normalization of the kernel. Supported values
            are _"none"_ for no normalization and _"l2"_ for the standard l2
            normalization of kernels.

        - **squared = true;**

            The module computes a Euclidean distance from the given kernel function. If
            this parameter is enabled a squared Euclidean distance is returned which
            is slightly faster due to the omitted root computation.

    - **};**
    - **kern\_wdegree = {**

        This module implements the weighted-degree kernel with shifts (Sonnenburg et
        al., 2007).  The runtime complexity is linear in the length of the strings.
        If the strings have unequal length, the remaining symbols of the longer
        string are ignored, in accordance with the kernel definition.  The following
        parameters are supported:

        - **norm = "none";**

            This parameter specifies the normalization of the kernel. Supported values
            are _"none"_ for no normalization and _"l2"_ for the standard l2
            normalization of kernels.

        - **degree = 3;**

            This parameter specifies the degree of the kernel, that is, the length of
            considered k-mers/k-grams.  As the kernel computation is implicit, the
            k-mers are not extracted but implicitly counted by blocks of matching symbols.

        - **shift = 0;**

            To compensate noise in the strings, the kernel can be computed with "shifts".
            The strings are compared multiple times with different positive and negative
            offsets up to **shift** symbols.  The different kernel values are added.  The
            runtime complexity is increased by twice the value of **shift**.

    - **};**
    - **kern\_subsequence = {**

        This module implements the subsequence kernel (Lodhi et al., 2002). The
        runtime complexity is quadratic in the length of the strings. The following
        parameters are supported:

        - **norm = "none";**

            This parameter specifies the normalization of the kernel. Supported values
            are _"none"_ for no normalization and _"l2"_ for the standard l2
            normalization of kernels.

        - **length = 3;**

            This parameter specifies the length of subsequence to consider.

        - **lambda = 0.1;**

            This parameter is a weighting term for gaps within subsequences.

    - **};**
    - **kern\_spectrum = {**

        This module implements the spectrum kernel (Leslie et al., 2002). The
        runtime complexity is linear in the length of the strings.  The spectrum
        kernel is closely related to bag-of-words kernels.  Thus, the tool
        **sally(1)** may be alternatively used to compute the kernel using an
        explicit vector space.  The following parameters are supported by the
        implementation:

        - **norm = "none";**

            This parameter specifies the normalization of the kernel. Supported values
            are _"none"_ for no normalization and _"l2"_ for the standard l2
            normalization of kernels.

        - **length = 3;**

            This parameter specifies the length of k-mers/k-grams to consider.

    - **};**
    - **kern\_distance = {**

        This module implements distance substitution kernels (Haasdonk and Bahlmann,
        2004).  The empty string is considered the origin of the underlying implicit
        vector space.  The runtime complexity depends on the selected distance
        function.  The following parameters are supported:

        - **dist = "dist\_bag";**

            This parameter selects the distance function to use for the kernel.
            Depending on the type of the substitution and the selected distance, the
            kernel might not be positive semi-definite.

        - **type = "linear";**

            Four types of substitutions can be selected for creating a kernel from a
            distance function: _"linear"_, _"poly"_, _"neg"_ and _"rbf"_.  For a
            detailed explanation of each substitution see the paper by Haasdonk and
            Bahlmann (2004).

        - **norm = "none";**

            This parameter specifies the normalization of the kernel. Supported values
            are _"none"_ for no normalization and _"l2"_ for the standard l2
            normalization of kernels.

        - **gamma = 1.0;**

            This parameter specifies a scaling factor for the substitution types
            _"poly"_ and _"rbf"_.

        - **degree = 1.0;**

            This parameter defines a polynomial degree for the substitution types
            _"poly"_ and _"neg"_.

    - **};**
    - **sim\_coefficient = {**

        This module implements several similarity coefficients for strings (see
        Cheetham and Hazel, 1969).  The runtime complexity of a comparison is linear
        in the length of the strings.  The following parameters are supported:

        - **matching = "bin";**

            The parameter specifies how the symbols of the strings are matched. If the
            parameter is set to _"bin"_, the symbols are considered as binary attributes
            that are either present or not.  If the parameter is set to _"cnt"_, the
            count of each symbol is considered for the matching.

    - **};**
- **};**

## Output formats

Once strings have been compared, **harry** stores the similarity values in
one of several common formats, which allows for applying typical tools of
statistics and machine learning to the data.  Following is a list of
supported output formats and respective parameters.  Additionally, the
output format can be specified using the command-line option **-o** (see
**OPTIONS**).

- **output = {**
    - **output\_format = "text";**

        Following is a list of output formats supported by **harry**:

        - _"text"_

            The similarity values are stored as plain text.

        - _"stdout"_

            The similarity values are written to standard output (stdout) as plain text.
            This output format is also enabled when _output_ is set to _-_, otherwise
            _output_ is ignored.

        - _"libsvm"_

            The similarity values are stored as precomputed kernel for libsvm.

        - _"json"_

            The similarity values are stored in JSON object.

        - _"matlab"_

            The similarity values are stored in Matlab format (version 5).

        - _"raw"_

            The similarity values are written to standard output (stdout) in raw format.
            This output module is designed for interfacing with other analysis
            environments.  The format of the similarity matrix has the following form

                | rows (uint32)  | cols (uint32)     |
                | fsize (uint32) | array (float) ... |

            where _rows_ and _cols_ are unsigned 32-bit integers specifing the
            dimensions of the matrix, _fsize_ is the size of a float in bytes and
            _array_ holds the matrix as floats.  Indices, labels and sources are not
            output.  This output format is also enables when _output_ is set to _=_,
            otherwise _output_ is ignored.

    - **precision = 0;**

        Precision of the output in terms of decimal places. A precision of _0_
        selects the full single float range for output.

    - **separator = ",";**

        This parameter defines the separator used in text mode for separating
        the similarity values.

    - **save\_indices = false;**

        If this parameter is to _true_ and supported by the output format, the
        indices of the strings will be additionally stored.

    - **save\_labels = false;**

        If this parameter is to _true_ and supported by the output format, the
        labels of the strings will be additionally stored.

    - **save\_sources = false;**

        If this parameter is to _true_ and supported by the output format, the
        sources of the strings will be additionally stored.

    - **compress = false;**

        If this parameter is set to _true_, the output is stored using zlib
        compression, which can significantly reduce the required disk space.
        Several programs support reading files compressed using zlib.
        Alternatively, the tools gzcat(1) and gunzip(1) can be used to access the
        data.
- **};**

# OPTIONS

The configuration of **harry** can be refined using several command-line
options.  Moreover, some parameters of the configuration can be overwritten
on the command line.  Following is the list of options:

## I/O options

    -i,  --input_format <format>   Set input format for strings.
         --decode_str              Enable URI-decoding of strings.
         --reverse_str             Reverse (flip) all strings.
         --stoptoken_file <file>   Provide a file with stop tokens.
         --soundex                 Enable soundex encoding of tokens.
         --benchmark <seconds>     Perform benchmark run.
    -o,  --output_format <format>  Set output format for matrix.
    -p,  --precision <num>         Set precision of output.
    -z,  --compress                Enable zlib compression of output.
         --save_indices            Save indices of strings.
         --save_labels             Save labels of strings.
         --save_sources            Save sources of strings.

## Module options:

    -m,  --measure <name>           Set similarity measure.
    -g,  --granularity <type>       Set granularity: bytes, bits, tokens.
    -d,  --token_delim <delim>      Set delimiters for tokens.
    -n,  --num_threads <num>        Set number of threads.
    -a,  --cache_size <size>        Set size of cache in megabytes.
    -G,  --global_cache             Enable global cache.
    -x,  --col_range <start>:<end>  Set the column range (x) of strings.
    -y,  --row_range <start>:<end>  Set the row range (y) of strings.
    -s,  --split <blocks>:<idx>     Split matrix into blocks and compute one.

## Generic options:

    -c,  --config_file <file>      Set configuration file.
    -v,  --verbose                 Increase verbosity.
    -l,  --log_line                Print a log line every minute
    -q,  --quiet                   Be quiet during processing.
    -M,  --print_measures          Print list of similarity measures
    -C,  --print_config            Print the current configuration.
    -D,  --print_defaults          Print the default configuration.
    -V,  --version                 Print version and copyright.
    -h,  --help                    Print this help screen.

# FILES

- `PREFIX/share/doc/harry/example.cfg`

    An example configuration file for **harry**. See the configuration
    section for further details.

# LIMITATIONS

**harry** supports only symmetric similarity measures, that is, m(x,y) =
m(y,x) for all x and y.  This restriction saves considerable run-time and
memory in most cases.  However, some similarity measures need to be
artificially "symmetrized", such as the normalized compression distance,
thereby requiring additional computations.

# REFERENCES

Bartolini, Ciaccia, Patella. String Matching with Metric Trees Using an
Approximate Distance.  String Processing and Information Retrieval, LNCS
2476, 271-283, 2002.

Cebrian, Alfonseca, and Ortega. Common pitfalls using the normalized
compression distance.  Communications in Information and Systems, 5 (4),
367-384, 2005.

Cheetham and Hazel. Binary (Presence-Absence) Similarity Coefficients.
Journal of Paleontology, 43:5, 1130-1136, 1969

Cilibrasi and Vitanyi. Clustering by compression, IEEE Transactions on
Information Theory, 51:4, 1523-1545, 2005.

Damerau. A technique for computer detection and correction of spelling
errors, Communications of the ACM, 7(3):171-176, 1964

Haasdonk and Bahlmann. Learning with Distance Substitution Kernels. Pattern
Recognition ; DAGM Symposium, 220-227, 2004.

Hamming. Error-detecting and error-correcting codes. Bell System Technical
Journal, 29(2):147-160, 1950.

Jaro. Advances in record linkage methodology as applied to the 1985 census
of Tampa Florida.  Journal of the American Statistical Association 84 (406):
414-420, 1989.

Lee. Some properties of nonbinary error-correcting codes. IRE Transactions
on Information Theory 4 (2): 77-82, 1958.

Levenshtein. Binary codes capable of correcting deletions, insertions, and
reversals.  Doklady Akademii Nauk SSSR, 163 (4):845-848, 1966.

Lodhi, Saunders, Shawe-Taylor, Cristianini, and Watkins. Text classification
using string kernels.  Journal of Machine Learning Research, 2:419-444,
2002.

Sonnenburg, Raetsch, and Rieck. Large scale learning with string kernels. In
Large Scale Kernel Machines, pages 73--103.  MIT Press, 2007.

Winkler.  String Comparator Metrics and Enhanced Decision Rules in the
Fellegi-Sunter Model of Record Linkage.  Proceedings of the Section on
Survey Research Methods.  354-359, 1990.

# COPYRIGHT

Copyright (c) 2013-2015 Konrad Rieck (konrad@mlsec.org)
                        Christian Wressnegger (christian@mlsec.org)

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.  This program is distributed without any warranty.  See
the GNU General Public License for more details.
