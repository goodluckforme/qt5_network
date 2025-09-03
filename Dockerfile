# 使用Ubuntu 22.04作为基础镜像
FROM ubuntu:22.04

# 设置非交互模式，避免安装过程中出现交互提示
ENV DEBIAN_FRONTEND=noninteractive

# 更新软件源并安装基础工具
RUN apt-get update && apt-get install -y \
    sudo \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg \
    lsb-release \
    software-properties-common \
    build-essential \
    git \
    wget \
    vim \
    locales \
    fonts-wqy-microhei \
    language-pack-zh-hans \
    && rm -rf /var/lib/apt/lists/*


# 安装Qt开发环境
RUN apt-get update && apt-get install -y \
    qtcreator \
    qtbase5-dev \
    qt5-qmake \
    qtbase5-dev-tools \
    qttools5-dev \
    qttools5-dev-tools \
    libqt5svg5-dev \
    qtmultimedia5-dev \
    libqt5multimedia5-plugins \
    qtwebengine5-dev \
    && rm -rf /var/lib/apt/lists/*

# 创建一个非root用户，避免权限问题
RUN useradd -m maqi && echo "maqi:maqi" | chpasswd && adduser maqi sudo
USER maqi
WORKDIR /home/maqi

# 设置环境变量
ENV PATH="/usr/lib/x86_64-linux-gnu/qt5/bin:${PATH}"

# 暴露Qt Creator默认使用的端口（如果需要远程调试）
EXPOSE 10000-10010

# 设置中文环境
USER root
RUN locale-gen zh_CN.UTF-8
ENV LANG zh_CN.UTF-8
ENV LC_ALL zh_CN.UTF-8
# 切換普通用戶  
USER maqi
 
# 容器启动时运行Qt Creator
CMD ["qtcreator"]
    