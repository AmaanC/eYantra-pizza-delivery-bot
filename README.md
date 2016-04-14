eYantra code repository
====

The text files in this folder (the root) are only here for reference, since most modules will use them as a template.

Every folder is a separate "module" with a different aim. If I'm not too lazy, they should have their own READMEs too. If not, the comments in the C file should indicate what we're trying to do.

Every folder *should* be structured so that it has the following files:

 - `project.h`: The header file with only the *declarations*, not *definitions*
 - `project.c`: The C source file with the definitions. This file will include `project.h`
 - `app.c`: The C source file which includes an example implementation of the `project` module. `app.c` and `project.c` will be linked together to test just that module's working.
 - `Makefile`: Make config options (what gets linked to what and how). Usually, it'll be a copy of the Makefile in the root directory.

### Example

One folder is called `SevenSegment`.
Another folder called `task1` wants to use `SevenSegment` to display digits given certain logic.

The Makefile in `task1` will be modified to add `SevenSegment.c` as a file to be linked, and `task1.c`
can include `SevenSegment.h` from the other folder.

That way, running `make` in any folder will do the right thing, and programs can be executed by running
`./app` in the folder.

----

Ideally, we'll manage to separate all the functional aspects of the bot into these folders, which should make
developing an actual component that combines them much simpler.

### Demo

[YouTube video of the bot following the map](https://www.youtube.com/watch?v=6Is9ASjp1w4)

We also had both the arms working and the timeline solver worked satisfactorily, but there aren't any
video demos of that.

### Bloopers

- [Here's a video of the first time we tried line following](https://goo.gl/photos/cuF9ZraoJfjcR1ym8)

- I had a very peculiar bug this one time where if I `free`d a certain variable, the program would crash. If I just skipped `free`ing it, it would work fine. The reason this was happening was because the code looked roughly like this:

```
    SomeArray *array_of_things = malloc(sizeof(*array_of_things));
    array_of_things->arr = malloc(sizeof(*(array_of_things->arr))); // Here lies the fatal flaw!

    YouShantBeFreed xyz = malloc(sizeof(*xyz));

    // Some code that used array_of_things->arr and xyz

    free(xyz); // This would cause the program to crash
```

The problem was that what I used as an array was actually just `malloc`d space enough for *1* element. So I was overwriting memory from `xyz` and whatever came after. `free`ing `xyz` was problematic because I still needed the values from that `arr`.

Don't work too hard on things just because of deadlines, kids.

![Do you want buffer overflows?](https://i.imgur.com/D99AKyQ.jpg)