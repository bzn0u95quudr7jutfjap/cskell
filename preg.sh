
preg() {
  GLOB="$1"
  shift
  GREP="$1"
  shift
  SBST="$1"
  if [ -n "$SBST" ] ; then
    sagasu.php -p "$GLOB" -g "$GREP" -s "$SBST"
  fi
  sagasu.php -p "$GLOB" -g "$GREP"
}

#preg 'src/*.{c,h}' 'Iter_Formatter' 'IterFormatter'
#preg 'src/*.{c,h}' 'Iter_String'    'IterString'
#preg 'src/*.{c,h}' '!._is_end'      '!ieos'
#
#preg 'src/*.{c,h}' '\.str' '.stack'
#preg 'src/*.{c,h}' '\.idx' '.i'
#preg 'src/*.{c,h}' '->str' '->stack'
#preg 'src/*.{c,h}' '->idx' '->i'
#
#preg 'src/*.{c,h}' '(\.)fmt' '\1stack' 
#preg 'src/*.{c,h}' '(->)fmt' '\1stack' 


preg 'src/{,*/}*.{c,h,txt}' '(\W?)testdata(\W?)' '\1Test\2'
preg 'src/{,*/}*.{c,h,txt}' '(\W?)stack_test(\W?)' '\1StackTest\2'

preg 'src/{,*/}*.{c,h,txt}' '(\W?)tokenizer_env(\W?)' '\1TokenEnv\2'
preg 'src/{,*/}*.{c,h,txt}' '(\W?)token_type(\W?)' '\1TokenType\2'


preg 'src/{,*/}*.{c,h,txt}' '(\W?)test_tokenizer(\W?)' '\1TestTokenizer\2'
preg 'src/{,*/}*.{c,h,txt}' '(\W?)Test_vtable(\W?)' '\1TestVTable\2'
preg 'src/{,*/}*.{c,h,txt}' '(\W?)testcode(\W?)' '\1TestCode\2'

