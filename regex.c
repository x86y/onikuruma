#include "../../a.h"
#include "../../k.h"
#include "/opt/homebrew/include/oniguruma.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

A match(A pattern, A str) {
  OnigRegex regex; OnigErrorInfo einfo; OnigRegion *region;
  char patt[NK(pattern) + 1]; CK(patt, pattern); patt[NK(pattern)] = '\0';
  char inpp[NK(str) + 1]; CK(inpp, str); inpp[NK(str)] = '\0';
  UChar *pattr = (UChar *)patt; UChar *inp = (UChar *)inpp;

  int r = onig_new(&regex, pattr, pattr + strlen((char *)pattr), ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) return -1;

  region = onig_region_new();
  r = onig_search(regex, inp, inp + strlen((char *)inp), inp, inp + strlen((char *)inp), region, ONIG_OPTION_NONE);

  K result;
  if (r >= 0) result = Ki(region->beg[0]); // position of the match
  else result = -1;

  onig_region_free(region, 1); onig_free(regex);
  return result;
}

A indices(A pattern, A str) {
    OnigRegex regex; OnigErrorInfo einfo; OnigRegion *region;
    char patt[NK(pattern) + 1]; CK(patt, pattern); patt[NK(pattern)] = '\0';
    char inpp[NK(str) + 1]; CK(inpp, str); inpp[NK(str)] = '\0';
    UChar *pattr = (UChar *)patt; UChar *inp = (UChar *)inpp;

    int r = onig_new(&regex, pattr, pattr + strlen((char *)pattr), ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);
    if (r != ONIG_NORMAL) return Ki(-1);
    region = onig_region_new();
    I *res = NULL; int res_size = 0;

    UChar *start = inp;
    UChar *end = inp + strlen((char *)inp);
    UChar *range = end;

    while (start <= end) {
        r = onig_search(regex, inp, end, start, range, region, ONIG_OPTION_NONE);

        if (r >= 0) {
            res = realloc(res, (res_size + region->num_regs * 2) * sizeof(I));
            for (int i = 0; i < region->num_regs; i++) {
                res[res_size + i * 2] = Ki(region->beg[i]);
                res[res_size + i * 2 + 1] = Ki(region->end[i]);
            }
            res_size += region->num_regs * 2;

            start = inp + region->end[0];
        } else if (r == ONIG_MISMATCH) {
            break;
        } else {
            free(res); onig_region_free(region, 1); onig_free(regex);
            return Ki(-1);
        }
    }

    K ret = KI(res, res_size);
    free(res); onig_region_free(region, 1); onig_free(regex);
    return ret;
}


A substrings(A pattern, A str) {
    OnigRegex regex; OnigErrorInfo einfo; OnigRegion *region;
    char patt[NK(pattern) + 1]; CK(patt, pattern); patt[NK(pattern)] = '\0';
    char inpp[NK(str) + 1]; CK(inpp, str); inpp[NK(str)] = '\0';
    UChar *pattr = (UChar *)patt; UChar *inp = (UChar *)inpp;

    int r = onig_new(&regex, pattr, pattr + strlen((char *)pattr), ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);
    if (r != ONIG_NORMAL) return Ki(-1);

    region = onig_region_new();
    char **substrings = NULL;
    int substrings_count = 0;

    UChar *start = inp;
    UChar *end = inp + strlen((char *)inp);

    while (start <= end) {
        r = onig_search(regex, inp, end, start, end, region, ONIG_OPTION_NONE);
        if (r >= 0) {
            for (int i = 1; i < region->num_regs; i++) {  // Start from 1 to skip the full match
                int length = region->end[i] - region->beg[i];
                substrings = realloc(substrings, (substrings_count + 1) * sizeof(char*));
                substrings[substrings_count] = (char*)malloc((length + 1) * sizeof(char));
                strncpy(substrings[substrings_count], (const char *)(inp + region->beg[i]), length);
                substrings[substrings_count][length] = '\0';
                substrings_count++;
            }
            start = inp + region->end[0]; // Update start to the end of the full match
        } else if (r == ONIG_MISMATCH) {
            break;
        } else {
            for (int i = 0; i < substrings_count; i++) free(substrings[i]); free(substrings); onig_region_free(region, 1); onig_free(regex);
            return Ki(-1);
        }
    }

    K ret = KS(substrings, substrings_count);
    for (int i = 0; i < substrings_count; i++) free(substrings[i]);
    free(substrings); onig_region_free(region, 1); onig_free(regex);
    return ret;
}

A replace(A pattern, A str, A replacement) {
    OnigRegex regex; OnigErrorInfo einfo; OnigRegion *region;
    char patt[NK(pattern) + 1]; CK(patt, pattern); patt[NK(pattern)] = '\0';
    char inpp[NK(str) + 1]; CK(inpp, str); inpp[NK(str)] = '\0';
    char repl[NK(replacement) + 1]; CK(repl, replacement); repl[NK(replacement)] = '\0';
    UChar *pattr = (UChar *)patt; UChar *inp = (UChar *)inpp;

    int r = onig_new(&regex, pattr, pattr + strlen((char *)pattr), ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);
    if (r != ONIG_NORMAL) return Ki(-1);

    region = onig_region_new();
    int offset = 0, total_len = strlen(inpp);
    char *result = malloc(total_len + 1);
    result[0] = '\0';

    while (offset < total_len && (r = onig_search(regex, inp, inp + total_len, inp + offset, inp + total_len, region, ONIG_OPTION_NONE)) >= 0) {
        strncat(result, (const char *)inp + offset, region->beg[0] - offset); // Add non-matched part
        strcat(result, repl); // Add replacement
        offset = region->end[0]; // Update offset
        result = realloc(result, strlen(result) + total_len - offset + strlen(repl) + 1);
    }

    if (offset < total_len) strcat(result, (const char *)inp + offset);

    K ret = KC(result, strlen(result));
    free(result); onig_region_free(region, 1); onig_free(regex);
    return ret;
}
