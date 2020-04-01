#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#include "targum_lexer.h"

#ifdef OS_WINDOWS
#	define TARGUM_LIB
#endif


TARGUM_API void targum_token_info_clear(struct TargumTokenInfo *const tokinfo)
{
	harbol_string_clear(&tokinfo->lexeme);
}

TARGUM_API const char *targum_token_info_get_lexeme(const struct TargumTokenInfo *const tokinfo)
{
	return tokinfo->lexeme.cstr;
}

TARGUM_API const char *targum_token_info_get_filename(const struct TargumTokenInfo *const tokinfo)
{
	return tokinfo->filename->cstr;
}

TARGUM_API uint32_t targum_token_info_get_token(const struct TargumTokenInfo *const tokinfo)
{
	return tokinfo->tag;
}



TARGUM_API struct TargumLexer *targum_lexer_new_from_buffer(const char src[restrict static 1], struct HarbolLinkMap *const restrict cfg)
{
	struct TargumLexer *restrict lexer = harbol_alloc(1, sizeof *lexer);
	if( lexer != NULL )
		*lexer = targum_lexer_create_from_buffer(src, cfg);
	return lexer;
}

TARGUM_API struct TargumLexer *targum_lexer_new_from_file(const char filename[restrict static 1], struct HarbolLinkMap *const restrict cfg)
{
	struct TargumLexer *restrict lexer = harbol_alloc(1, sizeof *lexer);
	if( lexer != NULL )
		*lexer = targum_lexer_create_from_file(filename, cfg);
	return lexer;
}

NEVER_NULL(1) static void _setup_lexer(struct TargumLexer *const lexer, struct HarbolLinkMap *const cfg)
{
	lexer->iter = lexer->line_start = lexer->src.cstr;
	lexer->cfg = cfg;
	lexer->tokens = harbol_vector_create(sizeof(struct TargumTokenInfo), 0);
}

TARGUM_API struct TargumLexer targum_lexer_create_from_buffer(const char src[restrict static 1], struct HarbolLinkMap *const restrict cfg)
{
	struct TargumLexer lexer = {0};
	lexer.src = harbol_string_create(src);
	harbol_string_format(&lexer.filename, "%p", src);
	_setup_lexer(&lexer, cfg);
	return lexer;
}

TARGUM_API struct TargumLexer targum_lexer_create_from_file(const char filename[restrict static 1], struct HarbolLinkMap *const restrict cfg)
{
	struct TargumLexer lexer = {0};
	FILE *restrict src_file = fopen(filename, "r");
	if( src_file==NULL )
		return lexer;
	
	const bool result = harbol_string_read_file(&lexer.src, src_file);
	fclose(src_file), src_file=NULL;
	if( !result ) {
		harbol_string_clear(&lexer.src);
	} else {
		lexer.filename = harbol_string_create(filename);
		_setup_lexer(&lexer, cfg);
	}
	return lexer;
}

static void clear_token(void **const p)
{
	struct TargumTokenInfo *t = *p;
	harbol_string_clear(&t->lexeme);
}

TARGUM_API void targum_lexer_clear(struct TargumLexer *const lexer, const bool free_config)
{
	targum_lexer_clear_tokens(lexer);
	if( free_config )
		harbol_cfg_free(&lexer->cfg);
	harbol_string_clear(&lexer->filename);
	harbol_string_clear(&lexer->src);
}


TARGUM_API void targum_lexer_clear_tokens(struct TargumLexer *const lexer)
{
	harbol_vector_clear(&lexer->tokens, clear_token);
	lexer->index = 0;
}

TARGUM_API void targum_lexer_free(struct TargumLexer **const lexer_ref, const bool free_config)
{
	targum_lexer_clear(*lexer_ref, free_config);
	harbol_free(*lexer_ref), *lexer_ref = NULL;
}

TARGUM_API bool targum_lexer_load_cfg_file(struct TargumLexer *const restrict lexer, const char filename[restrict static 1])
{
	return lexer->cfg = harbol_cfg_parse_file(filename), lexer->cfg != NULL;
}

TARGUM_API bool targum_lexer_load_cfg_cstr(struct TargumLexer *const restrict lexer, const char cfg_cstr[restrict static 1])
{
	return lexer->cfg = harbol_cfg_parse_cstr(cfg_cstr), lexer->cfg != NULL;
}

TARGUM_API struct HarbolLinkMap *targum_lexer_get_cfg(const struct TargumLexer *const lexer)
{
	return lexer->cfg;
}


TARGUM_API const char *targum_lexer_get_filename(const struct TargumLexer *const lexer)
{
	return lexer->filename.cstr;
}

