#include "utils.h"

K match(K pattern, K str) {
    INIT_REGEX(pattern, str, regex, einfo, region, pattr, inp)
    int r = onig_search(regex, inp, inp + strlen((char *)inp), inp, inp + strlen((char *)inp), region, ONIG_OPTION_NONE);
    K result = (r >= 0) ? Ki(region->beg[0]) : -1;
    cleanup_regex(regex, region);
    return result;
}

K indices(K pattern, K str) {
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

K substrings(K pattern, K str) {
    kinit();
    INIT_REGEX(pattern, str, regex, einfo, region, pattr, inp)
    UChar *start = inp, *end = inp + strlen((char *)inp);
    K result=Kx("()");
    while (start <= end) {
        int r = onig_search(regex, inp, end, start, end, region, ONIG_OPTION_NONE);
        if (r >= 0) {
            for (int i = 1; i < region->num_regs; i++) {
                int length = region->end[i] - region->beg[i];
                char l[length+1];
                strncpy(l, (const char *)(inp + region->beg[i]), length);
                l[length] = '\0';
                result=Kx("{x,,y}",result,KC(l, length));
            }
            start = inp + region->end[0];
        } else if (r == ONIG_MISMATCH) {
            break;
        } else {
            cleanup_regex(regex, region);
            return Ki(-1);
        }
    }
    cleanup_regex(regex, region);
    return result;
}

K replace(K pattern, K str, K replacement) {
    INIT_REGEX(pattern, str, regex, einfo, region, pattr, inp)
    char *result = malloc(1);
    result[0] = '\0'; int pos = 0, result_len = 1;
    char repl[NK(replacement) + 1]; CK(repl, replacement); repl[NK(replacement)] = '\0';

   int r = onig_new(&regex, pattr, pattr + strlen((char *)pattr), ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);
    if (r != ONIG_NORMAL) return Ki(-1);

    while (pos < strlen((char *)inp)) {
        r = onig_search(regex, inp, inp + strlen((char *)inp), inp + pos, inp + strlen((char *)inp), region, ONIG_OPTION_NONE);
        if (r >= 0) {
            int pre_match_len = region->beg[0] - pos;
            result = realloc(result, result_len + pre_match_len + strlen(repl) + 1);
            strncat(result, (const char *)inp + pos, pre_match_len);
            strcat(result, repl);
            pos = region->end[0]; // Move past the matched text
            result_len += pre_match_len + strlen(repl);
        } else if (r == ONIG_MISMATCH) {
            int remaining_len = strlen((char *)inp) - pos;
            result = realloc(result, result_len + remaining_len);
            strcat(result, (const char *)inp + pos);
            break;
        } else {
            free(result); cleanup_regex(regex, region);
            return Ki(-1);
        }
    }

    K ret = KC(result, strlen(result));
    free(result); cleanup_regex(regex, region);
    return ret;
}


