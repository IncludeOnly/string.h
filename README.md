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
    ArenaInit(1024);

    string_t str = String("Hello ");
    string_t str1 = String("World");
    string_t hello = Concat(str, str1);
    hello = Append(hello, String(" and something else   "));
    hello = Trim(hello);

    printf("'%s'\n", hello.chars);
    printf("%d\n", Compare(str, str1));
    printf("%s\n", Substring(hello, 5, 11).chars);
    printf("'%c'\n", CharAt(hello, 6));

    string_array_t words = Split(hello, ' ');
    for(size_t i = 0; i < words.count; i++){
        printf("%s\n", words.strings[i].chars);
    }

    ArenaFree();

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
