Dir
|--harbol_common_defines.h  => OS specific, Compiler specific, Standard specific defines for C.
|--harbol_common_includes.h => inline function helpers and standard types.
|--targum_err.(c/h)         => err/warning/custom messaging module.
|--targum_lexer.(c/h)       => custom lexical analyzer, dependency on the above data structures.
|--test_driver.c            => targum lexer test driver program.
|--tokens.cfg               => example config file.
|--Makefile                 => library makefile.
|----cfg                    => custom, JSON-like config parser. Dependency with Linkmap & Variant.
|----linkmap                => combination hash table & dynamic array. Dependency with map.
|----map                    => string-key hash table. Dependency with stringobj & vector.
|----stringobj              => immutable C string object type.
|----vector                 => dynamic array.
|----variant                => variant type for holding any type of object.
|----lex                    => lexing tools module.
|----docs                   => folder you're in right now.
