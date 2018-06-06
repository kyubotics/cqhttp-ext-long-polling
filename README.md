# CoolQ HTTP API 插件 - 长轮询扩展

[![License](https://img.shields.io/badge/license-MIT%20License-blue.svg)](LICENSE)
[![Release](https://img.shields.io/github/release/richardchien/cqhttp-ext-long-polling.svg)](https://github.com/richardchien/cqhttp-ext-long-polling/releases)

通过扩展（Extension）给 CoolQ HTTP API 插件提供类似 Telegram 的长轮询接口。

目前还不完善，有很多问题需要解决。

## API 说明

API 请求方式和 HTTP API 插件正常的请求一样。

### `/get_updates` 获取更新

#### 参数

| 字段名 | 数据类型 | 默认值 | 说明 |
| ----- | ------- | ----- | --- |
| `limit` | number | `100` | 获取的事件数量上限 |
| `timeout` | number | `0` | 没有事件时要等待的秒数，0 表示使用短轮询，不等待 |

#### 响应数据

`data` 字段为事件数据的数组。
