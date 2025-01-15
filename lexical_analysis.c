#include "ish.h"


int add_token(DynArray_T tokens, const char *buffer, int quoted) {
	Token *token = malloc(sizeof(Token));
	if (!token) {
		fprintf(stderr, "Error: Memory allocation failed\n");
		return -1;
	}
	strncpy(token->content, buffer, MAX_TOKEN_LENGTH);
	token->quoted = quoted;
	DynArray_add(tokens, token);
	return 0;
}

int lexical_analysis(const char *line, DynArray_T tokens) {
	const char *ptr = line;
	char buffer[MAX_TOKEN_LENGTH];
	int index = 0;
	int is_quote = 0;  
	int was_quote = 0;
	char quote_char = '\0';

	while (*ptr != '\0') {
		if (is_quote) {
			if (*ptr == quote_char) {
				is_quote = 0;
				was_quote = 1;
			} else {
				buffer[index++] = *ptr;
				if (index >= MAX_TOKEN_LENGTH) {
					fprintf(stderr, "Error: Token too long\n");
					return -1;
				}
			}
		} else {
			if (*ptr == '"' || *ptr == '\'') {
				is_quote = 1;
				quote_char = *ptr;
			} 
			else if (isspace(*ptr)) {
				if (index > 0) {
					buffer[index] = '\0';
					if (add_token(tokens, buffer, was_quote) == -1) {
						return -1;
					}
					index = 0;
					was_quote=0;
				}
			} 
			else if (*ptr == '|' || *ptr == '<' || *ptr == '>') {
				if (index > 0) {
					buffer[index] = '\0';
					if (add_token(tokens, buffer, 0) == -1) {
						return -1;
					}
					index = 0;
					was_quote=0;
				}
				buffer[0] = *ptr;
				buffer[1] = '\0';
				if (add_token(tokens, buffer, 0) == -1) {
					return -1;
				}
			} 
			else {
				buffer[index++] = *ptr;
				if (index >= MAX_TOKEN_LENGTH) {
					fprintf(stderr, "Error: Token too long\n");
					return -1;
				}
			}
		}
		ptr++;
	}

	if (is_quote) {
		fprintf(stderr, "Error: Unmatched quote\n");
		return -1;
	}

	if (index > 0) {
		buffer[index] = '\0';
		if (add_token(tokens, buffer, was_quote) == -1) {
			return -1;
		}
	}

	/*test
	printf("len : %d\n",DynArray_getLength(tokens));
	for(int i=0;i<DynArray_getLength(tokens);i++){
		Token *token = (Token *)DynArray_get(tokens, i);
		printf("test : %s %d\n",token->content,token->quoted);
	}
	//*/

	return 0;
}
