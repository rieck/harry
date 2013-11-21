
Development of Output Modules
==

Three Steps
--

Harry can be easily extended to support different output formats for
vectors. The development of a new output module basically involves
three steps:
  
1. Create the source files `output_xxx.c` and `output_xxx.h` for the
   new output module xxx.  Add these files to `Makefile.am` to
   include them in the compilation process of Harry.
  
2. Implement three functions in `output_xxx.c` and add respective
   declarations of these functions to `output_xxx.h`.
           
       `int output_xxx_open(char *name);`
     
   This function opens the output destination for writing similarity values. 
   For example, if `xxx` refers to matlab support, this functions
   corresponds to opening and initializing a file in matlab format.  The
   function returns 1 on success and on 0 on failure.
     
       `int output_xxx_write(hmatrix_t *mat);`

   The function writes a matrix of similarity/dissimilarity values to the
   output.  See the definition of hmatrix_t in hmatrix.h for details on the
   content of the matrix structure.  The function should return the number
   of written values.
       
       `void output_xxx_close();`
     
   This function closes the output destination for the format xxx.
   Memory allocated by output_xxx_open() should be freed here. Open
   files and similar objects should be closed.
     
3. Integrate the new interface into Harry by extending the code in
   `output.c`. First, add `output_xxx.h` to the list of included
   headers and, second, extend the function output_config() to
   initialize the new output format if requested.

Support Harry
--
 
That's it. Please contribute to the development of Harry. Send your
new modules to konrad@mlsec.org, so that they can be included in the
next release.
  
