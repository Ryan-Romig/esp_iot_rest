FROM ubuntu:22.04

RUN apt-get update \
&& apt-get install git curl cmake protobuf-compiler wget flex bison gperf python3 python-is-python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 -y && \
mkdir -p /Sdk/esp && cd /Sdk/esp && git clone -b v4.4.2 --recursive https://github.com/espressif/esp-idf.git esp-idf && \
cd /Sdk/esp/esp-idf &&  chmod +x install.sh && chmod +x export.sh && \
./install.sh esp32 && \
mkdir -p /firmware && \
# git clone https://github.com/Ryan-Romig/esp_iot_rest.git /firmware && \
wget -qO- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash
RUN [ -s "/root/.nvm/nvm.sh" ] && \. "/root/.nvm/nvm.sh"  && \
[ -s "/root/.nvm/bash_completion" ] && \. "/root/.nvm/bash_completion"  && \
nvm install 18
COPY * /firmware
WORKDIR /firmware
CMD ["/bin/bash", "-c", "export IDF_PATH=/Sdk/esp/esp-idf && . /Sdk/esp/esp-idf/export.sh && /bin/bash"]

