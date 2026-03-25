import subprocess
import time
import sys
import io
import os
import tempfile
import pandas as pd

# 设置编码
if sys.platform == 'win32':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

# 配置路径
EXE_PATH = r"localllm.exe"
MODEL_PATH = r"models\qwen2.5-1.5b-instruct-q4_0.gguf"

def parse_model_output(stdout_bytes):
    """从模型输出的 bytes 中解析结果"""
    if not stdout_bytes:
        print("[DEBUG] stdout is empty (模型没有任何输出)")
        return -1
        
    # 解码
    output = stdout_bytes.decode('utf-8', errors='ignore').strip()
    
    # === 调试打印 ===
    # 使用 repr 打印，这样能看到隐藏字符（如 \r, \n, 颜色代码等）
    print(f"\n[DEBUG] Raw output repr: {repr(output)}")
    # ===============
    
    # 拆行，从后往前找
    lines = [l.strip() for l in output.splitlines() if l.strip()]
    if not lines:
        return -1

    for l in reversed(lines):
        # 跳过统计信息 [ ... ]
        if l.startswith("[") and l.endswith("]"):
            continue
        # 跳过 REPL 提示符 ">" 以及 Prompt 回显 "> 这句话..."
        if l.startswith(">"):
            continue
        # 跳过可能的退出命令回显
        if "/exit" in l:
            continue
        
        # 找到有效行，开始匹配关键词
        # 优先判断 "不符合"，因为它包含了 "符合"
        if "不符合" in l:
            print(f"[DEBUG] Found negative answer in line: {repr(l)}")
            return 0
        
        # 你的输出里有 "|符合。"，可能前面跟了管道符或光标符
        if "符合" in l:
            # 如果包含 "符合" 但不包含 "不符合"
            print(f"[DEBUG] Found positive answer in line: {repr(l)}")
            return 1

    # 如果找完所有行都没匹配到
    return -1

def check_appointment(text):
    """判断任命证书是否合理，返回 1(合理) 或 0(不合理) 或 -1(无法判断)"""

    # 构造 prompt
    prompt_text = f"这句话是否符合逻辑：{text}。输出符合或者不符合"

    # 使用临时文件传递 prompt，解决 Windows 命令行参数传递中文时的编码(乱码)问题
    fd, prompt_file = tempfile.mkstemp(suffix=".txt", text=True)
    with os.fdopen(fd, 'w', encoding='utf-8') as f:
        f.write(prompt_text)

    # === 编码验证模块 (仅在第一次调用时运行) ===
    if not hasattr(check_appointment, "has_verified_encoding"):
        check_appointment.has_verified_encoding = True
        print(f"\n[编码自检] Prompt临时文件已生成: {prompt_file}")
        print(f"[编码自检] 文件内容写入编码: UTF-8")
        print(f"[编码自检] 请稍后检查该文件内容是否为正常中文。\n")
        # 标记该文件不被删除，供用户检查
        keep_file = True
    else:
        keep_file = False
    # ==========================================

    cmd = [
        EXE_PATH,
        "-m", MODEL_PATH,
        "-f", prompt_file,
        "-n", "16",
        "--temp", "0.0",
        # "--no-interactive"  # 删除不支持的参数
    ]

    # 将 stderr 直接输出到屏幕，这样用户能看到模型加载进度，就不会以为死机而手动按 Ctrl+C 了
    # stdout 依然捕获用于解析结果
    
    print(f" (正在加载模型，请耐心等待...)", end="", flush=True)
    proc = None
    try:
        proc = subprocess.Popen(
            cmd,
            stdin=None,  # 不使用 stdin pipe，防止管道阻塞
            stdout=subprocess.PIPE,
            stderr=None,  # 不捕获 stderr，直接显示在控制台，用户可以看到 model load log
        )
        
        # 等待进程结束，最长 20 秒
        try:
            # 不再发送 input，避免 stdin 管道问题导致启动卡死
            # 如果程序执行完需要按键，会等到超时，然后我们 kill 它并读取输出
            stdout_data, _ = proc.communicate(timeout=20)
            return parse_model_output(stdout_data)
            
        except subprocess.TimeoutExpired:
            # print(" 推理超时/等待输入，尝试获取结果...") # 调试用
            proc.kill()
            # 尝试读取已有的输出
            try:
                stdout_data, _ = proc.communicate(timeout=5)
                return parse_model_output(stdout_data)
            except:
                return -1

    except KeyboardInterrupt:
        print("\n 用户中断(Ctrl+C)！")
        if proc:
            proc.kill()
        # 既然是用户中断，可能不想跑了，这里直接抛出还是返回-1？
        # 为了不破坏整体流程，我们尝试解析一下如果不成则返回-1
        return -1

    except Exception as e:
        print(f" 错误: {e}")
        if proc:
            proc.kill()
        return -1
        
    finally:
        # 务必清理临时文件 (除非是保留用于检查的那一个)
        if os.path.exists(prompt_file) and not keep_file:
            try:
                os.remove(prompt_file)
            except:
                pass