TARGUM_API size_t targum_lexer_get_token_index(const struct TargumLexer *const lexer)
{
	return lexer->index;
}

TARGUM_API size_t targum_lexer_get_token_count(const struct TargumLexer *const lexer)
{
	return lexer->tokens.count;
}

TARGUM_API struct TargumTokenInfo *targum_lexer_get_token(const struct TargumLexer *const lexer)
{
	return lexer->curr_tok;
}

TARGUM_API struct TargumTokenInfo *targum_lexer_advance(struct TargumLexer *const lexer)
{
	if( lexer->index < lexer->tokens.count )
		lexer->curr_tok = harbol_vector_get(&lexer->tokens, lexer->index++);
	return lexer->curr_tok;
}

TARGUM_API void targum_lexer_reset(struct TargumLexer *lexer)
{
	targum_lexer_clear_tokens(lexer);
	_setup_lexer(lexer, lexer->cfg);
}

TARGUM_API void targum_lexer_reset_token_index(struct TargumLexer *lexer)
{
	lexer->index = 0;
	lexer->curr_tok = harbol_vector_get(&lexer->tokens, lexer->index++);
}

TARGUM_API bool targum_lexer_generate_tokens(struct TargumLexer *const lexer)
{
	size_t line = 1;
	bool result = false;
	if( harbol_string_is_empty(&lexer->src) ) {
		targum_err(lexer->filename.cstr, "critical error", line, 0, "No source file loaded! Failed to generate tokens.");
		goto targum_lex_err_exit;
	} else if( lexer->cfg==NULL ) {
		targum_err(lexer->filename.cstr, "critical error", line, 0, "No config loaded! Failed to generate tokens.");
		goto targum_lex_err_exit;
	} else {
		struct HarbolLinkMap *const tokens = harbol_cfg_get_section(lexer->cfg, "tokens");
		if( tokens==NULL ) {
			targum_err(lexer->filename.cstr, "critical error", line, 0, "missing tokens section in config file/string! Failed to generate tokens.");
			goto targum_lex_err_exit;
		}
		struct HarbolLinkMap
			*const whitespace = harbol_cfg_get_section(tokens, "whitespace"),
			*const comments = harbol_cfg_get_section(tokens, "comments"),
			*const keywords = harbol_cfg_get_section(tokens, "keywords"),
			*const operators = harbol_cfg_get_section(tokens, "operators")
		;
		const bool golang_style = *harbol_cfg_get_bool(tokens, "use golang-style");
		if( keywords==NULL && operators==NULL ) {
			targum_err(lexer->filename.cstr, "critical error", line, 0, "Missing both keywords and operators sections in config file/string! Either have a keyword or operator section. Failed to generate tokens.");
			goto targum_lex_err_exit;
		}
		
		while( *lexer->iter != 0 ) {
			/// check white space if they're considered legit tokens.
			if( is_whitespace(*lexer->iter) ) {
				const int32_t s = *lexer->iter++;
				if( s=='\n' ) {
					line++;
					lexer->line_start = lexer->iter;
				}
				
				if( whitespace != NULL ) {
					const char *restrict whitespace_key = NULL;
					switch( s ) {
						case ' ' : whitespace_key = "space";   break;
						case '\t': whitespace_key = "tab";     break;
						case '\n': whitespace_key = "newline"; break;
					}
					intmax_t *token_value = harbol_cfg_get_int(whitespace, whitespace_key);
					if( token_value != NULL ) {
						struct TargumTokenInfo tok = {
							.start = (uintptr_t)(lexer->iter - lexer->src.cstr),
							.end = (uintptr_t)(lexer->iter - lexer->src.cstr + 1),
							.col = (uintptr_t)(lexer->iter - lexer->line_start),
							.line = line,
							.filename = &lexer->filename,
							.tag = *token_value
						};
						harbol_string_add_char(&tok.lexeme, s);
						harbol_vector_insert(&lexer->tokens, &tok);
					}
				}
				continue;
			} else if( is_alphabetic(*lexer->iter) ) {
				/// check identifiers or keywords.
				struct TargumTokenInfo tok = {
					.start = (uintptr_t)(lexer->iter - lexer->src.cstr),
					.col = (uintptr_t)(lexer->iter - lexer->line_start),
					.line = line,
					.filename = &lexer->filename,
				};
				while( *lexer->iter != 0 && is_possible_id(*lexer->iter) )
					harbol_string_add_char(&tok.lexeme, *lexer->iter++);
				
				tok.end = (uintptr_t)(lexer->iter - lexer->src.cstr);
				
				/// check if we got a keyword or identifier.
				intmax_t *const keyword = harbol_cfg_get_int(keywords, tok.lexeme.cstr);
				tok.tag = ( keyword != NULL ) ? *keyword : *harbol_cfg_get_int(tokens, "identifier");
				harbol_vector_insert(&lexer->tokens, &tok);
			} else if( is_decimal(*lexer->iter) || *lexer->iter=='.' ) {
				const bool dot = *lexer->iter=='.';
				/// Check number literal.
				struct TargumTokenInfo tok = {
					.start = (uintptr_t)(lexer->iter - lexer->src.cstr),
					.col = (uintptr_t)(lexer->iter - lexer->line_start),
					.line = line,
					.filename = &lexer->filename
				};
				bool is_float = false;
				char *end = NULL;
				const bool result = (golang_style ? lex_go_style_number : lex_c_style_number)(( const char* )lexer->iter, ( const char** )&end, &tok.lexeme, &is_float);
				if( (!result && !dot) ) {
					targum_err(lexer->filename.cstr, "error", line, (uintptr_t)(lexer->iter - lexer->line_start), "invalid number!");
					harbol_string_clear(&tok.lexeme);
					goto targum_lex_err_exit;
				} else if( !result && dot ) {
					/// invalid number, jump to the operators section.
					harbol_string_clear(&tok.lexeme);
					goto check_operators;
				} else {
					lexer->iter = end;
					tok.end = (uintptr_t)(lexer->iter - lexer->src.cstr);
					tok.tag = is_float ? *harbol_cfg_get_int(tokens, "float") : *harbol_cfg_get_int(tokens, "integer");
					harbol_vector_insert(&lexer->tokens, &tok);
				}
			} else {
			check_operators:;
				/// check operators and comments!
				if( comments != NULL ) {
					bool got_something = false;
					const struct HarbolKeyVal **const end = harbol_linkmap_get_iter_end_count(comments);
					for( const struct HarbolKeyVal **iter = harbol_linkmap_get_iter(comments); iter != NULL && iter<end; iter++ ) {
						/// Match largest operator first.
						const struct HarbolKeyVal *const kv = *iter;
						if( !strncmp(lexer->iter, kv->key.cstr, kv->key.len) ) {
							struct TargumTokenInfo tok = {
								.start = (uintptr_t)(lexer->iter - lexer->src.cstr),
								.col = (uintptr_t)(lexer->iter - lexer->line_start),
								.line = line,
								.filename = &lexer->filename
							};
							tok.tag = *harbol_cfg_get_int(tokens, "comment");
							const struct HarbolString *const end_comment = harbol_cfg_get_str(comments, kv->key.cstr);
							const bool result = ( end_comment==NULL || end_comment->len==0 )
									? lex_single_line_comment(( const char* )lexer->iter, ( const char** )&lexer->iter, &tok.lexeme)
									: lex_multi_line_comment(( const char* )lexer->iter, ( const char** )&lexer->iter, end_comment->cstr, end_comment->len, &tok.lexeme);
							if( !result ) {
								targum_err(lexer->filename.cstr, "error", line, (uintptr_t)(lexer->iter - lexer->line_start), "invalid %s comment!", ( end_comment==NULL || end_comment->len==0 ) ? "single-line" : "multi-line");
								harbol_string_clear(&tok.lexeme);
								goto targum_lex_err_exit;
							}
							tok.end = (uintptr_t)(lexer->iter - lexer->src.cstr);
							harbol_vector_insert(&lexer->tokens, &tok);
							got_something = true;
							break;
						}
					}
					if( got_something )
						continue;
				}
				
				/// placing this code here so we don't glitch out "string-like" comments.
				if( *lexer->iter=='\'' || *lexer->iter=='"' || (golang_style && *lexer->iter=='`') ) {
					/// check strings!
					const int32_t quote = *lexer->iter;
					struct TargumTokenInfo tok = {
						.start = (uintptr_t)(lexer->iter - lexer->src.cstr),
						.col = (uintptr_t)(lexer->iter - lexer->line_start),
						.line = line,
						.filename = &lexer->filename
					};
					const bool result = (golang_style ? lex_go_style_str : lex_c_style_str)(( const char* )lexer->iter, ( const char** )&lexer->iter, &tok.lexeme);
					if( !result ) {
						targum_err(lexer->filename.cstr, "error", line, (uintptr_t)(lexer->iter - lexer->line_start), (quote=='"' || (golang_style && quote=='`')) ? "invalid string!" : "invalid rune");
						harbol_string_clear(&tok.lexeme);
						goto targum_lex_err_exit;
					}
					tok.end = (uintptr_t)(lexer->iter - lexer->src.cstr);
					tok.tag = (quote=='"' || (golang_style && quote=='`')) ? *harbol_cfg_get_int(tokens, "string") : *harbol_cfg_get_int(tokens, "rune");
					harbol_vector_insert(&lexer->tokens, &tok);
					continue;
				}
				
				if( operators != NULL ) {
					size_t operator_size = 0;
					const struct HarbolKeyVal *match = NULL;
					const struct HarbolKeyVal **const end = harbol_linkmap_get_iter_end_count(operators);
					for( const struct HarbolKeyVal **iter = harbol_linkmap_get_iter(operators); iter != NULL && iter < end; iter++ ) {
						/// Match largest operator first.
						const struct HarbolKeyVal *const kv = *iter;
						if( !strncmp(lexer->iter, kv->key.cstr, kv->key.len) && operator_size < kv->key.len ) {
							operator_size = kv->key.len;
							match = kv;
						}
					}
					if( match != NULL ) {
						struct TargumTokenInfo tok = {
							.start = (uintptr_t)(lexer->iter - lexer->src.cstr),
							.col = (uintptr_t)(lexer->iter - lexer->line_start),
							.line = line,
							.filename = &lexer->filename,
						};
						const struct HarbolVariant *const v = ( const struct HarbolVariant* )match->data;
						tok.tag = *( const intmax_t* )v->data;
						harbol_string_add_str(&tok.lexeme, &match->key);
						lexer->iter += operator_size;
						tok.end = (uintptr_t)(lexer->iter - lexer->src.cstr);
						harbol_vector_insert(&lexer->tokens, &tok);
					} else {
						targum_err(lexer->filename.cstr, "error", line, (uintptr_t)(lexer->iter - lexer->line_start), "found no match for symbol(s) '%c' (%u) in operators section.", *lexer->iter, *lexer->iter);
						goto targum_lex_err_exit;
					}
				}
			}
		}
	}
	result = true;
targum_lex_err_exit:;
	struct TargumTokenInfo eof_tok = {
		.start = (uintptr_t)(lexer->iter - lexer->src.cstr),
		.col = (uintptr_t)(lexer->iter - lexer->line_start),
		.end = (uintptr_t)(lexer->iter - lexer->src.cstr),
		.line = line,
		.filename = &lexer->filename,
		.lexeme = harbol_string_create(""),
		.tag = 0
	};
	harbol_vector_insert(&lexer->tokens, &eof_tok);
	return lexer->tokens.count > 1 && result;
}

