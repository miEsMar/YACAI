# 🌟 YACAI — Yet Another C Array Implementation

**A tiny, header-only dynamic array implementation for C.**

![C Header Only](https://img.shields.io/badge/header--only-✔️-blue)
![License MIT](https://img.shields.io/badge/license-MIT-green)
![Language C](https://img.shields.io/badge/language-C-orange)

YACAI is a **minimal, dependency-free, header-only** library
that brings convenient dynamic arrays to plain C.
No build system, no linker steps — just include the header and start using it.

---

## 🚀 Why YACAI?

C arrays are fast, but also:
- Fixed-size
- Unsafe if misused
- Verbose to manage dynamically

**YACAI** fills that gap with a lightweight abstraction that feels natural in C:

✔ Single header
✔ Generic via macros
✔ Dynamic resizing
✔ Simple, readable API
✔ MIT licensed

---

## 📦 Features

- 📄 **Header-only** — no compilation or linking required
- 🧩 **Generic API** — works with any type, keeps `[]` syntax.
- 📈 **Automatic resizing**
- 📏 **Size & capacity tracking**
- ⚡ **Fast & lightweight** -- support for **aligned** allocations
- 🔧 **Pure C** — no compiler extensions

---

## 📥 Installation

Just copy `yacai.h` into your project and include it:

```c
#include "yacai.h"
```

That’s it. Just make sure to `#define YACAI_IMPLEMENTATION` in **only one** `.c` file.

---

## 🧪 Example

```c
#define YACAI_IMPLEMENTATION
#include "yacai.h"
#include <stdio.h>

int main(void) {
    int *arr = YArray_new(int, 5);

    for (int i = 0; i < 5; ++i) {
        YArray_push_back(arr, i);
    }
    YArray_push_back(arr, 6);

    printf("Size: %zu\n", YArray_size(arr));

    for (size_t i = 0; i < YArray_size(arr); ++i)
        printf("%d ", arr[i]);
    printf("\n");

    YArray_free(arr);
    return 0;
}
```

**Output:**
```
Size: 6
0 1 2 3 4 6
```

---


## ⚠️ Notes

- Always call `YArray_free()` when done. YACAI does not manage memory itself.
- Indices are `size_t`.
- This library prioritizes **simplicity over heavy safety checks**.
- Ideal for small to medium C projects, tools, and embedded systems.

---

## 🧠 Design Philosophy

> “Do one thing well.”

YACAI avoids complex abstractions and stays close to idiomatic C.
It’s meant to be:
- Easy to understand
- Easy to debug
- Easy to remove if no longer needed

---

## 🤝 Contributing

Contributions are welcome!

- Bug fixes
- API improvements
- Documentation
- Examples
- Benchmarks

Feel free to open an issue or pull request.

---

## 📄 License

This project is licensed under the **MIT License**.  
You are free to use it in open-source or commercial projects.
