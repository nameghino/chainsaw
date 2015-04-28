#include <stdlib.h>
#include <string.h>

#ifdef TESTING
#include <stdio.h>
#endif

#include "jsmnSpark.h"

#define UNUSED_TOKEN -678
#define TOKEN_COUNT 32

class JSONParser {
private:
  jsmn_parser parser;
  jsmntok_t *tokens;
  size_t token_count;
  char *json;

  jsmntok_t* first_child_of(jsmntok_t *token, int *child_count) {
    int index = 0;
    int count = 0;
    while (tokens[index] != token) { index++; }
    jsmntok_t *first = tokens + index + 1;
    while (tokens[index]->parent == index) { child_count++; index++; }
    *child_count = count;
    return first;
  }

  char* string_value_for_token(jsmntok_t *token) {
    size_t buf_length = token->end - token->start;
    char *buf = (char *)calloc(sizeof(char), buf_length + 1);
    strncpy(buf, this->json + token->start, buf_length);
    return buf;
  }

  jsmntok_t* key_token_for_key(char *key) {
    jsmntok_t *token = NULL;
    for(int i=0; i < this->token_count; ++i) {
      jsmntok_t *t = tokens + i;
      char *token_key = string_value_for_token(t);
      if (strcmp(token_key, key) == 0) {
        free(token_key);
        return t;
      }
      free(token_key);
    }

    int starting_index = 0;
    if (token == NULL) {
      // try drilling down
      for (char *k = strtok(key, '.');
      k != NULL;
      k = strtok(key, '.')) {

      }
    }

    return token;
  }

  jsmntok_t* value_token_for_key(char *key) {
    jsmntok_t *t = key_token_for_key(key);
    if (t->parent == -1) {
      return NULL; // no parent
    }

    jsmntok_t *parent = tokens + (t->parent);
    if (parent->type != JSMN_OBJECT) { return NULL; }
    return t + 1;
  }

  char type_identifier(jsmntok_t *token) {
    switch(token->type) {
      case JSMN_PRIMITIVE:
      return 'P';
      case JSMN_OBJECT:
      return 'O';
      case JSMN_ARRAY:
      return 'A';
      case JSMN_STRING:
      return 'S';
      default:
      return 'X';
    }
  }

  jsmntok_t* token_at_index(int i) {
    return tokens + i;
  }

public:
  ~JSONParser() {
    free(tokens);
  }

  void test() {
    for(int i=0; i < token_count; ++i) {
      jsmntok_t *t = tokens + i;
      char *s = string_value_for_token(t);
      printf("[%c;%04d;%04d;%02d;%02d]%s\n", type_identifier(t), t->start, t->end, t->size, t->parent, s);
      free(s);
    }
  }

  int parse(char *json) {
    this->json = json;
    jsmn_init(&parser);
    int token_count = 10;
    int r = 0;
    tokens = (jsmntok_t*) calloc(sizeof(jsmntok_t), token_count);
    for (int i=0; i < token_count; ++i) { tokens[i].start = UNUSED_TOKEN; }
    do {
      r = jsmn_parse(&parser, json, tokens, token_count);
      if (r == JSMN_ERROR_NOMEM) {
        token_count *= 2;
        tokens = (jsmntok_t*) realloc(tokens, sizeof(jsmntok_t) * token_count);
        for (int i=0; i < token_count; ++i) { tokens[i].start = UNUSED_TOKEN; }
      }
    } while(r == JSMN_ERROR_NOMEM);

    r=0;
    while (tokens[r].start != UNUSED_TOKEN) { r++; }
    tokens = (jsmntok_t*) realloc(tokens, sizeof(jsmntok_t) * r);
    this->token_count = r;
    return r;
  }

  char* string_for_key(char *key) {
    jsmntok_t *token = value_token_for_key(key);
    if (token != NULL) {
      return string_value_for_token(token);
    }
    return NULL;
  }

  int int_for_key(char *key) {
    char *value = this->string_for_key(key);
    int i = atoi(value);
    free(value);
    return i;
  }

  float float_for_key(char *key) {
    char *value = this->string_for_key(key);
    float f = atof(value);
    free(value);
    return f;
  }

};

int test();

int main() {
  return test();
}

int test() {
  char *json0 = "{\"message\": \"Hello, world!\", \"timestamp\": 123456789}";
  char *json1 = "{\"response\": {\"message\": \"Hello, world!\", \"timestamp\": 123456789}}";
  char *json2 = "[1,2,3,4,5]";
  char *json3 = "chupala";
  char *test_data[] = {json0, json1, json2, json3, NULL};
  for (char **ptr = test_data; *ptr != NULL; ++ptr) {
    printf("=== TEST BEGIN ===\n");
    JSONParser *p = new JSONParser();
    int n = p->parse(*ptr);
    printf("got %d tokens\n", n);
    p->test();

    if (*ptr == json0) {
      char *v = p->string_for_key("message");
      printf("key-based access test: %s\n", v);
      free(v);

      int x = p->int_for_key("timestamp");
      printf("key-based access test: %d\n", x);
    }

    printf("=== TEST END ===\n");
  }
  return 0;
}
