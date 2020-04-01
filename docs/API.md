# C interface

# Datatypes

## struct TargumTokenInfo

```c
struct TargumTokenInfo {
	struct HarbolString lexeme;
	const struct HarbolString *filename;
	size_t
		start,  /// start offset of lexeme.
		end,    /// end offset of lexeme.
		line,   /// line in source code.
		col     /// column in source code.
	;
	uint32_t tag;
};
```

### lexeme
string object that holds the lexeme of the token.

### filename
pointer to a string object of the filename the token resides in.

### start
offset, in the source code, the token starts at.

### end
offset, in the source code, the token ends at.

### line
the line in the source code that the tokens is at.

### col
the column in the source code that the tokens is at.

### tag
unsigned integer value of the token. (defined by the config file used by the lexer)


## struct TargumLexer

```c
struct TargumLexer {
	struct HarbolVector tokens;
	struct HarbolString filename, src;
	struct HarbolLinkMap *cfg;
	char *iter, *line_start;
	size_t line, index;
	struct TargumTokenInfo *curr_tok;
};
```

### tokens
dynamic array of tokens. Always ends with an EOF token.

### filename
string of the filename that the lexer is currently lexing.

### src
entire source code in a string object.

### cfg
pointer to a linkmap structure representing the config file.

### iter
char pointer used to iterate the source code.

### line_start
char pointer used to determine the column of a token.

### line
current line in the source code.

### index
current index used to iterate the `tokens` array.

### curr_tok
token pointer as pointed to with `index`.



# Functions/Methods


## targum_err
```c
void targum_err(const char filename[], const char errtype[], const size_t line, const size_t col, const char err[], ...);
```

### Description
Prints a dedicated error message.

### Parameters
* `filename` - filename, can be NULL.
* `errtype` - custom "error type", can be NULL but best to just put "error".
* `line` - what line the error is at.
* `col` - what column the error is at.
* `err` - err message to format.
* `...` - variadic args.

### Return Value
None.


## targum_warn
```c
void targum_warn(const char filename[], const char warntype[], const size_t line, const size_t col, const char warn[], ...);
```

### Description
Prints a dedicated warning message.

### Parameters
* `filename` - filename, can be NULL.
* `warntype` - custom "warn type", can be NULL but best to just put "warning".
* `line` - what line the warning is at.
* `col` - what column the warning is at.
* `warn` - warning message to format.
* `...` - variadic args.

### Return Value
None.


## targum_msg
```c
void targum_msg(const char custom[], ...);
```

### Description
Prints a custom message.

### Parameters
* `custom` - warning message to format.
* `...` - variadic args.

### Return Value
None.


## targum_error_count
```c
size_t targum_error_count(void);
```

### Description
gives a count of how many error messages were printed.

### Parameters
None.

### Return Value
unsigned integer of number of error message printed.


## targum_is_fataled
```c
bool targum_is_fataled(void);
```

### Description
If an error was printed, the system is 'fataled' meaning compilation or tokenization failed.

### Parameters
None.

### Return Value
boolean if an error was given or not.


## targum_reset
```c
void targum_reset(void);
```

### Description
resets the system error counter.

### Parameters
None.

### Return Value
None.


## targum_token_info_clear
```c
void targum_token_info_clear(struct TargumTokenInfo *tokinfo);
```

### Description
clears out the memory of the token info object.

### Parameters
* `tokinfo` - pointer to token info object.

### Return Value
None.


## targum_token_info_get_lexeme
```c
const char *targum_token_info_get_lexeme(const struct TargumTokenInfo *tokinfo);
```

### Description
self explanatory.

### Parameters
* `tokinfo` - pointer to a constant token info object.

### Return Value
char pointer of the token lexeme string.


## targum_token_info_get_filename
```c
const char *targum_token_info_get_filename(const struct TargumTokenInfo *tokinfo);
```

### Description
self explanatory.

### Parameters
* `tokinfo` - pointer to a constant token info object.

### Return Value
char pointer of the file name string.


## targum_token_info_get_token
```c
uint32_t targum_token_info_get_token(const struct TargumTokenInfo *tokinfo);
```

### Description
self explanatory.

### Parameters
* `tokinfo` - pointer to a constant token info object.

### Return Value
unsigned integer token value.


## targum_lexer_new_from_buffer
```c
struct TargumLexer *targum_lexer_new_from_buffer(const char src[], struct HarbolLinkMap *cfg);
```

### Description
Sets up a lexer object with a C string value to tokenize.

### Parameters
* `src` - C string value to lexically analyze.
* `cfg` - linkmap config structure to share (useful when importing/including other files), can be NULL.

### Return Value
pointer to an allocated lexer object.


## targum_lexer_new_from_file
```c
struct TargumLexer *targum_lexer_new_from_file(const char filename[], struct HarbolLinkMap *cfg);
```

### Description
Sets up a lexer object with a file to tokenize.

### Parameters
* `filename` - C string of file to open and lexically analyze.
* `cfg` - linkmap config structure to share (useful when importing/including other files), can be NULL.

### Return Value
pointer to an allocated lexer object.


## targum_lexer_create_from_buffer
```c
struct TargumLexer targum_lexer_create_from_buffer(const char src[], struct HarbolLinkMap *cfg);
```

### Description
Sets up a lexer object with a C string value to tokenize.

