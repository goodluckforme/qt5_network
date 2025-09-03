# 构建Qt开发环境镜像
docker build -t qt-dev-env:latest .

# 查看构建的镜像
docker images | grep qt-dev-env

# 确保当前目录存在qt5_network文件夹，用于存放项目文件
mkdir -p qt5_network

# 运行Qt开发环境容器
# --rm: 容器退出后自动删除
# -it: 交互式终端
# -e DISPLAY: 共享主机的显示
# -v /tmp/.X11-unix:/tmp/.X11-unix: 挂载X11 socket，允许图形界面显示
# -v $(pwd)/qt5_network:/home/maqi/qt5_network: 将本地项目目录挂载到容器中
docker run --rm -it \
    -e DISPLAY=$DISPLAY \
    -e LANG=zh_CN.UTF-8 \
    -e LC_ALL=zh_CN.UTF-8 \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v $(pwd)/qt5_network:/home/maqi/qt5_network \
    --name qt-dev-container \
    qt-dev-env:latest