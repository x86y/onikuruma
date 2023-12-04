#include "../../a.h"
#include "../../k.h"
#include "/opt/homebrew/include/oniguruma.h"

A regx(A pattern, A str) {
  OnigRegex regex;
  OnigErrorInfo einfo;
  OnigRegion *region;
  char patt[NK(pattern) + 1]; CK(patt, pattern); patt[NK(pattern)] = '\0';
  char inpp[NK(str) + 1]; CK(inpp, str); inpp[NK(str)] = '\0';
  UChar *pattr = (UChar *)patt;
  UChar *inp = (UChar *)inpp;

  int r;
  r = onig_new(&regex, pattr, pattr + strlen((char *)pattr), ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) return -1;

  region = onig_region_new();
  r = onig_search(regex, inp, inp + strlen((char *)inp), inp, inp + strlen((char *)inp), region, ONIG_OPTION_NONE);

  K result;
  if (r >= 0) result = Ki(region->beg[0]); // position of the match
  else result = -1;

  onig_region_free(region, 1); onig_free(regex);
  return result;
}
