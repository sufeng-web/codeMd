import subprocess
import csv
import re
import time

# 配置路径
EXE_PATH = r"localllm.exe"
#MODEL_PATH = r"models/qwen2.5-1.5b-instruct-q4_0.gguf" 
MODEL_PATH = r"models/model.gguf" 
"ruct-q4_0.gguf"

def judge_appointment(text):
    """
    判断任命证书是否合理
    返回: 1(合理), 0(不合理), -1(无法判断)
    """
    prompt = f"""判断以下任命证书是否符合中文表达逻辑，只回答"合理"或"不合理"，不要输出其他内容：

任命证书：{text}

答案："""
    
    cmd = [
        EXE_PATH,
        "-m", MODEL_PATH,
        "-p", prompt,
        "-n", "10",
        "--temp", "0.0",
        "--no-display-prompt"
    ]
    
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=False,  # 禁用自动解码
            timeout=300  # 增加超时时间
        )
        
        # 手动解码输出
        output = result.stdout.decode('utf-8', errors='ignore').strip().lower()
        
        # 打印标准输出和错误输出
        print("STDOUT:", result.stdout.decode('utf-8', errors='ignore'))
        print("STDERR:", result.stderr.decode('utf-8', errors='ignore'))
        
        # 解析输出
        if "合理" in output and "不合理" not in output:
            return 1
        elif "不合理" in output:
            return 0
        else:
            # 尝试从输出中寻找
            if "合理" in output:
                return 1
            elif "不合理" in output:
                return 0
            return -1
            
    except subprocess.TimeoutExpired:
        print("超时")
        return -1
    except Exception as e:
        print(f"错误: {e}")
        return -1

def generate_test_data():
    """生成 200 条测试数据（100 正例 + 100 反例）"""
    positive_examples = []
    negative_examples = []
    
    # 正例模板（合理的任命证书）
    positive_templates = [
        "兹任命{name}为{position}，自{date}起生效。",
        "经研究决定，任命{name}担任{position}，任期{term}。",
        "特此任命{name}为{position}，即日起执行。",
        "根据公司发展需要，任命{name}为{position}，负责{responsibility}。",
        "聘任{name}为{position}，聘期{term}，自{date}起计算。"
    ]
    
    names = ["张三", "李四", "王五", "赵六", "陈七", "刘八", "周九", "吴十", "郑一", "林二"]
    positions = ["技术总监", "项目经理", "部门经理", "销售总监", "运营主管", "开发组长", "产品经理", "人力资源总监"]
    dates = ["2024年1月1日", "2024年3月15日", "2024年5月1日", "2024年7月1日", "2024年9月1日"]
    terms = ["一年", "两年", "三年", "自即日起"]
    responsibilities = ["技术研发工作", "项目管理", "团队建设", "业务拓展"]
    
    import random
    random.seed(42)
    
    # 生成 100 个正例
    for i in range(100):
        template = random.choice(positive_templates)
        text = template.format(
            name=random.choice(names),
            position=random.choice(positions),
            date=random.choice(dates),
            term=random.choice(terms),
            responsibility=random.choice(responsibilities)
        )
        positive_examples.append((i+1, text, 1))
    
    # 反例模板（不合理的任命证书）
    negative_templates = [
        "任命{name}为{position}，但{name}已于三年前离职。",
        "兹任命{name}为{position}，生效日期为过去时间。",
        "任命{name}担任{position}，但该职位已不存在。",
        "经研究，任命{name}为{position}，此人无相关工作经验。",
        "特此任命{name}为{position}，但{name}尚未入职。",
        "任命{name}为{position}，缺少具体生效日期。",
        "兹任命{name}为{position}，但{position}与{name}专业不符。",
        "任命{name}为{position}，生效日期模糊不清。",
        "特此任命{name}为{position}，但任命书未加盖公章。",
        "经讨论，任命{name}为{position}，但未说明任期。"
    ]
    
    # 生成 100 个反例
    for i in range(100):
        template = random.choice(negative_templates)
        text = template.format(
            name=random.choice(names),
            position=random.choice(positions),
            date=random.choice(dates)
        )
        negative_examples.append((101+i, text, 0))
    
    # 合并并打乱顺序
    all_examples = positive_examples + negative_examples
    random.shuffle(all_examples)
    
    return all_examples

print("=" * 80)
print("准确度测评 - 任命证书逻辑判断")
print("=" * 80)

# 生成测试数据
print("正在生成测试数据...")
test_data = generate_test_data()
print(f"已生成 {len(test_data)} 条测试数据（正例: 100, 反例: 100）")

# 进行测试
print("\n开始测试...")
print("-" * 80)

results = []
total = len(test_data)

for idx, (id_num, text, true_label) in enumerate(test_data, 1):
    print(f"\r进度: {idx}/{total} ({idx/total*100:.1f}%)", end="", flush=True)
    
    pred_label = judge_appointment(text)
    
    results.append({
        'id': id_num,
        'text': text,
        'true_label': true_label,
        'pred_label': pred_label
    })

print("\n\n测试完成！")

# 计算指标
valid_results = [r for r in results if r['pred_label'] != -1]
invalid_count = len(results) - len(valid_results)

y_true = [r['true_label'] for r in valid_results]
y_pred = [r['pred_label'] for r in valid_results]

# 混淆矩阵
tp = sum(1 for t, p in zip(y_true, y_pred) if t == 1 and p == 1)
tn = sum(1 for t, p in zip(y_true, y_pred) if t == 0 and p == 0)
fp = sum(1 for t, p in zip(y_true, y_pred) if t == 0 and p == 1)
fn = sum(1 for t, p in zip(y_true, y_pred) if t == 1 and p == 0)

# 计算指标
accuracy = (tp + tn) / len(valid_results) if valid_results else 0
precision = tp / (tp + fp) if (tp + fp) > 0 else 0
recall = tp / (tp + fn) if (tp + fn) > 0 else 0
f1 = 2 * (precision * recall) / (precision + recall) if (precision + recall) > 0 else 0

print("\n" + "=" * 80)
print("准确度测评报告")
print("=" * 80)
print(f"总样本数: {total}")
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

# 保存详细结果
with open('accuracy_results.csv', 'w', newline='', encoding='utf-8-sig') as f:
    writer = csv.DictWriter(f, fieldnames=['id', 'text', 'true_label', 'pred_label'])
    writer.writeheader()
    writer.writerows(results)

# 保存错误案例
errors = [r for r in results if r['true_label'] != r['pred_label'] and r['pred_label'] != -1]
if errors:
    with open('error_cases.csv', 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.DictWriter(f, fieldnames=['id', 'text', 'true_label', 'pred_label'])
        writer.writeheader()
        writer.writerows(errors)
    print(f"\n错误案例已保存到 error_cases.csv ({len(errors)} 条)")

print("\n" + "=" * 80)
print("结果已保存：")
print("  - accuracy_results.csv (全部结果)")
print("  - error_cases.csv (错误案例)")
print("=" * 80)