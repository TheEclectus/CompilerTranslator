#pragma once

const char* Skip(const char* Str, const char* Delims);
const char* SkipWhitespace(const char* Str);
const char* NextWord(const char** Str, const char* Delims);
int StrToInt(const char* Str, int* Err);