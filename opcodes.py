OPCODES = {
    # Format 1
    'FIX': ('C4', 1),
    'FLOAT': ('C0', 1),
    'HIO': ('F4', 1),
    'NORM': ('C8', 1),
    'SIO': ('F0', 1),
    'TIO': ('F8', 1),
    
    # Format 2
    'CLEAR': ('B4', 2),
    'COMPR': ('A0', 2),
    'TIXR': ('B8', 2),
    
    # Format 3/4
    'LDA': ('00', 3),
    'LDX': ('04', 3),
    'LDL': ('08', 3),
    'STA': ('0C', 3),
    'STX': ('10', 3),
    'STL': ('14', 3),
    'ADD': ('18', 3),
    'SUB': ('1C', 3),
    'MUL': ('20', 3),
    'DIV': ('24', 3),
    'COMP': ('28', 3),
    'J': ('3C', 3),
    'JEQ': ('30', 3),
    'JLT': ('38', 3),
    'JSUB': ('48', 3),
    'RSUB': ('4C', 3),
    'LDCH': ('50', 3),
    'STCH': ('54', 3),
    'LDB': ('68', 3),
    'STB': ('78', 3),
    'LDT': ('74', 3),
    'STT': ('84', 3),
    'TD': ('E0', 3),
    'RD': ('D8', 3),
    'WD': ('DC', 3)
}

DIRECTIVES = {'START', 'END', 'WORD', 'BYTE', 'RESW', 'RESB', 'BASE', 'NOBASE', 'MACRO', 'MEND', 'CSECT', 'EXTDEF', 'EXTREF', 'EQU'}

REGISTERS = {
    'A': 0, 'X': 1, 'L': 2, 'B': 3, 'S': 4, 'T': 5, 'F': 6, 'PC': 8, 'SW': 9
}
