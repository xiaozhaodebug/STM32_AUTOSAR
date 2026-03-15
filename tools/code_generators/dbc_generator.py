#!/usr/bin/env python3
"""
DBC 代码生成器
从 Excel DBC 文件自动生成 STM32 CAN 代码

使用方法:
    python3 dbc_generator.py --input XZ_CAN_V2.xlsx --output ../
"""

import openpyxl
import argparse
import os
import sys
from datetime import datetime

class DbcGenerator:
    """DBC 代码生成器类"""
    
    def __init__(self):
        self.messages = []  # 消息列表
        self.signals = []   # 信号列表
        self.tx_messages = []  # 发送消息
        self.rx_messages = []  # 接收消息
        
    def parse_excel(self, excel_file):
        """解析 Excel DBC 文件"""
        print(f"[INFO] 正在解析: {excel_file}")
        
        wb = openpyxl.load_workbook(excel_file)
        ws = wb['Sheet1']
        
        current_msg = None
        
        for row in ws.iter_rows(min_row=2, values_only=True):
            # 检查是否是消息行
            if row[0] and str(row[0]).startswith('0x'):
                # 新消息
                msg_id = int(row[0], 16)
                msg_name = row[1]
                msg_type = row[2]  # CAN_Standard or CAN_Extended
                msg_len = row[3]
                send_type = row[4]
                cycle_time = row[5] if row[5] else 0
                
                current_msg = {
                    'id': msg_id,
                    'name': msg_name,
                    'type': 'Ext' if 'Extended' in str(msg_type) else 'Std',
                    'length': msg_len,
                    'send_type': send_type,
                    'cycle_time': int(cycle_time) if cycle_time else 0,
                    'sender': '',
                    'receiver': '',
                    'signals': []
                }
                self.messages.append(current_msg)
                    
            # 检查是否是信号行
            elif current_msg and row[6]:  # Signal Name
                sender = row[15] if len(row) > 15 else ''
                receiver = row[16] if len(row) > 16 else ''
                
                # 更新消息的 sender/receiver
                if sender and not current_msg['sender']:
                    current_msg['sender'] = sender
                if receiver and not current_msg['receiver']:
                    current_msg['receiver'] = receiver
                
                signal = {
                    'name': row[6],
                    'byte_order': row[7],  # intel or motorola
                    'size': int(row[8]) if row[8] else 8,
                    'start_bit': int(row[9]) if row[9] else 0,
                    'factor': float(row[10]) if row[10] else 1.0,
                    'offset': float(row[11]) if row[11] else 0.0,
                    'init_value': row[12],
                    'min': row[13],
                    'max': row[14],
                    'unit': row[19] if len(row) > 19 else '',
                    'comment': row[18] if len(row) > 18 else ''
                }
                current_msg['signals'].append(signal)
                self.signals.append({
                    'msg_name': current_msg['name'],
                    **signal
                })
        
        # 第二遍：判断 TX/RX
        for msg in self.messages:
            if 'XZ_A' in str(msg['sender']):
                self.tx_messages.append(msg)
            elif 'XZ_A' in str(msg['receiver']):
                self.rx_messages.append(msg)
                
        print(f"[INFO] 解析完成: {len(self.messages)} 个消息, {len(self.signals)} 个信号")
        print(f"[INFO] TX 消息: {len(self.tx_messages)} 个")
        print(f"[INFO] RX 消息: {len(self.rx_messages)} 个")
        
    def generate_config_h(self):
        """生成 DbcConfig.h"""
        lines = []
        
        # 文件头
        lines.append("/**")
        lines.append(" * @file        DbcConfig.h")
        lines.append(" * @brief       DBC 配置头文件 (自动生成)")
        lines.append(f" * @generated   {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        lines.append(" * ")
        lines.append(" * @note        此文件由 dbc_generator.py 自动生成，请勿手动修改")
        lines.append(" */")
        lines.append("")
        lines.append("#ifndef DBC_CONFIG_H")
        lines.append("#define DBC_CONFIG_H")
        lines.append("")
        lines.append("#include <stdint.h>")
        lines.append("")
        
        # 消息 ID 定义
        lines.append("/*===================================================================================")
        lines.append(" *                                      消息 ID 定义")
        lines.append(" *=================================================================================*/")
        lines.append("")
        
        for msg in self.messages:
            define_name = f"MSG_ID_{msg['name'].upper()}"
            lines.append(f"#define {define_name:40s} 0x{msg['id']:03X}   /* {msg['sender']} -> {msg['receiver']} */")
            
        lines.append("")
        
        # 周期定义
        lines.append("/*===================================================================================")
        lines.append(" *                                      消息周期定义 (ms)")
        lines.append(" *=================================================================================*/")
        lines.append("")
        
        for msg in self.tx_messages:
            if msg['cycle_time'] > 0:
                define_name = f"CYCLE_TIME_{msg['name'].upper()}"
                lines.append(f"#define {define_name:40s} {msg['cycle_time']}")
                
        lines.append("")
        
        # 信号定义
        lines.append("/*===================================================================================")
        lines.append(" *                                      信号定义")
        lines.append(" *=================================================================================*/")
        lines.append("")
        
        for msg in self.messages:
            if msg['signals']:
                lines.append(f"/* {msg['name']} (0x{msg['id']:03X}) 信号定义 */")
                for sig in msg['signals']:
                    sig_prefix = f"SIG_{msg['name'].upper()}_{sig['name'].upper()}"
                    lines.append(f"#define {sig_prefix}_START_BIT   {sig['start_bit']}")
                    lines.append(f"#define {sig_prefix}_SIZE        {sig['size']}")
                    lines.append(f"#define {sig_prefix}_FACTOR      {sig['factor']}")
                    lines.append(f"#define {sig_prefix}_OFFSET      {sig['offset']}")
                    if sig['unit']:
                        lines.append(f"#define {sig_prefix}_UNIT        \"{sig['unit']}\"")
                    lines.append("")
                    
        # 数据结构定义
        lines.append("/*===================================================================================")
        lines.append(" *                                      数据结构定义")
        lines.append(" *=================================================================================*/")
        lines.append("")
        
        for msg in self.messages:
            if msg['signals']:
                struct_name = f"{msg['name']}_t"
                lines.append(f"typedef struct {{")
                for sig in msg['signals']:
                    # 根据位大小选择数据类型
                    if sig['size'] <= 8:
                        c_type = "uint8_t"
                    elif sig['size'] <= 16:
                        c_type = "uint16_t"
                    else:
                        c_type = "uint32_t"
                    lines.append(f"    {c_type:10s} {sig['name']};   /* {sig['size']}bit, {sig['comment'] or 'N/A'} */")
                lines.append(f"}} {struct_name};")
                lines.append("")
                
        # 函数声明
        lines.append("/*===================================================================================")
        lines.append(" *                                      函数声明")
        lines.append(" *=================================================================================*/")
        lines.append("")
        lines.append("#ifdef __cplusplus")
        lines.append("extern \"C\" {")
        lines.append("#endif")
        lines.append("")
        
        # 接收消息解析函数
        for msg in self.rx_messages:
            func_name = f"Dbc_Parse_{msg['name']}"
            struct_name = f"{msg['name']}_t"
            lines.append(f"void {func_name}(const uint8_t* data, {struct_name}* msg);")
            
        lines.append("")
        
        # 发送消息打包函数
        for msg in self.tx_messages:
            func_name = f"Dbc_Pack_{msg['name']}"
            struct_name = f"{msg['name']}_t"
            lines.append(f"void {func_name}(uint8_t* data, const {struct_name}* msg);")
            
        lines.append("")
        lines.append("/* 信号打包/解包辅助函数 (Little Endian) */")
        lines.append("uint64_t Dbc_SignalUnpack(const uint8_t* data, uint16_t startBit, uint8_t bitSize);")
        lines.append("void Dbc_SignalPack(uint8_t* data, uint64_t value, uint16_t startBit, uint8_t bitSize);")
        lines.append("")
        lines.append("#ifdef __cplusplus")
        lines.append("}")
        lines.append("#endif")
        lines.append("")
        lines.append("#endif /* DBC_CONFIG_H */")
        lines.append("")
        
        return '\n'.join(lines)
        
    def generate_handler_c(self):
        """生成 DbcHandler.c"""
        lines = []
        
        # 文件头
        lines.append("/**")
        lines.append(" * @file        DbcHandler.c")
        lines.append(" * @brief       DBC 信号处理实现 (自动生成)")
        lines.append(f" * @generated   {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        lines.append(" * ")
        lines.append(" * @note        此文件由 dbc_generator.py 自动生成，请勿手动修改")
        lines.append(" */")
        lines.append("")
        lines.append("#include \"DbcConfig.h\"")
        lines.append("#include \"DebugLog.h\"")
        lines.append("")
        
        # 辅助函数
        lines.append("/*===================================================================================")
        lines.append(" *                                      内部函数")
        lines.append(" *=================================================================================*/")
        lines.append("")
        lines.append("uint64_t Dbc_SignalUnpack(const uint8_t* data, uint16_t startBit, uint8_t bitSize)")
        lines.append("{")
        lines.append("    uint64_t value = 0;")
        lines.append("    uint16_t byteIndex = startBit / 8;")
        lines.append("    uint8_t bitOffset = startBit % 8;")
        lines.append("    uint8_t bitsRemaining = bitSize;")
        lines.append("    uint8_t shift = 0;")
        lines.append("    ")
        lines.append("    while (bitsRemaining > 0) {")
        lines.append("        uint8_t bitsInByte = 8 - bitOffset;")
        lines.append("        if (bitsInByte > bitsRemaining) {")
        lines.append("            bitsInByte = bitsRemaining;")
        lines.append("        }")
        lines.append("        uint8_t mask = ((1U << bitsInByte) - 1) << bitOffset;")
        lines.append("        uint64_t byteValue = (data[byteIndex] & mask) >> bitOffset;")
        lines.append("        value |= (byteValue << shift);")
        lines.append("        shift += bitsInByte;")
        lines.append("        bitsRemaining -= bitsInByte;")
        lines.append("        bitOffset = 0;")
        lines.append("        byteIndex++;")
        lines.append("    }")
        lines.append("    return value;")
        lines.append("}")
        lines.append("")
        lines.append("void Dbc_SignalPack(uint8_t* data, uint64_t value, uint16_t startBit, uint8_t bitSize)")
        lines.append("{")
        lines.append("    uint16_t byteIndex = startBit / 8;")
        lines.append("    uint8_t bitOffset = startBit % 8;")
        lines.append("    uint8_t bitsRemaining = bitSize;")
        lines.append("    uint8_t shift = 0;")
        lines.append("    ")
        lines.append("    while (bitsRemaining > 0) {")
        lines.append("        uint8_t bitsInByte = 8 - bitOffset;")
        lines.append("        if (bitsInByte > bitsRemaining) {")
        lines.append("            bitsInByte = bitsRemaining;")
        lines.append("        }")
        lines.append("        uint8_t mask = (1U << bitsInByte) - 1;")
        lines.append("        uint8_t byteValue = (value >> shift) & mask;")
        lines.append("        data[byteIndex] &= ~(mask << bitOffset);")
        lines.append("        data[byteIndex] |= (byteValue << bitOffset);")
        lines.append("        shift += bitsInByte;")
        lines.append("        bitsRemaining -= bitsInByte;")
        lines.append("        bitOffset = 0;")
        lines.append("        byteIndex++;")
        lines.append("    }")
        lines.append("}")
        lines.append("")
        
        # 接收消息解析函数
        if self.rx_messages:
            lines.append("/*===================================================================================")
            lines.append(" *                                      接收消息解析")
            lines.append(" *=================================================================================*/")
            lines.append("")
            
            for msg in self.rx_messages:
                func_name = f"Dbc_Parse_{msg['name']}"
                struct_name = f"{msg['name']}_t"
                
                lines.append(f"void {func_name}(const uint8_t* data, {struct_name}* msg)")
                lines.append("{")
                lines.append("    if (data == 0 || msg == 0) return;")
                lines.append("    ")
                
                for sig in msg['signals']:
                    sig_prefix = f"SIG_{msg['name'].upper()}_{sig['name'].upper()}"
                    if sig['size'] <= 8:
                        cast = "(uint8_t)"
                    elif sig['size'] <= 16:
                        cast = "(uint16_t)"
                    else:
                        cast = "(uint32_t)"
                    lines.append(f"    msg->{sig['name']} = {cast}Dbc_SignalUnpack(data, {sig_prefix}_START_BIT, {sig_prefix}_SIZE);")
                    
                lines.append("}")
                lines.append("")
                
        # 发送消息打包函数
        if self.tx_messages:
            lines.append("/*===================================================================================")
            lines.append(" *                                      发送消息打包")
            lines.append(" *=================================================================================*/")
            lines.append("")
            
            for msg in self.tx_messages:
                func_name = f"Dbc_Pack_{msg['name']}"
                struct_name = f"{msg['name']}_t"
                
                lines.append(f"void {func_name}(uint8_t* data, const {struct_name}* msg)")
                lines.append("{")
                lines.append("    if (data == 0 || msg == 0) return;")
                lines.append("    ")
                
                # 清零
                for i in range(8):
                    lines.append(f"    data[{i}] = 0;")
                    
                for sig in msg['signals']:
                    sig_prefix = f"SIG_{msg['name'].upper()}_{sig['name'].upper()}"
                    lines.append(f"    Dbc_SignalPack(data, msg->{sig['name']}, {sig_prefix}_START_BIT, {sig_prefix}_SIZE);")
                    
                lines.append("}")
                lines.append("")
                
        return '\n'.join(lines)
        
    def generate_main_template(self):
        """生成 main.c 模板代码"""
        lines = []
        
        lines.append("/* main.c 中 DBC 相关代码模板 (参考用) */")
        lines.append("")
        lines.append("/* 在主循环中添加: */")
        lines.append("")
        
        # 发送处理
        if self.tx_messages:
            lines.append("/*==== 周期发送处理 ====*/")
            for msg in self.tx_messages:
                period = msg['cycle_time'] if msg['cycle_time'] > 0 else 100
                lines.append(f"static uint32_t last{msg['name']}Tx = 0;")
            lines.append("")
            
            for msg in self.tx_messages:
                period = msg['cycle_time'] if msg['cycle_time'] > 0 else 100
                struct_name = f"{msg['name']}_t"
                pack_func = f"Dbc_Pack_{msg['name']}"
                
                lines.append(f"/* 发送 {msg['name']} ({hex(msg['id'])}), {period}ms 周期 */")
                lines.append(f"if ((gTickCount - last{msg['name']}Tx) >= {period}) {{")
                lines.append(f"    last{msg['name']}Tx = gTickCount;")
                lines.append(f"    {struct_name} {msg['name'].lower()} = {{0}};")
                lines.append(f"    /* 填充信号值 */")
                for sig in msg['signals']:
                    lines.append(f"    /* {msg['name'].lower()}.{sig['name']} = ...; */")
                lines.append(f"    {pack_func}(canTxData, &{msg['name'].lower()});")
                lines.append(f"    /* 调用 CanDriver_Send 发送 */")
                lines.append("}")
                lines.append("")
                
        # 接收处理
        if self.rx_messages:
            lines.append("/*==== 接收处理 ====*/")
            lines.append("switch (rxMsg.Id) {")
            for msg in self.rx_messages:
                struct_name = f"{msg['name']}_t"
                parse_func = f"Dbc_Parse_{msg['name']}"
                
                lines.append(f"    case MSG_ID_{msg['name'].upper()}: {{")
                lines.append(f"        {struct_name} {msg['name'].lower()};")
                lines.append(f"        {parse_func}(rxMsg.Data, &{msg['name'].lower()});")
                lines.append(f"        /* 处理接收到的信号 */")
                for sig in msg['signals']:
                    lines.append(f"        /* {msg['name'].lower()}.{sig['name']} ... */")
                lines.append(f"        break;")
                lines.append(f"    }}")
                
            lines.append("    default:")
            lines.append("        break;")
            lines.append("}")
            
        return '\n'.join(lines)
        
    def save_files(self, output_dir):
        """保存生成的文件"""
        # 生成文件内容
        config_h = self.generate_config_h()
        handler_c = self.generate_handler_c()
        main_template = self.generate_main_template()
        
        # 保存路径
        include_dir = os.path.join(output_dir, 'include')
        src_dir = os.path.join(output_dir, 'src', 'utils')
        
        # 确保目录存在
        os.makedirs(include_dir, exist_ok=True)
        os.makedirs(src_dir, exist_ok=True)
        
        # 保存文件
        config_h_path = os.path.join(include_dir, 'DbcConfig.h')
        handler_c_path = os.path.join(src_dir, 'DbcHandler.c')
        template_path = os.path.join(output_dir, 'dbc_main_template.txt')
        
        with open(config_h_path, 'w') as f:
            f.write(config_h)
        print(f"[INFO] 已生成: {config_h_path}")
        
        with open(handler_c_path, 'w') as f:
            f.write(handler_c)
        print(f"[INFO] 已生成: {handler_c_path}")
        
        with open(template_path, 'w') as f:
            f.write(main_template)
        print(f"[INFO] 已生成: {template_path}")
        
    def print_summary(self):
        """打印汇总信息"""
        print("\n" + "="*60)
        print("DBC 代码生成完成!")
        print("="*60)
        print(f"消息总数: {len(self.messages)}")
        print(f"  - 发送消息 (TX): {len(self.tx_messages)}")
        for msg in self.tx_messages:
            period = f"{msg['cycle_time']}ms" if msg['cycle_time'] > 0 else "Event"
            print(f"      0x{msg['id']:03X} {msg['name']} ({period})")
        print(f"  - 接收消息 (RX): {len(self.rx_messages)}")
        for msg in self.rx_messages:
            print(f"      0x{msg['id']:03X} {msg['name']}")
        print(f"信号总数: {len(self.signals)}")
        print("\n使用步骤:")
        print("1. 将生成的文件复制到工程中")
        print("2. 在 main.c 中包含 DbcConfig.h")
        print("3. 参考 dbc_main_template.txt 添加发送/接收代码")
        print("4. 重新编译烧录")
        print("="*60)


def main():
    parser = argparse.ArgumentParser(description='DBC 代码生成器')
    parser.add_argument('-i', '--input', required=True, help='输入 Excel DBC 文件')
    parser.add_argument('-o', '--output', default='../', help='输出目录 (默认: ../)')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.input):
        print(f"[ERROR] 文件不存在: {args.input}")
        sys.exit(1)
        
    # 创建生成器
    generator = DbcGenerator()
    
    # 解析 Excel
    generator.parse_excel(args.input)
    
    # 生成并保存文件
    generator.save_files(args.output)
    
    # 打印汇总
    generator.print_summary()


if __name__ == '__main__':
    main()
