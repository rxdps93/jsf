# JSF - JSON Formatter

A simple command line utility I'm writing for myself to serve a few purposes:
1. First and formost to learn more about lexers, parsers, and everything that goes into them.
2. Ensure my C skills don't get *too* rusty.
3. Provide myself with a simple command line utility I can pipe json into to validate and format it.

## Important Disclaimer
**This may not necessarily follow the JSON specification exactly** This is not intended for distribution or anything; it is a personal project. I would prefer to have it follow the specification completely but make no guarantees. I will, of course, remove this disclaimer if the time comes that this little tool is in full compliance with the JSON spec **and** has the test cases to prove it. Then and only then should anyone bother to rely upon this.

## Task Overview for Rough Completeness
- [x] Text can be read from `stdin` (e.g. piped in)
- [x] JSON is ran through a lexer and tokenized
- [ ] JSON is parsed into objects/arrays
- [ ] JSON is accurately validated
- [ ] JSON is formatted and writen to `stdout`

## Other TODO Items for Full Compliance (List is WIP)
- [ ] Negative numbers are supported in the lexer (e.g. -123)
- [ ] Decimal values are supported in the lexer (e.g. 123.5)
- [ ] Exponents are supported (e.g. 1e5, 1.25e23, 1.25e+23, incl. capital E)
- [ ] Leading 0s are accounted for
- [ ] All valid escape sequences are allowed for, both \ and unicode (u... or U...)
- [ ] Ensure valid unicode encoding (UTF-8, UTF-16, or UTF-32)
- [ ] Define utility specific max length and depth for input

For reference:
https://www.ietf.org/rfc/rfc4627.txt
