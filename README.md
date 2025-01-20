# string.h

## Get started

```bash
wget https://raw.githubusercontent.com/IncludeOnly/string.h/refs/heads/main/string.h
```

## Example

```c
#include <stdio.h>
#define STRING_IMPLEMENTATION
#include "string.h"

int main()
{
    string_arena_t arena;
    ArenaInit(&arena, 1024);

    string_t str = String("Hello ", &arena);
    string_t str1 = String("World", &arena);
    string_t hello = Concat(str, str1, &arena);
    string_t everyone = String(" and something else   ", &arena);
    Append(&hello, everyone, &arena);
    hello = Trim(hello, &arena);
    printf("'%s'\n", hello.chars);
    printf("%d\n", Compare(str, str));

    printf("%s\n", Substring(hello, 5, 11, &arena).chars);

    ArenaFree(&arena);
    return 0;
}
```

## If you really need to link

```bash
mv string.h string.c
cc -o libstring.so string.c -fPIC -DSTRING_IMPLEMENTATION -shared
mv string.c string.h
```

## License

[MIT](./LICENSE)
