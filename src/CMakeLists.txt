find_package(BISON)
BISON_TARGET(Parser ${CMAKE_SOURCE_DIR}/src/tiger.y ${CMAKE_SOURCE_DIR}/src/tiger.tab.c
        DEFINES_FILE ${CMAKE_SOURCE_DIR}/src/tiger.tab.h COMPILE_FLAGS -d)

find_package(FLEX)
FLEX_TARGET(Lexer ${CMAKE_SOURCE_DIR}/src/tiger.lex ${CMAKE_SOURCE_DIR}/src/lex.yy.c)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

file(GLOB SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c")

list(APPEND SRC_FILES
        ${BISON_Parser_OUTPUT_SOURCE}
        ${FLEX_Lexer_OUTPUTS}
)