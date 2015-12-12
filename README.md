eYantra code repository
====

The text files are just there for reference. Hopefully, they're self-sufficient.

Every folder is a separate "project" with a different aim. If I'm not too lazy, they should have their own READMEs too. If not, the comments in the C file should indicate what we're trying to do.

Every folder *should* be structured so that it has the following files:

 - `project.h`: The header file with only the *declarations*, not *definitions*
 - `project.c`: The C source file with the definitions. This file will include `project.h`
 - `app.c`: The C source file which includes the `int main()` function. `app.c` and `project.c` will be linked together.
 - `Makefile`: Make config options. Usually, it'll be a copy of the GenericMakefile in the root directory.

###Example:
One folder is called `SevenSegment`.
Another folder called `task1` wants to use `SevenSegment` to display digits given certain logic.

The Makefile in `task1` will be modified to add `SevenSegment.c`
as a file to be linked, and `task1.c` can include `SevenSegment.h` from the other folder.

That way, running `make` in any folder will do the right thing, and programs can be executed by running
`./app` in the folder.

----

Ideally, we'll manage to separate all the functional aspects of the bot into these folders, which should make
developing an actual component that combines them much simpler.