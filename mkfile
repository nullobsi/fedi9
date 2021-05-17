</$objtype/mkfile

timeline: timeline.$O
	$LD $LDFLAGS -o $target $prereq

%.$O: %.c
	$CC $CFLAGS $stem.c

