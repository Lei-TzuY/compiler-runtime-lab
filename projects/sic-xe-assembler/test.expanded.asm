COPY     START   1000
FIRST    LDA     ALPHA
         ADD     BETA
         STA     GAMMA
         COMP    ZERO
         JEQ     EXIT
         J       FIRST
EXIT     RSUB
ALPHA    WORD    5
BETA     WORD    10
GAMMA    RESW    1
ZERO     WORD    0
STR      BYTE    C'EOF'
HEX      BYTE    X'F1'
         END     FIRST
