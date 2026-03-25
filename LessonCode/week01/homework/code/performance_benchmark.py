# filepath: d:\AAAstudy materials\jinshan\CODE\llama-cpu\performance_benchmark.py
import subprocess
import time
import sys
import io
import os
import json
import urllib.request
import atexit
import pandas as pd
import random

# 尝试导入绘图库，如果没有则后续跳过绘图
try:
    import matplotlib.pyplot as plt
    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False

# 设置编码
if sys.platform == 'win32':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

# 配置
SERVER_EXE = r"llama-server.exe" 
MODEL_PATH = r"models\qwen2.5-1.5b-instruct-q4_0.gguf"
PORT = 8081 # 使用不同端口避免冲突
HOST = "127.0.0.1"
API_URL = f"http://{HOST}:{PORT}/completion"

server_process = None

def cleanup_server():
    global server_process
    if server_process:
        print("\n[系统] 正在关闭 LLM 服务...")
        try:
            server_process.kill()
        except:
            pass
        server_process = None

atexit.register(cleanup_server)

def start_server():
    global server_process
    print("=" * 70)
    print(f"正在启动 LLM 性能测试服务 (端口 {PORT})...")
    print("=" * 70)
    
    if not os.path.exists(MODEL_PATH):
        print(f"错误: 找不到模型 {MODEL_PATH}")
        return False

    cmd = [
        SERVER_EXE,
        "-m", MODEL_PATH,
        "--port", str(PORT),
        "-c", "4096",      # 更大的上下文以容纳长输入
        "--temp", "0.0",
        "--n-gpu-layers", "0"
    ]

    try:
        server_process = subprocess.Popen(
            cmd,
            stdout=subprocess.DEVNULL,
            stderr=None 
        )
    except FileNotFoundError:
        print(f"错误: 找不到 {SERVER_EXE}")
        return False

    print("等待服务就绪 (预计 10-20 秒)...")
    
    start_time = time.time()
    while time.time() - start_time < 120:
        if server_process.poll() is not None:
            return False
        try:
            with urllib.request.urlopen(f"http://{HOST}:{PORT}/health", timeout=1) as response:
                if response.status == 200:
                    print("\n>>> 服务已就绪！开始基准测试！ <<<\n")
                    return True
        except:
            time.sleep(1)
            pass
    return False

def stream_request(prompt, n_predict=128):
    """
    发送流式请求并计算性能指标
    """
    formatted_prompt = f"<|im_start|>user\n{prompt}<|im_end|>\n<|im_start|>assistant\n"
    data = {
        "prompt": formatted_prompt,
        "n_predict": n_predict,
        "temperature": 0.0,
        "stream": True,  # 开启流式模式关键
        "stop": ["<|im_end|>"]
    }

    req = urllib.request.Request(
        API_URL,
        data=json.dumps(data).encode('utf-8'),
        headers={'Content-Type': 'application/json'}
    )

    t0 = time.time() # 开始请求时间
    ttft = 0         # 首字时间
    first_token_time = None
    token_count = 0
    full_response = []

    try:
        with urllib.request.urlopen(req, timeout=60) as response:
            for line in response:
                line = line.decode('utf-8').strip()
                if not line.startswith("data: "):
                    continue
                
                content_str = line[6:] # 去掉 'data: '
                if content_str == "[DONE]":
                    break
                
                try:
                    chunk = json.loads(content_str)
                    
                    # 捕获第一个包含内容的 token
                    if 'content' in chunk and chunk['content']:
                        if first_token_time is None:
                            first_token_time = time.time()
                            ttft = first_token_time - t0
                        
                        token_count += 1
                        full_response.append(chunk['content'])
                        
                    if chunk.get('stop', False):
                        break
                except:
                    pass
    except Exception as e:
        print(f"请求失败: {e}")
        return None

    t_end = time.time()
    e2e_latency = t_end - t0
    
    # 计算 TPOT
    # 公式: (Total Time - TTFT) / (New Tokens - 1)
    # 或者是 decode 阶段的总时间 / 生成的 token 数
    # 这里用简化算法: 
    generate_time = e2e_latency - ttft
    tpot = (generate_time / (token_count - 1)) if token_count > 1 else 0

    return {
        "input_len": len(prompt), # 字符长度近似
        "ttft_ms": ttft * 1000,
        "e2e_ms": e2e_latency * 1000,
        "tpot_ms": tpot * 1000,
        "tokens": token_count,
        "tps": token_count / e2e_latency if e2e_latency > 0 else 0 # Tokens Per Second
    }

