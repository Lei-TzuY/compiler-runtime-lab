import sys
import os

def parse_obj_file(filepath):
    records = []
    with open(filepath, 'r') as f:
        for line in f:
            if line.strip():
                records.append(line.strip('\n'))
    return records

def pass1(obj_files, progaddr):
    estab = {}
    csaddr = progaddr
    
    for file in obj_files:
        records = parse_obj_file(file)
        
        current_cslth = 0
        for record in records:
            if record.startswith('H'):
                csect_name = record[1:7].strip()
                csect_len = int(record[13:19], 16)
                
                if csect_name in estab:
                    print(f"Error: Duplicate external symbol {csect_name}")
                else:
                    estab[csect_name] = csaddr
                
                current_cslth = csect_len
                
            elif record.startswith('D'):
                idx = 1
                while idx < len(record):
                    sym_name = record[idx:idx+6].strip()
                    sym_addr = int(record[idx+6:idx+12], 16)
                    
                    if sym_name in estab:
                        print(f"Error: Duplicate external symbol {sym_name}")
                    else:
                        estab[sym_name] = csaddr + sym_addr
                    
                    idx += 12
                    
            elif record.startswith('E'):
                csaddr += current_cslth
                
    return estab

def pass2(obj_files, progaddr, estab):
    csaddr = progaddr
    exec_addr = progaddr
    
    # 模擬 64KB 記憶體空間
    memory = bytearray(65536)
    
    for file in obj_files:
        records = parse_obj_file(file)
        current_cslth = 0
        
        for record in records:
            if record.startswith('H'):
                csect_name = record[1:7].strip()
                current_cslth = int(record[13:19], 16)
                
            elif record.startswith('T'):
                start_addr = csaddr + int(record[1:7], 16)
                length = int(record[7:9], 16)
                code_hex = record[9:]
                
                for i in range(0, len(code_hex), 2):
                    if start_addr + i//2 < len(memory):
                        byte_val = int(code_hex[i:i+2], 16)
                        memory[start_addr + i//2] = byte_val
                    
            elif record.startswith('M'):
                mod_addr = csaddr + int(record[1:7], 16)
                mod_len = int(record[7:9], 16)
                sign = record[9]
                sym_name = record[10:].strip()
                
                if sym_name not in estab:
                    print(f"Error: Undefined external symbol {sym_name}")
                    continue
                    
                sym_val = estab[sym_name]
                
                # 讀取 3 Bytes
                if mod_addr + 2 < len(memory):
                    val = (memory[mod_addr] << 16) | (memory[mod_addr+1] << 8) | memory[mod_addr+2]
                    
                    if mod_len == 5:
                        target = val & 0x0FFFFF
                        keep_mask = 0xF00000
                    elif mod_len == 6:
                        target = val & 0xFFFFFF
                        keep_mask = 0x000000
                    else:
                        target = val
                        keep_mask = 0
                        
                    if sign == '+':
                        target += sym_val
                    elif sign == '-':
                        target -= sym_val
                        
                    if mod_len == 5:
                        target &= 0x0FFFFF
                    elif mod_len == 6:
                        target &= 0xFFFFFF
                        
                    val = (val & keep_mask) | target
                    
                    memory[mod_addr] = (val >> 16) & 0xFF
                    memory[mod_addr+1] = (val >> 8) & 0xFF
                    memory[mod_addr+2] = val & 0xFF
                
            elif record.startswith('E'):
                if len(record) > 1:
                    exec_addr = csaddr + int(record[1:7], 16)
                csaddr += current_cslth
                
    return memory, exec_addr

def print_estab(estab):
    print("\n" + "="*30)
    print("External Symbol Table (ESTAB)")
    print("-" * 30)
    print(f"{'Symbol Name':<15} {'Address':<10}")
    print("-" * 30)
    for sym, addr in estab.items():
        print(f"{sym:<15} {addr:04X}")
    print("="*30 + "\n")

def dump_memory(memory, start_addr, length):
    print("="*65)
    print(f"Memory Dump ({start_addr:04X} - {start_addr+length-1:04X})")
    print("-" * 65)
    
    end_addr = start_addr + length
    dump_start = start_addr & ~0x0F
    dump_end = (end_addr + 15) & ~0x0F
    
    for addr in range(dump_start, dump_end, 16):
        hex_data = " ".join(f"{memory[addr+i]:02X}" for i in range(16))
        
        ascii_data = ""
        for i in range(16):
            b = memory[addr+i]
            if 32 <= b <= 126:
                ascii_data += chr(b)
            else:
                ascii_data += "."
                
        print(f"{addr:04X}  {hex_data}  |{ascii_data}|")
    print("="*65 + "\n")

def main():
    if len(sys.argv) < 2:
        print("Usage: python loader.py <obj_file1> [obj_file2 ...] [PROGADDR]")
        sys.exit(1)
        
    args = sys.argv[1:]
    progaddr = 0x4000
    
    obj_files = []
    for arg in args:
        if os.path.exists(arg):
            obj_files.append(arg)
        elif arg.startswith("0x") or arg.isdigit() or all(c in '0123456789ABCDEFabcdef' for c in arg):
            try:
                if arg.startswith("0x"):
                    progaddr = int(arg, 16)
                else:
                    progaddr = int(arg, 16)
            except ValueError:
                pass
                
    if not obj_files:
        print("Error: No valid object files provided.")
        sys.exit(1)
        
    print(f"Loading {len(obj_files)} object files starting at PROGADDR {progaddr:04X}...")
    
    estab = pass1(obj_files, progaddr)
    print_estab(estab)
    
    memory, exec_addr = pass2(obj_files, progaddr, estab)
    
    print(f"Load complete. Execution start address: {exec_addr:04X}\n")
    
    total_len = 0
    for file in obj_files:
        records = parse_obj_file(file)
        for record in records:
            if record.startswith('H'):
                total_len += int(record[13:19], 16)
                
    dump_memory(memory, progaddr, total_len)
    
if __name__ == "__main__":
    main()
