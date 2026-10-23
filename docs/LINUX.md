# Linux实践环境

方案：Colima提供轻量Linux虚拟机，Docker容器内使用Ubuntu 24.04 ARM64开发环境。容器是隔离的运行环境，共享虚拟机的Linux内核；不同于macOS原生进程。

本项目使用独立profile `led-practice` 和Docker context `colima-led-practice`；不设为默认context、不配置开机启动、不挂载Mac主目录。分配2个CPU、2GiB内存、12GiB虚拟数据盘及默认20GiB根盘（均为稀疏磁盘，实际占用按写入增长）。

## 运行

在Mac项目目录执行：

```sh
make linux-test
make linux-shell
```

第一条从当前源码构建镜像并运行完整验证；第二条进入已有镜像的Linux终端。若修改了Mac源码，需要重新运行第一条更新镜像。容器内源码为副本，退出后容器内修改不会回写Mac。

在Linux终端使用 `uname -s -m` 查看环境；`exit`返回Mac终端。结束实践后在Mac执行 `make linux-stop` 释放虚拟机运行资源。

已停止的环境可以在Mac执行 `colima start led-practice --activate=false` 再启动。首次安装及软件包下载需要网络；后续已构建环境可复用。系统与基础镜像版本、构建日志以本地证据为准；Dockerfile固定发行版系列，未声称按镜像摘要和包版本完全锁定。

## 当前任务

环境构建与测试由助手负责。用户第一项仅在Linux终端运行uname -s -m并与Darwin arm64比较，不需要独立安装整套工具。
