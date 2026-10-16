# CMPUT 350 HW 1


## AI assistance

AI was used to help troubleshoot the Windows build and draft function
documentation. The team reviewed and adjusted the suggestions to match the
project.

### Windows build troubleshooting

Prompt: “My CMake build fails on Windows while linking SFML. The linker reports
undefined references to HarfBuzz functions such as `hb_blob_create` and
`hb_face_create`. What is causing this, and how can I fix it?”

Reflection: The response identified that HarfBuzz was not linked with FreeType,
which helped resolve the linker errors in the Windows environment.

### Function documentation

Prompt: “What style of comments is this Doxygen format, and can you help me write
similar brief `@brief`, `@param`, and `@return` comments for the functions I
provide?” The example came from the project template's `DrawContext.cpp`.

Reflection: The suggestions helped document existing functions consistently and
introduced the team to Doxygen comments. The team reviewed and edited the
generated documentation.

## C++ style

- Public API names use PascalCase; standard-library names such as `begin()` and
  `end()` retain their required spelling.
- Formatting follows the repository's `.clang-format` file, including four-space
  indentation and consistent brace placement.
