# 通用 Calendar Heatmap 实现规范

请将系统中的日期热力图统一抽象为两种标准模式：

1. `rolling-52-weeks`：近 52 周
2. `calendar-year`：自然年

两种模式共用同一套日期、星期、月份和数据映射基础逻辑，但时间范围计算必须严格分离。

---

# 一、统一基础规则

## 1. 日期单位

热力图最小单位是：

`calendar day`

数据 Key 统一使用：

`YYYY-MM-DD`

例如：

2026-09-04

不要使用时间戳作为前端热力图的数据匹配 Key。

---

## 2. 星期排列

统一采用 Monday-first：

周一
周二
周三
周四
周五
周六
周日

内部：

Monday    = 0
Tuesday   = 1
Wednesday = 2
Thursday  = 3
Friday    = 4
Saturday  = 5
Sunday    = 6

JavaScript `Date.getDay()` 是：

Sunday = 0
Monday = 1
...
Saturday = 6

因此必须转换：

const weekdayIndex = (date.getDay() + 6) % 7

---

## 3. 每个日期的数据结构

建议统一为：

type HeatmapDay = {
  date: string
  value: number | null
  weekIndex: number
  weekdayIndex: number
  inRange: boolean
  isFuture: boolean
}

职责明确：

- `weekIndex`：X 轴
- `weekdayIndex`：Y 轴
- `date`：数据匹配
- `value`：业务数据

不要依赖 DOM 顺序反推日期。

---

# 二、模式 A：近 52 周

## 定义

“近 52 周”定义为：

> 包含当前周在内，连续显示最近 52 个 calendar weeks。

不是：

- 最近 365 天
- 最近 364 天简单切块
- 当前日期减一年

---

## 1. 时间范围

假设：

today = 2026-09-04 Friday

Monday-first 时：

currentWeekStart = 2026-08-31

第一列：

firstWeekStart =
  currentWeekStart - 51 weeks

因此：

week 0  = firstWeekStart
...
week 51 = currentWeekStart

始终严格：

52 columns

---

## 2. 当前周

例如当前日期：

2026-09-04

最后一列：

周一  2026-08-31
周二  2026-09-01
周三  2026-09-02
周四  2026-09-03
周五  2026-09-04
周六  2026-09-05
周日  2026-09-06

其中未来日期：

2026-09-05
2026-09-06

保留格子位置，但：

value = null
isFuture = true

不要把未来日期当作 0 次活动。

`0` 和 `future/null` 是不同语义。

---

## 3. 左边界

第一列也是完整 calendar week。

因此第一列可能包含一部分早于“实际 52×7 日窗口”的日期。

这是正常的。

本模式追求：

52 个完整周列

而不是：

精确 364 个业务日期。

---

## 4. 月份标签

月份标签不能根据：

weekStart.getMonth()

生成。

必须根据：

“每个月的 1 日落在哪一个 week column”

确定。

例如：

2026-08-31 Monday
2026-09-01 Tuesday
...
2026-09-06 Sunday

虽然该周 `weekStart` 是 8 月 31 日，

但：

2026-09-01

位于这一列，所以：

`9月`

必须定位到这一列。

逻辑应类似：

monthStart = YYYY-MM-01
weekIndex = getWeekIndex(monthStart)

然后将月份标签锚定到这个 weekIndex。

---

## 5. 适用场景

近 52 周适合：

- GitHub 风格活动图
- API 请求活动
- Token 使用量
- 登录活动
- 调用次数
- 错误事件
- 最近一年运行状态

特点：

- 永远 52 列
- 页面宽度稳定
- 随时间滚动
- 关注“最近活动”

---

# 三、模式 B：自然年

## 定义

“一年”定义为：

> 指定自然年的 1 月 1 日至 12 月 31 日。

例如：

2026-01-01
～
2026-12-31

不是：

today - 365 days

也不是：

最近 52 周。

---

## 1. 时间范围

指定：

year = 2026

则：

rangeStart = 2026-01-01
rangeEnd   = 2026-12-31

闰年自动为：

366 days

普通年：

365 days

---

## 2. 周列

为了维持 Monday → Sunday 的二维布局，需要把：

1 月 1 日所在周

作为第一列，

把：

12 月 31 日所在周

作为最后一列。

因此自然年热力图：

**不应强制固定为 52 列。**

通常会出现：

53 列

某些周历布局可能需要：

54 个视觉列

取决于：

- 星期起始规则
- 年初星期位置
- 年末星期位置

正确原则是：

> 根据自然年的实际日期计算 week columns，不写死 52。

---

# 四、自然年的年初空白

例如：

2026-01-01 是 Thursday。

那么第一列：

Monday    2025-12-29
Tuesday   2025-12-30
Wednesday 2025-12-31
Thursday  2026-01-01
Friday    2026-01-02
Saturday  2026-01-03
Sunday    2026-01-04

但自然年模式只统计 2026。

因此：

2025-12-29
2025-12-30
2025-12-31

对应格子可以保留布局占位，但必须：

