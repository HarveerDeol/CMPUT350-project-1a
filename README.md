# CMPUT 350 HW 1

## The following AI prompts were used for this project:

"My CMake build fails with this issue, how do I fix this?

[100%] Linking CXX executable Project1.exe
ld.exe: _deps/sfml-build/lib/libfreetyped.a(unity_0_c.c.obj):
in function `ft_hb_ft_reference_table':
ft-hb-ft.c:67: undefined reference to `hb_blob_create'
ft-hb-ft.c:86: undefined reference to `hb_face_create'
ft-hb-ft.c:87: undefined reference to `hb_blob_destroy'
ft-hb-ft.c:89: undefined reference to `hb_face_create_for_tables'
... (dozens more hb_\* symbols)
collect2.exe: error: ld returned 1 exit status"

Reflection: Successfully fixes the build errors in a simple way.
It explained that the font libraries were not linking properly.

---

"What style of comments is this?

/\*\*

- @brief One-line summary of what the function does.
- @param name What this parameter means.
- @return What the caller gets back.
  \*/

Can you help me replicate it for these functions?"
[functions pasted in, across multiple messages]
(comment snippet was taken from template's DrawContext.cpp)

Reflection: This saved time writing comments for functions that were already
commented for comprehension, but not formally documented. It also taught us
about Doxygen, which is the C++ documentation standard.
