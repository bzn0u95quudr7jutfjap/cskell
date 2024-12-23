#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack/stack.h"
#include "string_class.h"
#include <stack.h>

u0 set_newline(Token *p, Token *t, TokenEnv *env) {
  p->newline_after = 0 < p->newline_after ? p->newline_after : 1;
  p->space_after   = 0;
  t->indentation   = env->indentation;
}

u8 in_definitionspace(TokenEnv *env) {
  return 0 == env->globalspace && 0 < env->definitionspace;
}

u0 formatter(CodeTokens *codetokens) {
  TokenEnv env = {};
  u32      len = codetokens->tokens.size;
  Token   *t4  = NULL;
  Token   *t3  = NULL;
  Token   *t2  = NULL;
  Token   *t1  = NULL;
  Token   *p   = NULL;
  Token   *t   = NULL;
  if (0 < len) {
    t              = &codetokens->tokens.data[0];
    t->space_after = 1;
  }
  for (u32 i = 1; i < len; i++, p = t) {
    t4 = 4 < i ? &codetokens->tokens.data[i - 4] : NULL;
    t3 = 3 < i ? &codetokens->tokens.data[i - 3] : NULL;
    t2 = 2 < i ? &codetokens->tokens.data[i - 2] : NULL;
    t1 = &codetokens->tokens.data[i - 1];
    p  = &codetokens->tokens.data[i - 1];
    t  = &codetokens->tokens.data[i];
    if (0 < p->newline_after) {
      t->indentation = env.indentation;
    }
    switch (t->type) {
    case TOKEN_OPERATOR_UNARY:
      t->space_after = 0;
      break;
    case TOKEN_OPERATOR_BINARY:
      p->space_after = 1;
      t->space_after = 1;
      break;
    case TOKEN_OPERATOR_PREPOSTFIX: {
      u8 prefix      = p->type == TOKEN_WORD || p->type == TOKEN_EXPR_END;
      p->space_after = prefix;
      t->space_after = !prefix;
    } break;
    case TOKEN_OPERATOR_AMBIGUOUS: {
      u8 prefix      = p->type == TOKEN_WORD || p->type == TOKEN_EXPR_END;
      p->space_after = prefix ? 1 : p->space_after;
      t->space_after = prefix;
    } break;
    case TOKEN_WORD:
      if (p->type == TOKEN_BLOCK_END) {
        p->newline_after = 0;
        p->space_after   = 1;
      }
      t->space_after = 1;
      break;
    case TOKEN_OPERATOR_ATTRIBUTE:
      if (in_definitionspace(&env)) {
        p->space_after = 1;
        t->space_after = 0;
      } else {
        p->space_after = 0;
        t->space_after = 0;
      }
      break;
    case TOKEN_COMA:
      if (in_definitionspace(&env)) {
        set_newline(p, t, &env);
        t->space_after = 1;
      } else {
        p->space_after = 0;
        t->space_after = 1;
      }
      break;
    case TOKEN_END_OF_LINE:
      if (env.indentation == 0) {
        p->space_after   = 0;
        p->newline_after = 0;
        set_newline(p, t, &env);
        t->indentation   = 1;
        t->newline_after = 2;
      } else {
        set_newline(p, t, &env);
        p->space_after = 0;
        t->space_after = 1;
      }
      break;
    case TOKEN_COMMENT_SL:
    case TOKEN_COMMENT_ML:
      set_newline(p, t, &env);
      t->newline_after = 1;
      break;
    case TOKEN_EXPR_BEGIN:
    case TOKEN_ARRAY_I_BEGIN:
      if (in_definitionspace(&env)) {
        env.globalspace = 1;
      }
      if (p->type != TOKEN_OPERATOR_BINARY) {
        p->space_after = 0;
      }
      if (p->type == TOKEN_COMA) {
        p->space_after = 1;
      }
      t->space_after = 0;
      break;
    case TOKEN_EXPR_END:
    case TOKEN_ARRAY_I_END:
      if (env.definitionspace > 0) {
        env.globalspace = 0;
      }
      if (p->definition == 1) {
        p->newline_after = 1;
        t->indentation   = p->indentation;
        t->newline_after = 0;
      } else {
        p->space_after = 0;
        t->space_after = 0;
      }
      break;
    case TOKEN_BLOCK_BEGIN:
      u8 casting        = 1;
      casting          &= t3 != NULL && t3->type == TOKEN_EXPR_BEGIN;
      casting          &= t2 != NULL && t2->type == TOKEN_WORD;
      casting          &= t1 != NULL && t1->type == TOKEN_EXPR_END;
      u8 assignment     = t1 != NULL && t1->type == TOKEN_OPERATOR_BINARY;
      u8 in_definition  = casting || assignment || in_definitionspace(&env);
      if (in_definition) {
        env.definitionspace += 1;
        env.globalspace      = 0;
        t->definition        = 1;
      }
      if (casting) {
        if (t4 != NULL) {
          t4->newline_after = 1;
          t3->indentation   = env.indentation + 1;
        }
      }
      env.indentation += 1;
      set_newline(p, t, &env);
      t->space_after = 1;
      break;
    case TOKEN_BLOCK_END:
      set_newline(p, t, &env);
      env.indentation  += -(env.indentation > 0);
      t->newline_after  = env.indentation == 0 ? 2 : 1;
      if (in_definitionspace(&env)) {
        env.definitionspace += -(env.definitionspace > 0);
        t->definition        = 1;
      }
      break;
    case TOKEN_MACRO_END:
      env.indentation  += -(env.indentation > 0);
      t->newline_after  = 1;
      break;
    case TOKEN_MACRO_BEGIN:
      p->newline_after  = p->type == TOKEN_MACRO_END ? 0 : 2;
      env.indentation  += 1;
      t->space_after    = 1;
#if 0
      // if (es(&macrocmd, &inc)) {
      if (0) {
        t = iseekcur(iter, 1);
        while (t->type != TOKEN_MACRO_END) {
          t->space_after   = 0;
          t->newline_after = 0;
          t                = iseekcur(iter, 1);
        }
        iseekcur(iter, -1);
      }
#endif
      break;
    default:
      break;
    }
  }
}
