# library.h

## Get started

```bash
wget https://raw.githubusercontent.com/IncludeOnly/<library>.h/refs/heads/main/<library>.h
```

## Example

```c
<example code>
```

## If you really need to link

```bash
mv <library>.h <library>.c
cc -o lib<library>.so <library>.c -fPIC -D<LIBRARY>_IMPLEMENTATION -shared
mv <library>.c <library>.h
```

## License

[MIT](./LICENSE)
