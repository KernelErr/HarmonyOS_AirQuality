# Air Quality Detector on HarmonyOS

使用鸿蒙系统开发的嵌入式空气质量检测器，每两秒读取传感器数据并通过MQTT协议上传到OneNET平台，请将代码放于`applications/BearPi/BearPi-HM_Nano/sample/air_quality`。

硬件清单：

- BearPi-HM_nano
- CJ702 (空气质量传感器)

## 编译&烧录

安装[hpm](https://www.npmjs.com/package/@ohos/hpm-cli)进行编译和项目管理，利用HiBurn软件烧录即可，环境搭建可参考[鸿蒙开发板初探——BearPi-HM Nano](https://www.lirui.tech/2020/dca553965da4/)。

```bash
hpm dist
```

