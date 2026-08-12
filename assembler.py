import sys
import os
from pass1 import run_pass1
from pass2 import run_pass2
from macro import run_macro_processor

def main():
    if len(sys.argv) < 2:
        print("Usage: python assembler.py <source.asm>")
        sys.exit(1)
        
    asm_file = sys.argv[1]
    base_name = os.path.splitext(asm_file)[0]
    
    expanded_file = f"{base_name}.expanded.asm"
    int_file = f"{base_name}.int"
    sym_file = f"{base_name}.sym"
    obj_file = f"{base_name}.obj"
    lst_file = f"{base_name}.lst"
    
    print(f"Starting Macro Processor (Pass 0)...")
    run_macro_processor(asm_file, expanded_file)
    print(f"Macro Processor completed. Generated: {expanded_file}")
    
    print(f"Starting Pass 1...")
    csects, start_addr = run_pass1(expanded_file, int_file, sym_file)
    print(f"Pass 1 completed. Found {len(csects)} CSECT(s).")
    
    print(f"Starting Pass 2...")
    run_pass2(int_file, obj_file, lst_file, csects, start_addr)
    print(f"Pass 2 completed. Outputs generated: {int_file}, {sym_file}, {obj_file}, {lst_file}")

if __name__ == "__main__":
    main()
