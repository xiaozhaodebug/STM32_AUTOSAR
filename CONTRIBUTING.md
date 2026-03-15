# 贡献指南

感谢您对 STM32F407 AUTOSAR 项目的关注！我们欢迎各种形式的贡献。

## 如何贡献

### 报告问题

如果您发现了 bug 或有功能建议，请通过 GitHub Issue 提交：

1. 检查是否已有相关 Issue
2. 创建新 Issue，并提供以下信息：
   - 问题描述
   - 复现步骤
   - 预期行为
   - 实际行为
   - 环境信息（硬件、软件版本等）

### 提交代码

1. **Fork 项目**
   ```bash
   git clone https://github.com/your-username/STM32F407_AUTOSAR.git
   cd STM32F407_AUTOSAR
   ```

2. **创建分支**
   ```bash
   git checkout -b feature/your-feature-name
   # 或
   git checkout -b fix/bug-description
   ```

3. **编写代码**
   - 遵循 C89 标准
   - 使用驼峰命名法
   - 添加 Doxygen 注释
   - 保持代码风格一致

4. **测试**
   ```bash
   mkdir build && cd build
   cmake ..
   make -j4
   # 在硬件上测试
   ```

5. **提交更改**
   ```bash
   git add .
   git commit -m "feat: 添加新功能描述"
   git push origin feature/your-feature-name
   ```

6. **创建 Pull Request**
   - 描述更改内容
   - 关联相关 Issue
   - 等待审核

## 代码规范

### 命名规范

- **函数**：驼峰命名法，如 `CanDriver_Init()`
- **变量**：驼峰命名法，如 `gTickCount`
- **宏定义**：全大写，下划线分隔，如 `CAN_STATUS_OK`
- **类型**：驼峰命名法，后缀 `_Type`，如 `CanMessage_Type`

### 注释规范

使用 Doxygen 风格注释：

```c
/**
 * @file        CanDriver.c
 * @brief       CAN 驱动实现
 * @details     STM32F407 CAN1 外设驱动
 * 
 * @author      [你的名字]
 * @date        2026-03-15
 */

/**
 * @brief  初始化 CAN 驱动
 * @param  None
 * @return CAN_STATUS_OK 成功，其他值失败
 * @note   必须在其他 CAN 操作前调用
 */
CanStatus CanDriver_Init(void);
```

### 代码风格

- 缩进：4 个空格
- 行宽：不超过 80 字符
- 花括号：另起一行
- 条件语句：必须使用花括号

```c
if (condition) {
    do_something();
} else {
    do_other();
}
```

## 提交信息规范

使用 [Conventional Commits](https://www.conventionalcommits.org/) 规范：

- `feat:` 新功能
- `fix:` 修复 bug
- `docs:` 文档更新
- `style:` 代码格式调整
- `refactor:` 重构
- `test:` 测试相关
- `chore:` 构建或辅助工具变动

示例：
```
feat: 添加 CAN 过滤器配置功能

- 支持标准帧和扩展帧过滤
- 可配置 32 位掩码模式

Closes #123
```

## 开发环境

### 推荐工具

- **编辑器**: VS Code + C/C++ 插件
- **编译器**: GCC ARM Embedded
- **调试器**: OpenOCD + J-Link/ST-Link
- **版本控制**: Git

### VS Code 配置

项目已包含 `.vscode` 目录，包含推荐配置：

- `c_cpp_properties.json` - C/C++ 配置
- `launch.json` - 调试配置
- `tasks.json` - 构建任务

## 测试要求

提交代码前请确保：

- [ ] 代码能编译通过
- [ ] 在硬件上测试通过
- [ ] 不引入新的警告
- [ ] 相关文档已更新

## 问题咨询

如有疑问，欢迎通过以下方式联系：

- 创建 GitHub Discussion
- 发送邮件至：your.email@example.com

## 行为准则

参与本项目即表示您同意遵守我们的行为准则：

- 尊重所有参与者
- 保持专业和友善
- 接受建设性批评
- 关注社区最大利益

## 许可证

通过贡献代码，您同意将其许可为 MIT 许可证。
