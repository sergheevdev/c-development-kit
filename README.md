# :dizzy: CDK (C Development Kit)

[![MIT License](https://img.shields.io/badge/License-Apache%202.0-brightgreen.svg)](https://github.com/sergheevdev/c-development-kit/blob/main/LICENSE)

## Introduction

Whenever programming in C, if you want to use a library, most of them are really hard to 
understand, have difficult inner workings and unexpected behavior. One reason for this, is that
current standards do not focus on code being readable for legacy reasons, but times change
and so must C.

**C Development Kit**: is a development kit that contains pre-defined data structures and
algorithms ready to use. The focus is on providing readable, secure and fast pre-defined
data structures and algorithms.

## Prelude

If you are willing to contribute to this repository:
- No old standards (i.e. ANSI C) will be acceptable, we won't support a deprecated platform in
  any case, if you want to use C89 or any old standard feel free to fork the project and
  adapt it to your needs (most hardware supports +C99, so there shouldn't be any problem).
- The focus is more on readability and security than "micro optimizations". If you are willing
  to "pull request" for any "micro optimization" that harms in any way legibility or security, it
  won't be accepted.
- We follow no particular standard, but consistency is king (check the already implemented 
  structures and try to be as consistent as possible). Why? Because the idea is to discover
  a new standard that in the long run will favor the creation of high quality software.
- All algorithms and structures domain validation must be included and none of it skipped for
  "micro optimization" reasons, because we want secure software.
- Unit testing is mandatory (before writing any production code), the process of writing unit
  tests and then improving code quality for a better design creates a really positive feedback
  loop. We will use a mix of "purist TDD" (progress guided by failing unit tests) and "pragmatic
  TDD" (write tests first), it is not mandatory to write tests first as long as they are present,
  you may adapt any of those two ideas depending on the development context and your personality.
- Any or this following ideas is arguable and might change over time as the software evolves.

The final idea will be having a highly maintainable development kit for the C language that could
be flexible enough to adapt to most situations, and by doing so provide a base universal library,
that follows a fictional standard that evolves according to developer's needs. 

## Status

This kit is in **pre-alpha** version, and there is currently no stable version nor documentation, 
use at your own risk (you may find incomplete implementations).

## License

[Apache 2.0](LICENSE) &copy; Serghei Sergheev
