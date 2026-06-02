file(READ "${PHP_ZEND_CONFIG}" content)
string(REPLACE "# define ZEND_API"
       "# define ZEND_API __attribute__((visibility(\"default\")))"
       content "${content}")
file(WRITE "${PHP_ZEND_CONFIG}" "${content}")
