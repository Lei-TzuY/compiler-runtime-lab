import os
from pass1 import parse_line

def run_macro_processor(input_asm, output_asm):
    namtab = {}
    deftab = []
    
    with open(input_asm, 'r') as f_in, open(output_asm, 'w') as f_out:
        lines = f_in.readlines()
        
        idx = 0
        
        while idx < len(lines):
            line = lines[idx]
            label, opcode, operand, is_comment = parse_line(line)
            
            if is_comment or not opcode:
                f_out.write(line)
                idx += 1
                continue
                
            if opcode == 'MACRO':
                macro_name = label
                macro_params = []
                if operand:
                    macro_params = [p.strip() for p in operand.split(',')]
                
                start_idx = len(deftab)
                deftab.append(line.strip('\n'))
                
                idx += 1
                level = 1
                while idx < len(lines) and level > 0:
                    def_line = lines[idx]
                    def_label, def_opcode, def_operand, def_is_comment = parse_line(def_line)
                    
                    if def_opcode == 'MACRO':
                        level += 1
                    elif def_opcode == 'MEND':
                        level -= 1
                        
                    if not def_is_comment and level >= 0:
                        processed_line = def_line.strip('\n')
                        for p_idx, p_name in enumerate(macro_params):
                            processed_line = processed_line.replace(p_name, f"?{p_idx+1}")
                        deftab.append(processed_line)
                    else:
                        deftab.append(def_line.strip('\n'))
                    
                    idx += 1
                    
                end_idx = len(deftab) - 1
                namtab[macro_name] = (start_idx, end_idx)
                continue
                
            if opcode in namtab:
                macro_name = opcode
                actual_args = []
                if operand:
                    actual_args = [p.strip() for p in operand.split(',')]
                    
                start_idx, end_idx = namtab[macro_name]
                
                f_out.write(f". Macro Expansion: {macro_name}\n")
                if label:
                    f_out.write(f"{label}\n")
                
                for i in range(start_idx + 1, end_idx):
                    exp_line = deftab[i]
                    for a_idx, a_val in enumerate(actual_args):
                        exp_line = exp_line.replace(f"?{a_idx+1}", a_val)
                    
                    # 處理 Label 替換為空，避免 Label 重複 (簡化做法)
                    # 只對有 Label 的行處理，或是如果需要生成唯一標籤則應加入處理機制。
                    # 這裡直接展開
                    f_out.write(exp_line + "\n")
                    
                idx += 1
                continue
                
            f_out.write(line)
            idx += 1
