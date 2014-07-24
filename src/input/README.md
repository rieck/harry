Development of Input Modules
==

Three steps
--

Harry can be easily extended to support different input formats for strings. 
The development of a new input module basically involves three steps:
  
1. Create the source files `input_xxx.c` and `input_xxx.h` for the
   new input module xxx. Add these files to `Makefile.am` to include them in
   the compilation process of Harry.
  
2. Implement three functions in `input_xxx.c` and add respective
   declarations of these functions to `input_xxx.h`.
           
       `int input_xxx_open(char *name);`
     
   This function opens the input source for reading of strings. For example,
   if `xxx` refers to an archive, this functions corresponds to opening the
   archive and preparing it for loading entries.  The function returns 
   1 if the input could be successfully opened and 0 otherwise.
     
       `int input_xxx_read(string_t *strs, int len);`
     
   This function reads a block of strings. The parameter `strs` is used to
   store the loaded strings and respective information (see input.h) The
   array need to be allocated by the caller, where its length is given in
   `len`.  The function returns the number of loaded strings; any number
   smaller than `len` indicates the end of the input source.
       
       `void input_xxx_close();`
     
   This function closes the input source for the format xxx. Memory
   allocated by input_xxx_open() should be freed here.  Open files and
   similar objects should be closed.
       
3. Integrate the new interface into Harry by extending the code in
   `input.c`.  First, add `input_xxx.h` to the list of included headers and,
   second, extend the function input_config() to initialize the new input
   format if requested.

Support Harry
--
     
That's it. Please contribute to the development of Harry. Send your new
modules to konrad@mlsec.org, so that they can be included in the next
release.
  
