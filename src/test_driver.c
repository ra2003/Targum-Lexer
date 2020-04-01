#include <assert.h>
#include "targum_lexer.h"

int32_t main(const int32_t argc, char *argv[restrict static 1])
{
	if( argc<2 ) {
		puts("Targum Lexer Driver Error: missing text file.");
		return -1;
	} else if( !strcmp(argv[1], "--help") ) {
		puts("Targum Lexer Driver - 'test_driver filename'\nExample: 'test_driver file.ext'");
	} else if( !strcmp(argv[1], "--version") ) {
		puts("Targum Lexer Driver Version " TARGUM_LEXER_VERSION_STRING);
	} else {
		struct TargumLexer lexer = targum_lexer_create_from_file(argv[1], NULL);
		assert( targum_lexer_load_cfg_file(&lexer, "tokens.cfg") && "failed to load tokens.cfg!" );
		const bool result = targum_lexer_generate_tokens(&lexer);
		harbol_string_clear(&lexer.src);
		printf("tokenization? '%s'\n", result ? "success!" : "failure!");
		//targum_lexer_remove_comments(&lexer);
		///*
		FILE *restrict print_text = fopen("targum_lexer_tokens.txt", "w");
		assert( print_text != NULL && "failed to create targum_lexer_tokens.txt!" );
		for( uindex_t i=0; i<lexer.tokens.count; i++ ) {
			const struct TargumTokenInfo *ti = harbol_vector_get(&lexer.tokens, i);
			fprintf(print_text, "token info [%zu]:\n\tlexeme: '%s' | len: %zu\n\tfilename: '%s'\n\ttoken value: '%u'\n\tpos:: start: '%zu', end: '%zu', line: '%zu', col: '%zu'\n\n", i, ti->lexeme.cstr, ti->lexeme.len, ti->filename->cstr, ti->tag, ti->start, ti->end, ti->line, ti->col);
		}
		fclose(print_text), print_text=NULL;
		//*/
		targum_lexer_clear(&lexer, true);
	}
}
