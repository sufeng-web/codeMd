powershell -Command "Set-Content -Path 'README_summary.md' -Value '

#本地 LLM 在 CPU 环境下的性能与准确率测试总结

## 1. 项目产出文件说明

| 文件名 | 类型 | 说明 |
| :--- | :--- | :--- |
| **build.bat** | **执行脚本** | **双击此文件即可一键完成环境安装与所有测试** |
| `performance_benchmark.py` | 源码 | 测速脚本（读取 text.xlsx，计算 TTFT, TPS） |
| `accuracy_test_fast.py` | 源码 | 准确率脚本（调用 API，计算 F1, Recall） |
| `performance_report.csv` | 产出 | 性能测试的详细原始数据 |
| `performance_charts.png` | 产出 | 性能分布图表（可视化 TPS 波动） |
| `test_results_fast.xlsx` | 产出 | 每一条数据的预测结果对比表 |
| `accuracy_metrics.csv` | 产出 | **最终四大指标（准确率、精确率、召回率、F1）** |
| `error_cases_fast.xlsx` | 产出 | 专门记录模型判断错误的案例，便于分析 |
||||

## 文件说明：

目录：code data  llama text

code里面存放测试模型性能的源代码   

data：正面样例100个  反 面样例100个

result：对千问模型跑出来 的测评结果

build.bat: 一键 构建llama框架编译，把相应的模型放入到\llama\models 里面，即可在命令行运行。



## 2. 项目背景
本项目旨在 CPU 环境下（使用 `llama.cpp` / `llama-server.exe`）测试量化模型（Qwen2.5-1.5B-Instruct-int4）在处理中文短文本逻辑判断任务时的性能与准确率。

## 3. 遇到的问题与解决方案

### 问题一：性能图表横纵坐标不合理
- **现象**：初版性能测试生成的图表横坐标（Input Length）都在 500~3000 字符，且需要很长时间运行。
- **原因**：使用了自动生成的长文本压力测试脚本，而实际业务场景是 20 字以内的短句。
- **解决**：
  - 修改 `performance_benchmark.py`，不再生成虚拟长文本。
  - 改为直接读取 `text.xlsx` 中的真实短句。
  - 图表从“折线图”改为“分布直方图”或散点图，因为输入长度差异很小，主要关注 TPS（每秒生成 Token 数）的稳定性。

### 问题二：缺乏详细的分类评估指标
- **现象**：测试结果仅有“是否正确”的简单统计，无法区分模型是“误判”还是“漏判”。
- **需求**：需要计算准确率 (Accuracy)、召回率 (Recall) 和 F1 分数。
- **解决**：
  - 更新 `accuracy_test_fast.py`。
  - 引入 `sklearn.metrics`（或增加手动计算逻辑）来计算混淆矩阵。
  - 定义：
    - **Precision (精确率)**：模型说是“符合”的，有多少是真的符合？
    - **Recall (召回率)**：真的“符合”的，模型找出了多少？
    - **F1 Score**：二者的调和平均数，综合衡量指标。
  - 结果输出至 `accuracy_metrics.csv`。

### 问题三：自动化构建与 Git 交付
- **需求**：需要提交到 Git，并且其他人下载后通过双击 `.bat` 即可运行，无需手动敲 Python 命令。
- **解决**：
  - 创建 `build.bat` 脚本。
  - **功能 1**：自动检查 Python 环境并安装依赖 (`pip install pandas ...`)。
  - **功能 2**：按顺序执行性能测试和准确率测试。
  - **功能 3**：测试完成后自动生成所有报表并暂停展示结果。


