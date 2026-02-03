# TodoList
Simple CLI todo list in C++ with categories, priorities, and undo.

## Build
```bash
make
```

## Run
```bash
./main
```

## Commands
```
help
add <category:priority:text>
edit <id> <new_text>
ls [options]
done <id>
undone <id>
del <id>
undo
clear
q
```

### ls options
- `-s, --sort <id|done|priority>`
- `-f, --find <text>`
- `-p, --pending`
- `-d, --done`
- `-l, --low`
- `-m, --medium`
- `-h, --high`

## Data file
Tasks are stored in `todo.json` in the project root.

## Tests
```bash
make test
```

## Clean
```bash
make clean
```