def main():
    print("=" * 70)
    print("准确度测试 - 任命证书逻辑判断")
    print("=" * 70)
    
    # 读取 Excel 文件
    excel_file = "text.xlsx"
    
    if not os.path.exists(excel_file):
        print(f"错误：找不到文件 {excel_file}")
        print("请确保 text.xlsx 在当前目录下")
        input("按回车键退出...")
        return
    
    try:
        df = pd.read_excel(excel_file)
        print(f"成功读取 {len(df)} 条数据")
        print(f"列名: {list(df.columns)}")
    except Exception as e:
        print(f"读取 Excel 失败: {e}")
        input("按回车键退出...")
        return
    
    # 检查列名
    if 'id' not in df.columns:
        print("警告：没有找到 'id' 列，将使用行号作为 ID")
        df['id'] = range(1, len(df) + 1)
    
    if '文本' not in df.columns and 'text' not in df.columns:
        print("错误：需要 '文本' 或 'text' 列")
        input("按回车键退出...")
        return
    
    text_col = '文本' if '文本' in df.columns else 'text'
    label_col = '标签' if '标签' in df.columns else 'label'
    
    if label_col not in df.columns:
        print(f"错误：需要 '{label_col}' 列")
        input("按回车键退出...")
        return
    
    print(f"使用列: ID={df.columns[0]}, 文本={text_col}, 标签={label_col}")
    print("-" * 70)
    
    # 测试所有样本
    results = []
    total = len(df)
    
    print("\n开始测试...")
    print("-" * 70)
    
    for idx, row in df.iterrows():
        id_val = row['id'] if 'id' in df.columns else idx + 1
        text = row[text_col]
        true_label = row[label_col]
        
        print(f"[{idx+1}/{total}] ID={id_val} 测试中...", end=" ", flush=True)
        
        pred_label = check_appointment(text)
        
        if pred_label == 1:
            pred_str = "合理"
        elif pred_label == 0:
            pred_str = "不合理"
        else:
            pred_str = "无法判断"
        
        true_str = "合理" if true_label == 1 else "不合理"
        
        if pred_label == true_label:
            status = "✓"
        elif pred_label == -1:
            status = "?"
        else:
            status = "✗"
        
        print(f"{status} 预测:{pred_str} 真实:{true_str}")
        
        results.append({
            'id': id_val,
            '文本': text,
            '真实标签': true_label,
            '预测标签': pred_label,
            '预测结果': pred_str,
            '是否正确': status
        })
    
    # 计算指标
    print("\n" + "=" * 70)
    print("测试结果统计")
    print("=" * 70)
    
    valid_results = [r for r in results if r['预测标签'] != -1]
    invalid_count = len(results) - len(valid_results)
    
    if valid_results:
        y_true = [r['真实标签'] for r in valid_results]
        y_pred = [r['预测标签'] for r in valid_results]
        
        # 混淆矩阵
        tp = sum(1 for t, p in zip(y_true, y_pred) if t == 1 and p == 1)
        tn = sum(1 for t, p in zip(y_true, y_pred) if t == 0 and p == 0)
        fp = sum(1 for t, p in zip(y_true, y_pred) if t == 0 and p == 1)
        fn = sum(1 for t, p in zip(y_true, y_pred) if t == 1 and p == 0)
        
        # 计算指标
        accuracy = (tp + tn) / len(valid_results)
        precision = tp / (tp + fp) if (tp + fp) > 0 else 0
        recall = tp / (tp + fn) if (tp + fn) > 0 else 0
        f1 = 2 * (precision * recall) / (precision + recall) if (precision + recall) > 0 else 0
        
        print(f"\n总样本数: {len(results)}")
        print(f"有效样本数: {len(valid_results)}")
        print(f"无法判断样本数: {invalid_count}")
        
        print(f"\n混淆矩阵:")
        print(f"              预测合理    预测不合理")
        print(f"实际合理        {tp:>6}        {fn:>6}")
        print(f"实际不合理      {fp:>6}        {tn:>6}")
        
        print(f"\n评估指标:")
        print(f"准确率 (Accuracy):  {accuracy:.4f} ({accuracy*100:.2f}%)")
        print(f"精确率 (Precision): {precision:.4f} ({precision*100:.2f}%)")
        print(f"召回率 (Recall):    {recall:.4f} ({recall*100:.2f}%)")
        print(f"F1分数:             {f1:.4f}")
    else:
        print(f"\n所有样本都无法判断！")
        print(f"总样本数: {len(results)}")
        print(f"无法判断样本数: {invalid_count}")
    
    # 保存详细结果
    results_df = pd.DataFrame(results)
    results_df.to_excel('test_results.xlsx', index=False)  # 去掉 encoding

    # 保存错误案例
    errors = [r for r in results if r['真实标签'] != r['预测标签'] and r['预测标签'] != -1]
    if errors:
        errors_df = pd.DataFrame(errors)
        errors_df.to_excel('error_cases.xlsx', index=False)  # 同样去掉 encoding
    
    print(f"\n详细结果已保存到 test_results.xlsx")
    
    if errors:
        print(f"错误案例已保存到 error_cases.xlsx ({len(errors)} 条)")
    
    print("\n" + "=" * 70)
    print("测试完成！")
    print("=" * 70)

if __name__ == "__main__":
    main()
    input("\n按回车键退出...")