# filepath: d:\AAAstudy materials\jinshan\CODE\llama-cpu\accuracy_test_fast.py
import subprocess
import time
import sys
import io
import os
import pandas as pd
import json
import urllib.request
import atexit

# 尝试导入 sklearn，如果没有则使用手动计算
try:
    from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score, classification_report
    HAS_SKLEARN = True
except ImportError:
    HAS_SKLEARN = False

# 设置编码
if sys.platform == 'win32':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

# 配置路径 (根据你的环境)
SERVER_EXE = r"llama-server.exe" 
MODEL_PATH = r"models\qwen2.5-1.5b-instruct-q4_0.gguf"
PORT = 8080
HOST = "127.0.0.1"
API_URL = f"http://{HOST}:{PORT}/completion"

# 全局保存服务进程，方便清理
server_process = None

def cleanup_server():
    """清理服务进程"""
    global server_process
    if server_process:
        print("\n正在关闭 LLM 服务...")
        try:
            server_process.kill()
        except:
            pass
        server_process = None

atexit.register(cleanup_server)

def start_server():
    """启动 llama-server"""
    global server_process
    print("=" * 70)
    print(f"正在启动 LLM 服务 (端口 {PORT})...")
    print("只需加载一次模型，后续推理将非常快！")
    print("=" * 70)
    
    # 检查模型是否存在
    if not os.path.exists(MODEL_PATH):
        print(f"错误: 找不到模型文件 {MODEL_PATH}")
        return False

    cmd = [
        SERVER_EXE,
        "-m", MODEL_PATH,
        "--port", str(PORT),
        "-c", "2048",      # 上下文长度
        "--temp", "0.0",   # 默认温度
        "--n-gpu-layers", "0" # 纯CPU
    ]

    # 启动进程
    try:
        server_process = subprocess.Popen(
            cmd,
            stdout=subprocess.DEVNULL,   # 屏蔽标准输出
            stderr=None                  # 显示错误输出（包含加载进度）
        )
    except FileNotFoundError:
        print(f"错误: 找不到 {SERVER_EXE}，请确保它在当前目录")
        return False

    print("等待服务就绪 (预计 10-30 秒)...")
    
    # 轮询端口直到服务可用
    start_time = time.time()
    while time.time() - start_time < 120: # 最多等2分钟
        if server_process.poll() is not None:
            print("错误：服务进程意外退出！")
            return False
        
        try:
            # 尝试访问健康检查接口
            with urllib.request.urlopen(f"http://{HOST}:{PORT}/health", timeout=1) as response:
                if response.status == 200:
                    print("\n>>> 服务已就绪！开始极速测试！ <<<\n")
                    return True
        except:
            # 如果连接被拒绝，说明还没启动好，等待一下
            time.sleep(1)
            pass
            
    print("错误：服务启动超时！")
    return False

def call_llm(prompt):
    """调用 HTTP API 获取结果"""
    # 使用 ChatML 模板格式，强迫模型进入对话模式
    # 这样模型就会认为自己是在回答问题，而不是在做文本续写
    formatted_prompt = f"<|im_start|>user\n{prompt}<|im_end|>\n<|im_start|>assistant\n"
    
    data = {
        "prompt": formatted_prompt,
        "n_predict": 32,    # 给足够的长度
        "temperature": 0.0, # 0度 最稳定
        "stop": ["<|im_end|>"] # 以这个作为结束符
    }
    
    req = urllib.request.Request(
        API_URL,
        data=json.dumps(data).encode('utf-8'),
        headers={'Content-Type': 'application/json'}
    )
    
    try:
        with urllib.request.urlopen(req, timeout=30) as response:
            result = json.loads(response.read().decode('utf-8'))
            return result.get('content', '').strip()
    except Exception as e:
        print(f"[API Error] {e}")
        return ""

def parse_result(output):
    """解析输出结果"""
    if not output:
        return -1
    if "不符合" in output:
        return 0
    if "符合" in output:
        return 1
    return -1

