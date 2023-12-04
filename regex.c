#include "utils.c"

A match(A pattern, A str) {
    INIT_REGEX(pattern, str, regex, einfo, region, pattr, inp)
    int r = onig_search(regex, inp, inp + strlen((char *)inp), inp, inp + strlen((char *)inp), region, ONIG_OPTION_NONE);
    K result = (r >= 0) ? Ki(region->beg[0]) : -1;
    cleanup_regex(regex, region);
    return result;
}

A indices(A pattern, A str) {
    INIT_REGEX(pattern, str, regex, einfo, region, pattr, inp)
    I *res = NULL; int res_size = 0;
    UChar *start = inp, *end = inp + strlen((char *)inp), *range = end;

    while (start <= end) {
        int r = onig_search(regex, inp, end, start, range, region, ONIG_OPTION_NONE);
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
            free(res);
            cleanup_regex(regex, region);
            return Ki(-1);
        }
    }

    K ret = KI(res, res_size);
    free(res); cleanup_regex(regex, region);
    return ret;
}

A substrings(A pattern, A str) {
    INIT_REGEX(pattern, str, regex, einfo, region, pattr, inp)
    char **substrings = NULL;
    int substrings_count = 0;
    UChar *start = inp, *end = inp + strlen((char *)inp);

    while (start <= end) {
        int r = onig_search(regex, inp, end, start, end, region, ONIG_OPTION_NONE);
        if (r >= 0) {
            for (int i = 1; i < region->num_regs; i++) {
                int length = region->end[i] - region->beg[i];
                substrings = realloc(substrings, (substrings_count + 1) * sizeof(char*));
                substrings[substrings_count] = (char*)malloc((length + 1) * sizeof(char));
                strncpy(substrings[substrings_count], (const char *)(inp + region->beg[i]), length);
                substrings[substrings_count][length] = '\0';
                substrings_count++;
            }
            start = inp + region->end[0];
        } else if (r == ONIG_MISMATCH) {
            break;
        } else {
            for (int i = 0; i < substrings_count; i++) free(substrings[i]);
            free(substrings);
            cleanup_regex(regex, region);
            return Ki(-1);
        }
    }

    K ret = KS(substrings, substrings_count);
    for (int i = 0; i < substrings_count; i++) free(substrings[i]);
    free(substrings);
    cleanup_regex(regex, region);
    return ret;
}

A replace(A pattern, A str, A replacement) {
    INIT_REGEX(pattern, str, regex, einfo, region, pattr, inp)
    int offset = 0, total_len = strlen((char *)inp);
    char *result = malloc(total_len + 1);
    result[0] = '\0';

    while (offset < total_len) {
        int r = onig_search(regex, inp, inp + total_len, inp + offset, inp + total_len, region, ONIG_OPTION_NONE);
        if (r >= 0) {
            strncat(result, (const char *)inp + offset, region->beg[0] - offset);
            strcat(result, (const char *)replacement);
            offset = region->end[0];
            result = realloc(result, strlen(result) + total_len - offset + strlen((char *)replacement) + 1);
        } else if (r == ONIG_MISMATCH) {
            strcat(result, (const char *)inp + offset);
            break;
        } else {
            free(result);
            cleanup_regex(regex, region);
            return Ki(-1);
        }
    }

    K ret = KC(result, strlen(result));
    free(result); cleanup_regex(regex, region);
    return ret;
}

