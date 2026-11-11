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
    TYPE_STRING,
    TYPE_VOID
} kValueType;

typedef struct {
    kValueType type;
    union {
        int i;
        double d;
        bool b;
        kString str;
    } value;
} kVariant;

static int kValGetInt(const kVariant *variant){
    return variant->type == TYPE_INT ? variant->value.i : 0;
}
static double kValGetDouble(const kVariant *variant){
    return variant->type == TYPE_DOUBLE ? variant->value.d : 0.0f;
}
static kString* kValGetString(const kVariant *variant){
    return variant->type == TYPE_STRING ? (kString*) &variant->value.str : (kString*) nullptr;
}

static void kValSetInt(kVariant *variant, const int value){
    variant->type = TYPE_INT;
    variant->value.i = value;
}
static void kValSetDouble(kVariant *variant, const double value){
    variant->type = TYPE_DOUBLE;
    variant->value.d = value;
}
static void kValSetString(kVariant *val, const char *str) {
    if (val->type == TYPE_STRING) {
        kStrSet(&val->value.str, str);
    } else {
        val->type = TYPE_STRING;
        kStrMake(&val->value.str);
        kStrSet(&val->value.str, str);
    }
}

static void kValAsString(const kVariant *variant, char *buffer, const size_t size){
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

static const char *kTypeToString(const kValueType type){
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

static kValueType kTypeofVariant(const kVariant *variant){
    return variant->type;
}

static kVariant kDefaultValueOfType(const kValueType type) {
    kVariant var;
    switch (type) {
        case TYPE_INT:
            kValSetInt(&var, 0);
            break;
        case TYPE_DOUBLE:
            kValSetDouble(&var, 0);
            break;
        case TYPE_STRING:
            kValSetString(&var, "");
            break;
    }
    return var;
}

#endif //KORD_KVARIANT_H
