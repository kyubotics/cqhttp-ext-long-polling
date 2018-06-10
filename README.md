# CoolQ HTTP API 插件 - 长轮询扩展

[![License](https://img.shields.io/badge/license-MIT%20License-blue.svg)](LICENSE)
[![Release](https://img.shields.io/github/release/richardchien/cqhttp-ext-long-polling.svg)](https://github.com/richardchien/cqhttp-ext-long-polling/releases)

通过扩展（Extension）给 CoolQ HTTP API 插件提供类似 Telegram 的长轮询接口。

## 使用方式

将 `LongPolling.dll` 放到酷 Q 的 `app\io.github.richardchien.coolqhttpapi\extensions` 目录下（没有则创建），然后配置文件中添加 `long_polling.enable = true`，**并将 `server_thread_pool_size` 设为大于 2 的值**，然后重启插件即可。

默认在队列中保留 2000 条消息，可通过 `long_polling.max_queue_size` 设置（0 表示不限制队列大小，谨慎使用）。

## API 说明

API 请求方式和 HTTP API 插件正常的请求一样。

### `/get_updates` 获取更新

任何时候只能有一个请求正在被处理，如果已经有请求正在阻塞地等待新的事件，后来的请求将会直接返回，`retcode` 为 `10100`。

#### 参数

| 字段名 | 数据类型 | 默认值 | 说明 |
| ----- | ------- | ----- | --- |
| `limit` | number | `100` | 获取的事件数量上限 |
| `timeout` | number | `0` | 没有事件时要等待的秒数，0 表示使用短轮询，不等待 |

#### 响应数据

`data` 字段为事件数据的数组。
