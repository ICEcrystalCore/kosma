# Kosma Module System Design

## Overview

Kosma 框架提供两种模块化方案：**静态模块**和**动态模块**。
两者对模块开发者来说接口一致，区别仅在注册方式和加载方式。

---

## 核心原则

1. **用户只关心"定义模块"** — 继承 Module、声明依赖、实现 startup/shutdown
2. **框架控制流程** — ModuleManager、依赖解析、加载顺序等全部是框架私有实现，不暴露给用户
3. **静态和动态统一接口** — ModuleManager 只接收 Factory，不关心 Factory 来源

---

## Module 公开 API

```cpp
// 所有模块的基类
class Module {
public:
    virtual ~Module() = default;
    virtual void startup() = 0;
    virtual void shutdown() = 0;

protected:
    // 在模块内获取依赖模块（类型安全）
    template<typename T>
    T* getModule() const;
};
```

---

## 静态模块

### 概念

编译进主二进制，程序启动时通过静态初始化自动注册到全局注册表。
适用于引擎核心功能：日志、资源管理、渲染后端等。

### 用户侧用法

```cpp
class LogModule : public Module {
public:
    void startup() override { /* ... */ }
    void shutdown() override { /* ... */ }
};

// 一行宏完成注册，声明模块名和依赖
KOSMA_REGISTER_STATIC_MODULE(LogModule, "Log", {})

class RenderModule : public Module {
public:
    void startup() override {
        auto* log = getModule<LogModule>();
        // ...
    }
    void shutdown() override { /* ... */ }
};

KOSMA_REGISTER_STATIC_MODULE(RenderModule, "Render", {"Log"})
```

### 框架内部机制

```
程序启动
  │
  ▼
静态初始化阶段：KOSMA_REGISTER_STATIC_MODULE 宏产生的全局变量
  构造时将 Factory 注册到 StaticModuleRegistry（全局单例）
  │
  ▼
Application::initialize()
  │
  ├─ 从 StaticModuleRegistry 取出所有 Factory
  ├─ 全部灌入 ModuleManager
  ├─ ModuleManager 解析依赖、检测循环、拓扑排序
  └─ 按顺序 create() → startup()
  │
  ▼
Application::shutdown()
  │
  └─ ModuleManager 按逆序 shutdown() → 销毁
```

### 宏展开原理

```cpp
#define KOSMA_REGISTER_STATIC_MODULE(ModuleClass, Name, Deps)        \
    namespace {                                                       \
    class ModuleClass##Factory : public ::Kosma::Core::Module::Factory { \
    public:                                                           \
        ::Kosma::Core::Module::Name name() const override {           \
            return ::Kosma::Core::String(Name);                       \
        }                                                             \
        ::Kosma::Core::Vector<::Kosma::Core::Module::Name>            \
        requiredModules() const override { return Deps; }             \
        ::Kosma::Core::SharedPtr<::Kosma::Core::Module>               \
        create() const override {                                     \
            return ::Kosma::Core::makeShared<ModuleClass>();           \
        }                                                             \
    };                                                                \
    static ::Kosma::Core::StaticModuleRegistrar                       \
        s_reg_##ModuleClass(                                          \
            ::Kosma::Core::makeUnique<ModuleClass##Factory>());       \
    }
```

---

## 动态模块

### 概念

编译为独立的共享库（.dylib / .dll / .so），运行时按需加载。
适用于插件、编辑器扩展、第三方 mod 等。

### 用户侧用法

```cpp
// PhysicsPlugin.cpp — 编译为 PhysicsPlugin.dylib
class PhysicsPlugin : public Module {
public:
    void startup() override { /* ... */ }
    void shutdown() override { /* ... */ }
};

// 导出入口点，框架 dlopen 后调用此函数获取 Factory
KOSMA_REGISTER_DYNAMIC_MODULE(PhysicsPlugin, "Physics", {"Render"})
```

### 框架内部机制

```
Application::initialize()
  │
  ├─ 先加载所有静态模块（同上）
  │
  ├─ DynamicModuleLoader 扫描插件目录
  │   ├─ 对每个 .dylib / .dll：
  │   │   ├─ dlopen / LoadLibrary
  │   │   ├─ dlsym 查找导出的 kosma_create_factory() 函数
  │   │   └─ 调用该函数，获取 Factory
  │   └─ 将所有 Factory 灌入 ModuleManager
  │
  └─ ModuleManager 统一处理（不区分静态/动态来源）
```

### 导出入口

```cpp
#define KOSMA_REGISTER_DYNAMIC_MODULE(ModuleClass, Name, Deps)        \
    /* 同静态宏一样生成 Factory 类 */                                    \
    extern "C" KOSMA_PLUGIN_EXPORT                                    \
    ::Kosma::Core::Module::Factory* kosma_create_factory() {          \
        static ModuleClass##Factory factory;                          \
        return &factory;                                              \
    }
```

### 平台抽象

| 操作 | macOS / Linux | Windows |
|------|---------------|---------|
| 加载 | `dlopen` | `LoadLibrary` |
| 查找符号 | `dlsym` | `GetProcAddress` |
| 卸载 | `dlclose` | `FreeLibrary` |
| 导出宏 | `__attribute__((visibility("default")))` | `__declspec(dllexport)` |

---

## 对比总结

| | 静态模块 | 动态模块 |
|---|---|---|
| 编译形态 | 链接进主二进制 | 独立 .dylib / .dll / .so |
| 注册方式 | 静态初始化，自动注册 | dlopen + 导出入口函数 |
| 发现机制 | 程序启动即可用 | 扫描插件目录 |
| 热重载 | 不支持 | 可以支持（卸载 → dlclose → dlopen → 重新加载） |
| 部署 | 单一可执行文件 | 需要管理插件目录和版本 |
| 适用场景 | 引擎核心功能 | 插件、扩展、mod |

---

## 实现计划

### Phase 1: 静态模块

1. 设计 `StaticModuleRegistry`（全局注册表单例）
2. 实现 `KOSMA_REGISTER_STATIC_MODULE` 宏
3. 将 ModuleManager 接入 Application
4. 将现有的 ModuleManager / ModuleGraph / LoadHelper 等沉为私有实现
5. 用户侧只暴露 Module 基类 + 注册宏 + `getModule<T>()`
6. 写一个 LogModule 作为第一个真实模块验证流程

### Phase 2: 动态模块

1. 实现平台抽象层（dlopen / LoadLibrary 封装）
2. 实现 `DynamicModuleLoader`（扫描目录、加载共享库、提取 Factory）
3. 实现 `KOSMA_REGISTER_DYNAMIC_MODULE` 宏（导出入口函数）
4. 支持动态模块的卸载和可选的热重载

---

## 待讨论

- [ ] `getModule<T>()` 是否需要模块名参数，还是纯粹按类型查找？
- [ ] 是否需要可选依赖（soft dependency）？
- [ ] 动态模块的版本兼容性检查机制
- [ ] 模块是否需要更细粒度的生命周期阶段（preInit / postInit）？