TARGUM_API bool targum_lexer_remove_token(struct TargumLexer *const lexer, const uint32_t tag)
{
	bool deleted_something = false;
	for( uindex_t i=0; i<lexer->tokens.count; i++ ) {
		struct TargumTokenInfo *const ti = harbol_vector_get(&lexer->tokens, i);
		if( ti->tag==tag ) {
			harbol_vector_del(&lexer->tokens, i, clear_token);
			i = 0;
			deleted_something |= true;
			continue;
		}
	}
	return deleted_something;
}

TARGUM_API bool targum_lexer_remove_comments(struct TargumLexer *const lexer)
{
	if( lexer->tokens.count <= 1 || lexer->cfg==NULL ) {
		return false;
	} else {
		const intmax_t *const value_ptr = harbol_cfg_get_int(lexer->cfg, "tokens.comment");
		return( value_ptr==NULL ) ? false : targum_lexer_remove_token(lexer, *value_ptr);
	}
}

TARGUM_API bool targum_lexer_remove_whitespace(struct TargumLexer *const lexer)
{
	bool deleted_something = false;
	if( lexer->tokens.count <= 1 || lexer->cfg==NULL ) {
		return deleted_something;
	} else {
		struct HarbolLinkMap *const whitespace = harbol_cfg_get_section(lexer->cfg, "tokens.whitespace");
		if( whitespace==NULL ) {
			return deleted_something;
		} else {
			const intmax_t *whitespaces[] = {
				harbol_cfg_get_int(whitespace, "space"),
				harbol_cfg_get_int(whitespace, "tab"),
				harbol_cfg_get_int(whitespace, "newline")
			};
			for( const intmax_t **iter=&whitespaces[0]; iter<1[&whitespaces]; iter++ ) {
				if( *iter==NULL )
					continue;
				else deleted_something = targum_lexer_remove_token(lexer, **iter);
			}
			return deleted_something;
		}
	}
}
