#include "minicc.h"

Type *ty_int    = &(Type){TY_INT,    4, 4, false};
Type *ty_long   = &(Type){TY_LONG,   8, 8, false};
Type *ty_char   = &(Type){TY_CHAR,   1, 1, false};
Type *ty_short  = &(Type){TY_SHORT,  2, 2, false};
Type *ty_void   = &(Type){TY_VOID,   1, 1, false};
Type *ty_bool   = &(Type){TY_BOOL,   1, 1, false};

Type *ty_uint   = &(Type){TY_INT,    4, 4, true};
Type *ty_ulong  = &(Type){TY_LONG,   8, 8, true};
Type *ty_uchar  = &(Type){TY_CHAR,   1, 1, true};
Type *ty_ushort = &(Type){TY_SHORT,  2, 2, true};

Type *ty_float  = &(Type){TY_FLOAT,  4, 4, false};
Type *ty_double = &(Type){TY_DOUBLE, 8, 8, false};

bool is_integer(Type *ty) {
    return ty->kind == TY_INT || ty->kind == TY_LONG ||
           ty->kind == TY_CHAR || ty->kind == TY_SHORT ||
           ty->kind == TY_BOOL;
}

bool is_flonum(Type *ty) {
    return ty->kind == TY_FLOAT || ty->kind == TY_DOUBLE;
}

bool is_numeric(Type *ty) {
    return is_integer(ty) || is_flonum(ty);
}

Type *pointer_to(Type *base) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_PTR;
    ty->size = 8;
    ty->align = 8;
    ty->base = base;
    return ty;
}

Type *array_of(Type *base, int len) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_ARRAY;
    ty->size = base->size * len;
    ty->align = base->align;
    ty->base = base;
    ty->array_len = len;
    return ty;
}

Type *func_type(Type *return_ty) {
    Type *ty = calloc(1, sizeof(Type));
    ty->kind = TY_FUNC;
    ty->size = 1;
    ty->align = 1;
    ty->return_ty = return_ty;
    return ty;
}

// Usual arithmetic conversions: promote both operands to the larger type.
Type *get_common_type(Type *ty1, Type *ty2) {
    if (ty1->base)
        return pointer_to(ty1->base);

    if (ty1->kind == TY_DOUBLE || ty2->kind == TY_DOUBLE)
        return ty_double;
    if (ty1->kind == TY_FLOAT || ty2->kind == TY_FLOAT)
        return ty_float;

    if (ty1->size == 8 || ty2->size == 8) {
        if (ty1->is_unsigned || ty2->is_unsigned)
            return ty_ulong;
        return ty_long;
    }

    if (ty1->is_unsigned || ty2->is_unsigned)
        return ty_uint;
    return ty_int;
}

void add_type(Node *node) {
    if (!node) return;

    add_type(node->lhs);
    add_type(node->rhs);
    add_type(node->cond);
    add_type(node->then);
    add_type(node->els);
    add_type(node->init);
    add_type(node->inc);

    for (Node *n = node->body; n; n = n->next)
        add_type(n);
    for (Node *n = node->args; n; n = n->next)
        add_type(n);

    if (node->ty) return; // already typed

    switch (node->kind) {
    case ND_ADD:
    case ND_SUB:
        if (node->lhs->ty->base) {
            node->ty = node->lhs->ty;
            return;
        }
        if (node->rhs->ty && node->rhs->ty->base) {
            node->ty = node->rhs->ty;
            return;
        }
        node->ty = get_common_type(node->lhs->ty, node->rhs->ty);
        return;
    case ND_MUL:
    case ND_DIV:
    case ND_MOD:
    case ND_BITAND:
    case ND_BITOR:
    case ND_BITXOR:
    case ND_SHL:
    case ND_SHR:
        if (node->lhs->ty && node->rhs->ty)
            node->ty = get_common_type(node->lhs->ty, node->rhs->ty);
        else
            node->ty = ty_int;
        return;
    case ND_NEG:
    case ND_BITNOT:
        node->ty = node->lhs->ty;
        return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_LOGAND:
    case ND_LOGOR:
    case ND_NOT:
        node->ty = ty_int;
        return;
    case ND_FUNCALL:
        node->ty = ty_int;
        return;
    case ND_NUM:
        node->ty = ty_int;
        return;
    case ND_TERNARY:
        node->ty = node->then->ty;
        return;
    case ND_COMMA:
        node->ty = node->rhs->ty;
        return;
    case ND_ASSIGN:
    case ND_ADD_EQ:
    case ND_SUB_EQ:
    case ND_MUL_EQ:
    case ND_DIV_EQ:
    case ND_MOD_EQ:
    case ND_AND_EQ:
    case ND_OR_EQ:
    case ND_XOR_EQ:
    case ND_SHL_EQ:
    case ND_SHR_EQ:
    case ND_PRE_INC:
    case ND_PRE_DEC:
    case ND_POST_INC:
    case ND_POST_DEC:
        node->ty = node->lhs->ty;
        return;
    case ND_VAR:
        node->ty = node->var->ty;
        return;
    case ND_MEMBER:
        node->ty = node->member->ty;
        return;
    case ND_ADDR:
        node->ty = pointer_to(node->lhs->ty);
        return;
    case ND_DEREF:
        if (!node->lhs->ty->base)
            error("invalid pointer dereference");
        node->ty = node->lhs->ty->base;
        return;
    }
}