inRange = false
value = null

视觉上为空白/禁用格。

不能读取 2025 年数据填进去。

---

# 五、自然年的年末空白

同理，如果 12 月 31 日不是 Sunday，

最后一列可能进入下一年。

例如：

2027-01-01
2027-01-02
...

这些日期：

inRange = false
value = null

不得计入当前年份。

---

# 六、当前自然年

如果展示：

2026 年

且今天：

2026-09-04

那么：

2026-09-05
～
2026-12-31

均属于：

inRange = true
isFuture = true
value = null

这里需要区分三种状态：

### 年外日期

inRange = false

### 年内未来日期

inRange = true
isFuture = true

### 年内有效日期

inRange = true
isFuture = false

不要全部简单渲染成：

value = 0

---

# 七、历史自然年

如果展示：

2025 年

而当前已经是：

2026 年

则：

2025-01-01 ～ 2025-12-31

全部属于已完成日期。

不存在未来格子。

---

# 八、月份标签

自然年模式同样采用：

monthStart

作为锚点。

分别计算：

2026-01-01
2026-02-01
2026-03-01
...
2026-12-01

属于哪个：

weekIndex

然后在对应列显示：

1月
2月
3月
...
12月

不要按照每周 Monday 属于哪个月份生成标签。

---

# 九、推荐 UI

## A. 近 52 周

标题：

Token 活动 · 近 52 周

横向：

约 52 个固定周列

月份标签：

9月 10月 11月 ... 8月 9月

这是滚动窗口，因此首尾月份重复是完全正常的。

例如可能：

9月 → 10月 → ... → 8月 → 9月

---

## B. 自然年

标题：

Token 活动 · 2026

或：

Token 活动 · 本年

提供年份选择：

‹ 2025   2026 ›

月份固定：

1月 → 2月 → ... → 12月

适合年度回顾和历史比较。

---

# 十、两种模式的核心区别

| 项目 | 近 52 周 | 自然年 |
|---|---|---|
| 时间语义 | 滚动窗口 | 自然年度 |
| 起点 | 当前周往前 51 周 | 1 月 1 日 |
| 终点 | 当前周 | 12 月 31 日 |
| 周列 | 固定 52 | 动态计算 |
| 日期数 | 最多约 364 天 | 365/366 天 |
| 跨年 | 必然可能 | 不跨业务年度 |
| 月份 | 可能首尾重复 | 固定 1～12 月 |
| 未来日期 | 当前周未来几天 | 当前年剩余日期 |
| 适合 | 最近活动 | 年度统计 |

---

# 十一、不要混用两个概念

禁止将：

“近 52 周”

实现为：

new Date(today.getFullYear() - 1, ...)

也禁止将：

“自然年”

实现为：

currentWeek - 51 weeks

这两者虽然都接近“一年”，但统计语义完全不同。

---

# 十二、日期/时区

热力图属于 date-only 数据。

避免：

new Date("2026-09-01")

后又混合：

getUTCDate()
getDate()
getUTCDay()
getDay()

建议提供统一 date helper。

例如：

createLocalDate(year, month, day)

或者全套 UTC date-only helper。

整个 heatmap 只能选择一种时间体系。

禁止 UTC/local 混用导致：

日期 ±1
星期错位
月份错位

---

# 十三、推荐通用 API

可以抽象：

buildCalendarHeatmap({
  mode: "rolling-52-weeks" | "calendar-year",
  year?: number,
  today: Date,
  weekStartsOn: "monday"
})

返回：

{
  weeks: HeatmapDay[][],
  monthLabels: [
    {
      year: number,
      month: number,
      weekIndex: number
    }
  ],
  rangeStart: string,
  rangeEnd: string
}

---

# 十四、关键测试

必须覆盖：

### rolling-52-weeks

today = 2026-09-04

最后一列必须：

2026-08-31 Monday
2026-09-01 Tuesday
2026-09-02 Wednesday
2026-09-03 Thursday
2026-09-04 Friday
2026-09-05 Saturday future
2026-09-06 Sunday future

且：

9月 label → 该列

总列数：

52

---

### calendar-year

year = 2026

必须：

rangeStart = 2026-01-01
rangeEnd   = 2026-12-31

第一列允许包含：

2025-12-29 ～ 2025-12-31

但这些必须：

inRange = false

同样检查年底跨入 2027 的占位日期。

---

### 其他边界

必须测试：

- 月初为 Monday
- 月初为 Sunday
- 12 月 → 1 月
- 普通年
- 闰年
- 2028-02-29
- 当前年
- 历史年
- UTC/local 边界

---

# 十五、最终原则

两个热力图应遵循：

### 近 52 周

固定周数，滚动时间窗口。

### 自然年

固定年度，动态周数。

月份永远根据：

`monthStart 所属 weekIndex`

定位。

星期永远根据：

真实 calendar date

定位。

业务数据永远根据：

`YYYY-MM-DD`

匹配。

不要使用视觉位置反推业务日期。
