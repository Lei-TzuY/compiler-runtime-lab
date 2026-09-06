COPY     START   0
FIRST    STL     RETADR
         LDB     #LENGTH
         BASE    LENGTH
. Macro Expansion: RDBUFF
CLOOP
         CLEAR   X
         CLEAR   A
         CLEAR   S
         +LDT    #4096
         +TD     INPUT
         JEQ     *-3
         +RD     INPUT
         COMPR   A,S
         JEQ     *+11
         STCH    BUFFER,X
         TIXR    T
         JLT     *-19
         STX     LENGTH
         LDA     LENGTH
         COMP    #0
         JEQ     ENDFIL
         +JSUB   WRREC
         J       CLOOP
ENDFIL   LDA     EOF
         STA     BUFFER
         LDA     #3
         STA     LENGTH
         +JSUB   WRREC
         J       @RETADR
EOF      BYTE    C'EOF'
RETADR   RESW    1
LENGTH   RESW    1
BUFFER   RESB    4096
WRREC    CLEAR   X
         LDT     LENGTH
WLOOP    TD      OUTPUT
         JEQ     WLOOP
         LDCH    BUFFER,X
         WD      OUTPUT
         TIXR    T
         JLT     WLOOP
         RSUB
INPUT    BYTE    X'F1'
OUTPUT   BYTE    X'05'
         END     FIRST
