from opcodes import OPCODES, REGISTERS
from pass1 import parse_line

def run_pass2(int_file, obj_file, lst_file, csects, start_addr):
    base_register = -1
    
    with open(int_file, 'r') as f_in, open(lst_file, 'w') as f_lst, open(obj_file, 'w') as f_obj:
        
        current_csect = ""
        text_record = ""
        text_start = -1
        mod_records = []
        
        def flush_text_record():
            nonlocal text_record, text_start
            if text_record:
                length = len(text_record) // 2
                f_obj.write(f"T{text_start:06X}{length:02X}{text_record}\n")
                text_record = ""
                text_start = -1
                
        def start_csect(name, addr):
            nonlocal current_csect, mod_records
            current_csect = name
            csect_len = csects[name]['length']
            
            # H record
            name_str = (name + "      ")[:6]
            f_obj.write(f"H{name_str}{addr:06X}{csect_len:06X}\n")
            
            # D record
            extdefs = csects[name]['extdef']
            if extdefs:
                d_rec = "D"
                for d in extdefs:
                    if d in csects[name]['symtab']:
                        d_addr = csects[name]['symtab'][d]
                        d_rec += f"{d.ljust(6)[:6]}{d_addr:06X}"
                f_obj.write(d_rec + "\n")
                
            # R record
            extrefs = csects[name]['extref']
            if extrefs:
                r_rec = "R"
                for r in extrefs:
                    r_rec += f"{r.ljust(6)[:6]}"
                f_obj.write(r_rec + "\n")
                
            mod_records = []
            
        def end_csect(write_e=False, exec_addr=None):
            flush_text_record()
            for m in mod_records:
                f_obj.write(f"{m}\n")
            if write_e:
                if exec_addr is not None:
                    f_obj.write(f"E{exec_addr:06X}\n")
                else:
                    f_obj.write(f"E\n")
            else:
                f_obj.write(f"E\n")

        lines = f_in.readlines()
        
        for idx, line in enumerate(lines):
            parts = line.strip('\n').split('\t', 1)
            if len(parts) != 2:
                if "END" in line:
                    _, _, operand, _ = parse_line(line)
                    f_lst.write(f"\t\t\t{line.strip()}\n")
                    first_exec_addr = start_addr
                    if operand and operand in csects[current_csect]['symtab']:
                        first_exec_addr = csects[current_csect]['symtab'][operand]
                    end_csect(write_e=True, exec_addr=first_exec_addr)
                    break
                continue
                
            addr_str = parts[0].strip()
            orig_line = parts[1].strip()
            current_pc = int(addr_str, 16) if addr_str else 0
            
            next_pc = current_pc
            if idx + 1 < len(lines):
                next_parts = lines[idx+1].strip('\n').split('\t', 1)
                if len(next_parts) == 2 and next_parts[0].strip():
                    next_pc = int(next_parts[0].strip(), 16)
            
            label, opcode, operand, is_comment = parse_line(orig_line)
            
            if is_comment:
                continue
                
            obj_code = ""
            
            if opcode == 'START':
                start_csect(label if label else "DEFAULT", current_pc)
                f_lst.write(f"{addr_str}\t\t{orig_line}\n")
                continue
                
            if opcode == 'CSECT':
                end_csect() # End previous csect
                start_csect(label if label else "UNNAMED", 0)
                base_register = -1
                f_lst.write(f"{addr_str}\t\t{orig_line}\n")
                continue
            
            if opcode == 'END':
                f_lst.write(f"\t\t\t{orig_line}\n")
                first_exec_addr = start_addr
                if operand and operand in csects[current_csect]['symtab']:
                    first_exec_addr = csects[current_csect]['symtab'][operand]
                end_csect(write_e=True, exec_addr=first_exec_addr)
                break
                
            if opcode in ['EXTDEF', 'EXTREF']:
                f_lst.write(f"\t\t\t{orig_line}\n")
                continue
                
            if opcode == 'BASE':
                if operand in csects[current_csect]['symtab']:
                    base_register = csects[current_csect]['symtab'][operand]
                f_lst.write(f"\t\t\t{orig_line}\n")
                continue
            
            if opcode == 'NOBASE':
                base_register = -1
                f_lst.write(f"\t\t\t{orig_line}\n")
                continue
                
            raw_opcode = opcode[1:] if opcode and opcode.startswith('+') else opcode
            
            if raw_opcode in OPCODES:
                op_val = int(OPCODES[raw_opcode][0], 16)
                fmt = OPCODES[raw_opcode][1]
                is_format4 = opcode.startswith('+')
                
                if fmt == 1:
                    obj_code = f"{op_val:02X}"
                elif fmt == 2:
                    r1 = 0
                    r2 = 0
                    if operand:
                        regs = operand.split(',')
                        r1 = REGISTERS.get(regs[0].strip(), 0)
                        if len(regs) > 1:
                            r2 = REGISTERS.get(regs[1].strip(), 0)
                    obj_code = f"{op_val:02X}{(r1 << 4) | r2:02X}"
                elif fmt == 3:
                    n, i, x, b, p, e = 1, 1, 0, 0, 0, 0
                    disp = 0
                    
                    if is_format4:
                        e = 1
                    
                    if not operand:
                        pass
                    else:
                        op_symbol = operand
                        if operand.startswith('#'):
                            n, i = 0, 1
                            op_symbol = operand[1:]
                        elif operand.startswith('@'):
                            n, i = 1, 0
                            op_symbol = operand[1:]
                            
                        if op_symbol.endswith(',X'):
                            x = 1
                            op_symbol = op_symbol[:-2]
                            
                        target_addr = 0
                        is_absolute = False
                        is_extref = False
                        
                        if op_symbol.startswith('*'):
                            offset = 0
                            if len(op_symbol) > 1:
                                offset = int(op_symbol[1:])
                            target_addr = next_pc + offset
                        elif op_symbol.isdigit() or (op_symbol.startswith('-') and op_symbol[1:].isdigit()):
                            target_addr = int(op_symbol)
                            is_absolute = True
                        elif op_symbol in csects[current_csect]['symtab']:
                            target_addr = csects[current_csect]['symtab'][op_symbol]
                        elif op_symbol in csects[current_csect]['extref']:
                            is_extref = True
                            target_addr = 0
                        else:
                            print(f"Error: Undefined symbol {op_symbol} in CSECT {current_csect}")
                            
                        if is_absolute:
                            disp = target_addr
                        elif is_extref:
                            if e == 1:
                                disp = 0
                                mod_records.append(f"M{current_pc+1:06X}05+{op_symbol.ljust(6)[:6]}")
                            else:
                                print(f"Error: External reference {op_symbol} must be used with Format 4")
                        else:
                            if e == 1:
                                disp = target_addr
                                mod_records.append(f"M{current_pc+1:06X}05+{current_csect.ljust(6)[:6]}")
                            else:
                                disp_val = target_addr - next_pc
                                if -2048 <= disp_val <= 2047:
                                    p = 1
                                    disp = disp_val & 0xFFF
                                elif base_register != -1:
                                    disp_val = target_addr - base_register
                                    if 0 <= disp_val <= 4095:
                                        b = 1
                                        disp = disp_val & 0xFFF
                                    else:
                                        print(f"Error: Displacement out of bounds for {opcode} {operand}")
                                else:
                                    print(f"Error: Displacement out of bounds and NOBASE for {opcode} {operand}")
                    
                    byte1 = (op_val & 0xFC) | (n << 1) | i
                    byte2 = (x << 7) | (b << 6) | (p << 5) | (e << 4)
                    
                    if e == 1:
                        byte2 |= (disp >> 16) & 0x0F
                        obj_code = f"{byte1:02X}{byte2:02X}{disp & 0xFFFF:04X}"
                    else:
                        byte2 |= (disp >> 8) & 0x0F
                        obj_code = f"{byte1:02X}{byte2:02X}{disp & 0xFF:02X}"
                        
            elif opcode == 'WORD':
                val = 0
                if operand in csects[current_csect]['extref']:
                    # Handle WORD with EXTREF
                    mod_records.append(f"M{current_pc:06X}06+{operand.ljust(6)[:6]}")
                else:
                    try:
                        val = int(operand)
                    except ValueError:
                        print(f"Error: Unsupported WORD operand {operand}")
                if val < 0:
                    val = (1 << 24) + val
                obj_code = f"{val:06X}"
                
            elif opcode == 'BYTE':
                if operand.startswith("C'") and operand.endswith("'"):
                    chars = operand[2:-1]
                    obj_code = "".join(f"{ord(c):02X}" for c in chars)
                elif operand.startswith("X'") and operand.endswith("'"):
                    obj_code = operand[2:-1]
                    
            elif opcode in ['RESW', 'RESB']:
                pass
                
            if obj_code:
                f_lst.write(f"{addr_str}\t{obj_code.ljust(10)}\t{orig_line}\n")
                
                if text_start == -1:
                    text_start = int(addr_str, 16)
                    
                if len(text_record) + len(obj_code) > 60:
                    flush_text_record()
                    text_start = int(addr_str, 16)
                    
                text_record += obj_code
                
            else:
                f_lst.write(f"{addr_str}\t\t\t{orig_line}\n")
                flush_text_record()
