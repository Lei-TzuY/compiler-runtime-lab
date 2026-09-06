COPY     START   0
RDBUFF   MACRO   &INDEV,&BUFADR,&RECLTH
         CLEAR   X
         CLEAR   A
         CLEAR   S
         +LDT    #4096
         +TD     &INDEV
         JEQ     *-3
         +RD     &INDEV
         COMPR   A,S
         JEQ     *+11
         STCH    &BUFADR,X
         TIXR    T
         JLT     *-19
         STX     &RECLTH
         MEND
FIRST    STL     RETADR
         LDB     #LENGTH
         BASE    LENGTH
CLOOP    RDBUFF  INPUT,BUFFER,LENGTH
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
