#include "../../a.h"
#include "../../k.h"
#include "/opt/homebrew/include/oniguruma.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int init_regex(OnigRegex *regex, OnigErrorInfo *einfo, const A pattern, const A str, UChar **pattr, UChar **inp) {
    *pattr = malloc(NK(pattern) + 1);
    if (*pattr == NULL) return ONIGERR_MEMORY;
    memcpy(*pattr, (const void *)pattern, NK(pattern));
    (*pattr)[NK(pattern)] = '\0';
    *inp = malloc(NK(str) + 1);
    if (*inp == NULL) { free(*pattr); return ONIGERR_MEMORY; }
    memcpy(*inp, (const void *)str, NK(str));
    (*inp)[NK(str)] = '\0';
    return onig_new(regex, *pattr, *pattr + strlen((char *)*pattr), ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, einfo);
}
#define INIT_REGEX(pattern, str, regex, einfo, region, pattr, inp) \
    OnigRegex regex; \
    OnigErrorInfo einfo; \
    OnigRegion *region; \
    UChar *pattr, *inp; \
    if (init_regex(&regex, &einfo, pattern, str, &pattr, &inp) != ONIG_NORMAL) { \
        return Ki(-1); \
    } \
    region = onig_region_new();

void cleanup_regex(OnigRegex regex, OnigRegion *region) {
    onig_region_free(region, 1); 
    onig_free(regex);
}

