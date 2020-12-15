#ifndef FASTSTRTOX_H
#define FASTSTRTOX_H

#ifdef __cplusplus
#error
#endif

  signed       int fast_strtoi  (char **str);
unsigned       int fast_strtoui (char **str);
  signed      long fast_strtol  (char **str);
unsigned      long fast_strtoul (char **str);
  signed long long fast_strtoll (char **str);
unsigned long long fast_strtoull(char **str);

#endif // FASTSTRTOX_H
