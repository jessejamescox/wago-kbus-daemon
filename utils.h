#ifndef __UTILS_H__
#define __UTILS_H__

//Set verbose level
extern int vlevel; /**< @brief Variable for verbosity level needed by many modules\nDeclaration is in main.c*/

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define UNUSED(x) ((void)x)
/**
 * @name Verbosity_Level
 * @brief Definiton of several verbosity_levels
 * @{
 */
#define VERBOSE_STD     1
#define VERBOSE_INFO    2
#define VERBOSE_GOSSIPY 3
#define VERBOSE_DEBUG   7
/**
 * @}
 */

/**
 * @brief Definition of helper function.\n It allows to print out debug infromation at severals
 * verbosity level.
 *
 * @param[in] printlevel At given minimum level it will be printed
 * @param[in] format Like printf format
 * @param[in] ... VA_ARGS like printf
 */
#define dprintf(printlevel, format, ...) do {       \
        if (vlevel>=printlevel)                     \
            fprintf(stderr, format, ##__VA_ARGS__); \
} while(0)

#ifndef VERSION
#define VERSION "Development"
#endif

/**
 * @brief Return values for str2int
 */
typedef enum {
	STR2INT_SUCCESS,
	STR2INT_OVERFLOW,
	STR2INT_UNDERFLOW,
	STR2INT_INCONVERTIBLE
} str2int_errno;

str2int_errno str2int(int *out, char *s, int base);
void utils_hexdump(uint8_t *memptr, size_t len);

/**
 * @brief Returns full bytes on given bit count
 * @param[in] bitcnt
 * @return Number of bytes for bit count
 */
#define utils_bitCountToByte(bitcnt) ((bitcnt/8) + ((bitcnt % 8) ? 1 : 0))

#endif /* __UTILS_H__ */
