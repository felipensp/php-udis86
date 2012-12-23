php-udis86
==========

PHP bindings for udis86 library

For more information: http://pecl.php.net/package/udis86


```php
<?php

$rs = udis86_init();

udis86_input_file($rs, "/bin/false");
udis86_set_mode($rs, 64);
udis86_set_syntax($rs, UDIS86_ATT);

while (udis86_disassemble($rs)) {
	printf("%s\t|\t%-30s\t// len=%d - hex=%s\n", 
		udis86_insn_off($rs),
		udis86_insn_asm($rs),
		udis86_insn_len($rs),
		udis86_insn_hex($rs));
}

udis86_close($rs);
```
