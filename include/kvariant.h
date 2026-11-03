//
// Created by piti on 11/3/26.
//

#ifndef KORD_KVARIANT_H
#define KORD_KVARIANT_H

#include <stdio.h>
#include "kstring.h"

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING
} ValueType;

typedef struct {
    ValueType type;
    union {
        int i;
        double d;
        kString str;
        bool b;
    } value;
} kVariant;

static inline int kValGetInt(const kVariant *variant){
    return variant->type == TYPE_INT ? variant->value.i : 0;
}
static inline double kValGetDouble(const kVariant *variant){
    return variant->type == TYPE_DOUBLE ? variant->value.d : 0.0f;
}
static kString* kValGetString(const kVariant *variant){
    return variant->type == TYPE_STRING ? (kString*) &variant->value.str : (kString*) nullptr;
}

static inline void kValSetInt(kVariant *variant, const int value){
    variant->type = TYPE_INT;
    variant->value.i = value;
}
static inline void kValSetFloat(kVariant *variant, const double value){
    variant->type = TYPE_DOUBLE;
    variant->value.d = value;
}
static inline void kValSetString(kVariant *val, const char *str) {
    if (val->type == TYPE_STRING) {
        kStrSet(&val->value.str, str);
    } else {
        val->type = TYPE_STRING;
        kStrMake(&val->value.str);
        kStrSet(&val->value.str, str);
    }
}

static inline void kValAsString(const kVariant *variant, char *buffer, const size_t size){
    switch (variant->type) {
        case TYPE_INT:
            snprintf(buffer, size, "%d", variant->value.i);
            break;
        case TYPE_DOUBLE:
            snprintf(buffer, size, "%f", variant->value.d);
            break;
        case TYPE_STRING:
            snprintf(buffer, size, "%s", variant->value.str.buffer);
            break;
        default:
            snprintf(buffer, size, "null");
            break;
    }
}

static inline const char *kTypeToString(const ValueType type){
    switch (type)
    {
    case TYPE_INT:
        return "int";
    case TYPE_DOUBLE:
        return "float";
    case TYPE_STRING:
        return "string";
    default:
        return "unknown";
    }
}

static inline ValueType kTypeofVariant(const kVariant *variant){
    return variant->type;
}

#endif //KORD_KVARIANT_H
