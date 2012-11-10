php-udis86
==========

PHP bindings for udis86 library


```php
<?php

$rs = udis86_init();

udis86_input_file($rs, "/bin/false");

while (udis86_disassemble($rs)) {
	printf("%s // %d - %s\n", 
		udis86_insn_asm($rs),
		udis86_insn_len($rs),
		udis86_insn_hex($rs));
}
```
