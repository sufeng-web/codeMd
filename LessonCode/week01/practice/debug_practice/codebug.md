# 项目代码问题

本文档列出 `debug_practice` 项目中用于调试练习的**刻意埋入的 bug**。可通过断点、监视（Watch）和单步进入（Step Into）定位并修复。

---

## 1. score_reader.cpp — 输入循环 off-by-one


| 项   | 说明                                |
| ------ | ------------------------------------- |
| 文件 | `score_reader.cpp`                  |
| 行号 | 16                                  |
| 代码 | `for (int i = 0; i <= count_; ++i)` |

**问题：** 条件使用了 `i <= count_`，循环会多读 **count_ + 1** 个分数。例如 5 门课时会读 6 个数，第 6 个被写入 `scores_[5]`。当 `count_ == 10` 时，会写入 `scores_[10]`，发生越界（合法下标为 0–9）。

**修复：** 改为 `i < count_`。

---

## 2. score_analyzer.cpp — setGlobalCourseCount 传入错误参数


| 项   | 说明                                   |
| ------ | ---------------------------------------- |
| 文件 | `score_analyzer.cpp`                   |
| 行号 | 34                                     |
| 代码 | `setGlobalCourseCount(g_courseCount);` |

**问题：** `g_courseCount` 从未被 reader 赋值，一直为 0，因此报告里“Courses”始终显示 0，而不是实际课程数。

**修复：** 改为 `setGlobalCourseCount(reader.getCount());`。

---

## 3. statistics.cpp — 求和循环 off-by-one（越界读）


| 项   | 说明                               |
| ------ | ------------------------------------ |
| 文件 | `statistics.cpp`                   |
| 行号 | 7                                  |
| 代码 | `for (int i = 0; i <= count; ++i)` |

**问题：** 条件使用了 `i <= count`，会访问 `scores[count]`，越界；同时多累加了一个元素，导致 `result.total` 错误。

**修复：** 改为 `i < count`。

---

## 4. statistics.cpp — 平均分使用整数除法


| 项   | 说明                                     |
| ------ | ------------------------------------------ |
| 文件 | `statistics.cpp`                         |
| 行号 | 11                                       |
| 代码 | `result.average = result.total / count;` |

**问题：** `result.total` 与 `count` 均为整数，除法为整数除法，结果赋给 `result.average`（double）时小数部分已丢失（例如 394/5 → 78，而不是 78.8）。

**修复：** 改为 `result.average = static_cast<double>(result.total) / count;`。

---

## 5. statistics.cpp — 未初始化变量（passCount）


| 项   | 说明                                       |
| ------ | -------------------------------------------- |
| 文件 | `statistics.cpp`                           |
| 行号 | 13–18                                     |
| 代码 | `int passCount;` 后在循环中 `passCount++;` |

**问题：** `passCount` 未初始化。循环仅在 `scores[i] >= kMaxCourses` 时自增，最终值未定义，报告的“及格人数”错误。

**修复：** 初始化为 `int passCount = 0;`。

---

## 6. statistics.cpp — 及格线用错常量


| 项   | 说明                            |
| ------ | --------------------------------- |
| 文件 | `statistics.cpp`                |
| 行号 | 15                              |
| 代码 | `if (scores[i] >= kMaxCourses)` |

**问题：** 使用了 `kMaxCourses`（10）而不是 `kPassThreshold`（60），统计的是“分数 ≥ 10”而不是“分数 ≥ 60”（及格）。

**修复：** 改为 `if (scores[i] >= kPassThreshold)`。

---

## 7. statistics.cpp — 最大分存的是下标而不是分数值


| 项   | 说明                                         |
| ------ | ---------------------------------------------- |
| 文件 | `statistics.cpp`                             |
| 行号 | 20–26                                       |
| 代码 | `maxVal = i;` 与 `result.maxScore = maxVal;` |

**问题：** 发现更大分数时，代码把**下标** `i` 赋给 `maxVal`，而不是分数 `scores[i]`，因此报告输出的是最大分的下标（如 2），而不是最大分数（如 90）。

**修复：** 在循环中改为 `maxVal = scores[i];`，并保留 `result.maxScore = maxVal;`（或在循环结束后再赋给 `result.maxScore`）。

---

## 8. name_reader.cpp — 拷贝后缺少 null 结尾符


| 项   | 说明                                   |
| ------ | ---------------------------------------- |
| 文件 | `name_reader.cpp`                      |
| 行号 | 15–18                                 |
| 代码 | 仅有拷贝循环；没有`m_name[len] = '\0'` |

**问题：** 构造函数只设置了 `m_name[0] = '\0'`。拷贝循环会覆盖 `m_name[0]` … `m_name[len-1]`，但从未写入 `m_name[len] = '\0'`，因此拷贝完成后缓冲区不是以 null 结尾的，`m_name[len]` 为未初始化值。输出或后续字符串操作可能出现乱码或不可预期行为。

**修复：** 在循环后增加 `m_name[len] = '\0';`。

---

## 9. 编码规范问题

| 项   | 说明     |
|------|----------|
| 范围 | 多个文件 |
| 类型 | 函数、类、文件、变量等命名与风格 |

**问题：** 存在命名或风格不统一：如函数/变量命名风格不一致、文件名与职责不清晰、类名或成员命名不符合项目约定等，影响可读性和后续维护。

**修复：** 统一遵循项目编码规范：函数与变量使用一致命名风格（如小写+下划线或驼峰）；类名、文件名与职责对应；成员变量可加统一前缀（如 `m_`）以便区分；必要时在项目根目录或文档中补充命名约定说明。

---

## 汇总


| # | 文件               | 问题概要                                                            |
| --- | -------------------- | --------------------------------------------------------------------- |
| 1 | score_reader.cpp   | 循环`i <= count_` → 多读一个分数，count_=10 时越界                 |
| 2 | score_analyzer.cpp | `setGlobalCourseCount(g_courseCount)` → 应使用 `reader.getCount()` |
| 3 | statistics.cpp     | 求和循环`i <= count` → 越界读、total 错误                          |
| 4 | statistics.cpp     | 平均分使用整数除法                                                  |
| 5 | statistics.cpp     | `passCount` 未初始化                                                |
| 6 | statistics.cpp     | 及格条件用了`kMaxCourses` 而非 `kPassThreshold`                     |
| 7 | statistics.cpp     | `maxVal = i` 应改为 `maxVal = scores[i]`                            |
| 8 | name_reader.cpp    | 拷贝后未写 `m_name[len] = '\0'`                                      |
| 9 | 多文件 / 编码规范 | 函数、类、文件、变量等命名与风格不统一，影响可读与维护               |

输入姓名 "Alice"、5 门课、分数 80, 72, 90, 65, 88 时，正确输出应为：

- Total: 395, Average: 79.00, Pass count: 4, Max: 90, Courses: 5, Student: Alice
