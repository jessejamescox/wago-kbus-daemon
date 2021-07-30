//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     utils.c
///
///  \brief    Holds a collection of useful tool/functions
///
///  \author   <BrT> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "utils.h"

/*
 * Convert string s to int out.
 *
 *
 * The format is the same as strtol,
 * except that the following are inconvertible:
 *
 * - empty string
 * - leading whitespace
 * - any trailing characters that are not part of the number
 *
 * Cannot be NULL.
 *
 * @param[in] base Base to interpret string in. Same range as strtol (2 to 36).
 * @param[out] out The converted int. Cannot be NULL.
 * @param[in] s Input string to be converted.
 *
 * @return Indicates if the operation succeeded, or why it failed.
 */
str2int_errno str2int(int *out, char *s, int base) 
{
	char *end;
	if (s[0] == '\0' || isspace((unsigned char) s[0]))
		return STR2INT_INCONVERTIBLE;
	errno = 0;
	long l = strtol(s, &end, base);
	/* Both checks are needed because INT_MAX == LONG_MAX is possible. */
	if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
		return STR2INT_OVERFLOW;
	if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
		return STR2INT_UNDERFLOW;
	if (*end != '\0')
		return STR2INT_INCONVERTIBLE;
	*out = l;
	return STR2INT_SUCCESS;
}

/**
 * @brief Hexdump utiliy wich creates a dump from given pointer and length
 *
 * @param[in] memptr Pointer to memeory location
 * @param[in] len Number of bytes dumped
 */
void utils_hexdump(uint8_t *memptr, size_t len)
{
	size_t i = 0;
	for (i = 0; i < len; i++)
	{
		if ((i % 16) == 0)
			printf("\n%04X: ", i);
		printf("%02X ", memptr[i]);
	}
	printf("\n");
}