### Parameters
* `src` - C string value to lexically analyze.
* `cfg` - linkmap config structure to share (useful when importing/including other files), can be NULL.

### Return Value
lexer object.


## targum_lexer_create_from_file
```c
struct TargumLexer targum_lexer_create_from_file(const char filename[], struct HarbolLinkMap *cfg);
```

### Description
Sets up a lexer object with a file to tokenize.

### Parameters
* `filename` - C string of file to open and lexically analyze.
* `cfg` - linkmap config structure to share (useful when importing/including other files), can be NULL.

### Return Value
lexer object.


## targum_lexer_clear
```c
void targum_lexer_clear(struct TargumLexer *lexer, bool free_cfg);
```

### Description
clears out the memory of a lexer object. 

### Parameters
* `lexer` - pointer to lexer object to free up.
* `free_cfg` - boolean to free the linkmap config structure held by the lexer object, set to false if sharing the config pointer.

### Return Value
None.


## targum_lexer_free
```c
void targum_lexer_free(struct TargumLexer **lexer_ref, bool free_cfg);
```

### Description
clears out the memory of a lexer object. Pointed-to-pointer is set to NULL.

### Parameters
* `lexer_ref` - pointer to a pointer to lexer object to free up.
* `free_cfg` - boolean to free the linkmap config structure held by the lexer object, set to false if sharing the config pointer.

### Return Value
None.


## targum_lexer_load_cfg_file
```c
bool targum_lexer_load_cfg_file(struct TargumLexer *lexer, const char cfg_file[]);
```

### Description
loads a config file to a lexer object.

### Parameters
* `lexer` - pointer to lexer object.
* `cfg_file` - config file name to load.

### Return Value
true if successful, false otherwise.


## targum_lexer_load_cfg_cstr
```c
bool targum_lexer_load_cfg_cstr(struct TargumLexer *lexer, const char cfg_cstr[]);
```

### Description
parse a manually constructed config C string to a lexer object.

### Parameters
* `lexer` - pointer to lexer object.
* `cfg_cstr` - C string of a config.

### Return Value
true if successful, false otherwise.


## targum_lexer_get_cfg
```c
struct HarbolLinkMap *targum_lexer_get_cfg(const struct TargumLexer *lexer);
```

### Description
gets the linkmap structure that represents a config.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
true if successful, false otherwise.


## targum_lexer_get_filename
```c
const char *targum_lexer_get_filename(const struct TargumLexer *lexer);
```

### Description
retrieves C string of the filename the lexer is to analyze or analyzing.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
char pointer to the filename string.


## targum_lexer_get_token_index
```c
size_t targum_lexer_get_token_index(const struct TargumLexer *lexer);
```

### Description
self explanatory.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
unsigned integer of the current token index.


## targum_lexer_get_token_count
```c
size_t targum_lexer_get_token_count(const struct TargumLexer *lexer);
```

### Description
self explanatory.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
unsigned integer of how many tokens were tokenized.


## targum_lexer_get_token
```c
struct TargumTokenInfo *targum_lexer_get_token(const struct TargumLexer *lexer);
```

### Description
self explanatory.

### Parameters
* `lexer` - pointer to constant lexer object.

### Return Value
pointer to the current token managed by the lexer object.


## targum_lexer_advance
```c
struct TargumTokenInfo *targum_lexer_advance(struct TargumLexer *lexer, bool flush_tokens);
```

### Description
sets the current token managed by the lexer object, increment the tokens index, and returns the current token.

### Parameters
* `lexer` - pointer to lexer object.
* `flush_tokens` - boolean to clear out token array (if tokenizing limited amount of tokens at a time).

### Return Value
pointer to the current token managed by the lexer object.


## targum_lexer_reset
```c
void targum_lexer_reset(struct TargumLexer *lexer);
```

### Description
Resets the lexer object's state and clears out all the tokens.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
None.


## targum_lexer_reset_token_index
```c
void targum_lexer_reset_token_index(struct TargumLexer *lexer);
```

### Description
Resets the token index back to 0 and sets the current token managed by the lexer to the first token.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
None.


## targum_lexer_generate_tokens
```c
bool targum_lexer_generate_tokens(struct TargumLexer *lexer);
```

### Description
Tokenizes the source file given to the lexer object.
Do not forget to load/supply a source file/string to tokenize.
Do not forget to load/supply a config file/string to use for tokenizing.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
true if successful, false otherwise.


## targum_lexer_remove_token
```c
bool targum_lexer_remove_token(struct TargumLexer *lexer, uint32_t tag);
```

### Description
Purges a specific token type from the dynamic token array held by the lexer object.

### Parameters
* `lexer` - pointer to lexer object.
* `tag` - unsigned integer token value to purge from the token array.

### Return Value
true if successful, false otherwise.


## targum_lexer_remove_comments
```c
bool targum_lexer_remove_comments(struct TargumLexer *lexer);
```

### Description
Purges all comment tokens from the token array.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
true if successful, false otherwise.


## targum_lexer_remove_whitespace
```c
bool targum_lexer_remove_whitespace(struct TargumLexer *lexer);
```

### Description
Purges all whitespace tokens from the token array.
Only necessary if you set up your language config to tokenize (specific) whitespace like what Python does.

### Parameters
* `lexer` - pointer to lexer object.

### Return Value
true if successful, false otherwise.
