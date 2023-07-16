#pragma once
#include <inttypes.h>
#include <assert.h>

#define BIT_SET(number, bit)    (number |= (1 << bit))
#define BIT_ZERO(number, bit)   (number &= ~(1 << bit))
#define BIT_TOGGLE(number, bit) (number ^= 1 << bit)
#define BIT_TEST(number, bit)   (number & (1 << bit))

#define BITSET(type, x)\
typedef struct {\
	type bits;\
	size_t length;\
} Bitset##x##;\
\
static Bitset##x bitset_create_##x() {\
	Bitset##x bitset;\
	bitset.bits = 0;\
	bitset.length = x;\
	return bitset;\
}\
static inline void bitset_set_##x(Bitset##x##* bitset, size_t bit) {\
	assert(bitset->length > bit && "bit is out of range!");\
	BIT_SET(bitset->bits, bit);\
}\
\
static inline void bitset_zero_##x(Bitset##x##* bitset, size_t bit) {\
	assert(bitset->length > bit && "bit is out of range!");\
	BIT_ZERO(bitset->bits, bit);\
}\
\
static inline void bitset_toggle_##x(Bitset##x##* bitset, size_t bit) {\
	assert(bitset->length > bit && "bit is out of range!");\
	BIT_TOGGLE(bitset->bits, bit);\
}\
\
static inline bool bitset_test_##x(Bitset##x##* bitset, size_t bit) {\
	assert(bitset->length > bit && "bit is out of range!");\
	return BIT_TEST(bitset->bits, bit);\
}\
static inline void bitset_clear_##x(Bitset##x##* bitset) {\
	bitset->bits = 0;\
}\
static inline bool bitset_and_compare_##x(Bitset##x##* a, Bitset##x##* b) {\
	return (a->bits & b->bits) == b->bits;\
}\

BITSET(char, 8)
BITSET(uint16_t, 16)
BITSET(uint32_t, 32)