def generate_test_inputs():
    """生成 10 组不同长度的测试输入"""
    base_text = "这是一段用于测试模型性能的文本。我们需要评估模型在处理不同长度输入时的首字延迟和生成速度。"
    inputs = []
    
    # 生成从 1 倍到 50 倍长度的输入
    # 长度梯度: 少量文本 -> 大量文本
    scales = [1, 2, 5, 10, 15, 20, 30, 40, 50, 60]
    
    for i, scale in enumerate(scales):
        # 构造 prompt，让模型做摘要，这样输出长度相对可控，主要测输入处理能力
        long_input = base_text * scale
        prompt = f"请总结以下这段文字（字数限制100字以内）：\n\n{long_input}"
        inputs.append({
            "id": i + 1,
            "scale": scale,
            "text": prompt,
            "desc": f"Input ~{len(long_input)} chars"
        })
    
    return inputs

def main():
    if not start_server():
        return

    test_cases = generate_test_inputs()
    results = []

    print(f"{'ID':<4} | {'Input Size':<15} | {'TTFT (ms)':<10} | {'TPOT (ms)':<10} | {'E2E (s)':<10} | {'TPS':<8}")
    print("-" * 80)

    for case in test_cases:
        # 预热一下或者直接测? 这里直接测，但第一条可能会稍慢（Cache Miss）
        metrics = stream_request(case["text"], n_predict=100) # 限制输出长度，主要测输入对 TTFT 的影响
        
        if metrics:
            print(f"{case['id']:<4} | {case['desc']:<15} | {metrics['ttft_ms']:>10.2f} | {metrics['tpot_ms']:>10.2f} | {metrics['e2e_ms']/1000:>10.2f} | {metrics['tps']:>8.2f}")
            
            results.append({
                "ID": case['id'],
                "Input_Length_Chars": metrics['input_len'],
                "TTFT_ms": metrics['ttft_ms'],
                "TPOT_ms": metrics['tpot_ms'],
                "E2E_Latency_ms": metrics['e2e_ms'],
                "Tokens_Generated": metrics['tokens'],
                "Tokens_Per_Second": metrics['tps']
            })
        else:
            print(f"{case['id']} - Failed")

    # 保存 CSV
    df = pd.DataFrame(results)
    df.to_csv("performance_report.csv", index=False, encoding='utf-8-sig')
    print("\n[完成] 报告已保存至 performance_report.csv")

    # 绘图
    if HAS_MATPLOTLIB and len(results) > 0:
        plt.figure(figsize=(12, 6))
        
        # 子图 1: Input Length vs TTFT
        plt.subplot(1, 2, 1)
        plt.plot(df["Input_Length_Chars"], df["TTFT_ms"], 'o-', label="TTFT (ms)")
        plt.xlabel("Input Length (Characters)")
        plt.ylabel("Time to First Token (ms)")
        plt.title("Latency Analysis: Input Size vs TTFT")
        plt.grid(True)
        plt.legend()

        # 子图 2: Input Length vs TPS
        plt.subplot(1, 2, 2)
        plt.plot(df["Input_Length_Chars"], df["Tokens_Per_Second"], 's-', color='orange', label="Throughput (Tokens/s)")
        plt.xlabel("Input Length (Characters)")
        plt.ylabel("Tokens Per Second")
        plt.title("Throughput Analysis")
        plt.grid(True)
        plt.legend()

        plt.tight_layout()
        plt.savefig("performance_charts.png")
        print("[完成] 图表已保存至 performance_charts.png")
    else:
        print("[提示] 未检测到 matplotlib 或无数据，跳过绘图。")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    finally:
        cleanup_server()