def calculate_metrics(y_true, y_pred):
    """计算评估指标"""
    # 过滤掉无法判断(-1)的样本
    valid_indices = [i for i, x in enumerate(y_pred) if x != -1]
    
    if not valid_indices:
        return 0, 0, 0, 0, "No valid samples"
        
    y_true_valid = [y_true[i] for i in valid_indices]
    y_pred_valid = [y_pred[i] for i in valid_indices]

    if HAS_SKLEARN:
        # 使用 sklearn 计算
        acc = accuracy_score(y_true_valid, y_pred_valid)
        # pos_label=1 关注 "符合" 类
        prec = precision_score(y_true_valid, y_pred_valid, pos_label=1, zero_division=0)
        rec = recall_score(y_true_valid, y_pred_valid, pos_label=1, zero_division=0)
        f1 = f1_score(y_true_valid, y_pred_valid, pos_label=1, zero_division=0)
        report = classification_report(y_true_valid, y_pred_valid, target_names=['不符合', '符合'], zero_division=0)
    else:
        # 手动计算
        tp = sum(1 for t, p in zip(y_true_valid, y_pred_valid) if t == 1 and p == 1)
        # tn = sum(1 for t, p in zip(y_true_valid, y_pred_valid) if t == 0 and p == 0)
        fp = sum(1 for t, p in zip(y_true_valid, y_pred_valid) if t == 0 and p == 1)
        fn = sum(1 for t, p in zip(y_true_valid, y_pred_valid) if t == 1 and p == 0)
        
        acc = sum(1 for t, p in zip(y_true_valid, y_pred_valid) if t == p) / len(y_true_valid)
        prec = tp / (tp + fp) if (tp + fp) > 0 else 0
        rec = tp / (tp + fn) if (tp + fn) > 0 else 0
        f1 = 2 * (prec * rec) / (prec + rec) if (prec + rec) > 0 else 0
        report = f"Manual Calc: Accuracy={acc:.4f}, Precision={prec:.4f}, Recall={rec:.4f}, F1={f1:.4f}"

    return acc, prec, rec, f1, report

def run_test():
    # 读取 Excel
    excel_file = "text.xlsx"
    if not os.path.exists(excel_file):
        print(f"找不到 {excel_file}")
        return

    try:
        df = pd.read_excel(excel_file)
    except Exception as e:
        print(f"读取 Excel 失败: {e}")
        return

    # 检查列名 (兼容处理)
    text_col = '文本' if '文本' in df.columns else 'text'
    label_col = '标签' if '标签' in df.columns else 'label'
    
    if text_col not in df.columns or label_col not in df.columns:
        print("Excel 列名不匹配，需要 '文本' 和 '标签'")
        return

    # 启动服务
    if not start_server():
        return

    results = []
    y_true_list = []
    y_pred_list = []
    total = len(df)
    
    print("-" * 70)
    start_test_time = time.time()

    for idx, row in df.iterrows():
        id_val = row.get('id', idx + 1)
        text = row[text_col]
        true_label = row[label_col]
        
        # 构造 Prompt
        prompt = f"这句话是否符合逻辑：{text}。输出符合或者不符合"
        
        # 调用 API (极速！)
        llm_output = call_llm(prompt)
        pred_label = parse_result(llm_output)
        
        # 收集标签用于后续计算指标
        y_true_list.append(true_label)
        y_pred_list.append(pred_label)

        # 打印状态
        pred_str = "符合" if pred_label == 1 else ("不符合" if pred_label == 0 else "无法判断")
        true_str = "符合" if true_label == 1 else "不符合"
        status = "✓" if pred_label == true_label else ("?" if pred_label == -1 else "✗")
        
        print(f"[{idx+1}/{total}] {status} 预测:{pred_str} | 真实:{true_str}")
        
        results.append({
            'id': id_val,
            '文本': text,
            '真实标签': true_label,
            '预测标签': pred_label,
            '预测结果': pred_str,
            '是否正确': status,
            '原始输出': llm_output
        })

    total_time = time.time() - start_test_time
    print("-" * 70)
    
    # 计算详细指标
    acc, prec, rec, f1, report = calculate_metrics(y_true_list, y_pred_list)
    print(f"测试完成！总耗时: {total_time:.2f} 秒，平均每条: {total_time/total:.2f} 秒")
    
    print("\n" + "="*30 + " 分类性能指标 " + "="*30)
    print(f"准确率 (Accuracy):  {acc:.2%}")
    print(f"精确率 (Precision): {prec:.2%}")
    print(f"召回率 (Recall):    {rec:.2%}")
    print(f"F1 分数 (F1 Score): {f1:.2%}")
    print("-" * 76)
    if HAS_SKLEARN:
        print("详细报告:\n", report)
    print("=" * 76 + "\n")

    # 保存计算结果到 csv
    metrics_data = [{"Metric": k, "Value": v} for k, v in zip(
        ["Accuracy", "Precision", "Recall", "F1_Score"],
        [acc, prec, rec, f1]
    )]
    pd.DataFrame(metrics_data).to_csv('accuracy_metrics.csv', index=False)
    
    # 保存结果
    try:
        pd.DataFrame(results).to_excel('test_results_fast.xlsx', index=False)
        print("详细结果已保存到 test_results_fast.xlsx (含分类指标 accuracy_metrics.csv)")
    except Exception as e:
        print(f"保存结果失败: {e}")
    
    # 错误案例
    errors = [r for r in results if r['真实标签'] != r['预测标签'] and r['预测标签'] != -1]
    if errors:
        try:
            pd.DataFrame(errors).to_excel('error_cases_fast.xlsx', index=False)
            print("错误案例已保存到 error_cases_fast.xlsx")
        except:
            pass

if __name__ == "__main__":
    try:
        run_test()
    except KeyboardInterrupt:
        print("\n测试中断")
    finally:
        cleanup_server()
